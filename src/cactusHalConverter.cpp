/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <deque>
#include <sstream>

#include "cactusHalConverter.h"
extern "C" {
#include "sonLibTree.h"
}

using namespace std;
using namespace hal;


CactusHalConverter::CactusHalConverter()
{

}

CactusHalConverter::~CactusHalConverter()
{
 
}

void CactusHalConverter::convert(const string& halFilePath,
                                 const string& faFilePath,
                                 const string& treeString,
                                 AlignmentPtr alignment,
                                 const vector<string>& outgroups)
{
  clear();
  _halFilePath = halFilePath;
  _faFilePath = faFilePath;
  _outgroups = set<string>(outgroups.begin(), outgroups.end());
  _alignment = alignment;
  _treeString = treeString;
  convertGenomes();
  convertSegments();
  updateRootParseInfo();
  clear();
}

void CactusHalConverter::clear()
{
  _alignment = AlignmentPtr();
  _sequenceIterator = SequenceIteratorPtr();
  _topIterator = TopSegmentIteratorPtr();
  _bottomIterator = BottomSegmentIteratorPtr();
  _parentIterator = BottomSegmentIteratorPtr();
  _bottomParseIterator = BottomSegmentIteratorPtr();
  _sequenceIndex = 0;
  _nameMap.clear();
  _childIdxMap.clear();
  _dupCache.clear();
  _skipSequences.clear();
  _active = false;
  _outgroups.clear();
}

void CactusHalConverter::convertGenomes()
{
  vector<pair<Genome*, bool> > inputGenomes;

  //pass 1: scan the genome dimensions from the .hal
  _dimensionScanner.scanDimensions(_halFilePath, _faFilePath);
  const GenMapType* genMap = _dimensionScanner.getDimensionsMap();
  
  //iterate over input tree, using map to set the information for each
  //genome
  char* ctreeString = const_cast<char*>(_treeString.c_str());
  stTree* root = stTree_parseNewickString(ctreeString);
  assert(_outgroups.find(stTree_getLabel(root)) == _outgroups.end());
  deque<stTree*> bfQueue;
  bfQueue.push_back(root);
  vector<pair<Genome*, bool> > readGenomes;
  string rootName;
  while (bfQueue.empty() == false)
  {
    stTree* node = bfQueue.front();
    Genome* genome;
    string name = stTree_getLabel(node);
    bool existingGenome = false;
    bfQueue.pop_front();
    GenMapType::const_iterator mapIt = genMap->find(name);
    if (mapIt != genMap->end())
    {
      //case 1: add a new root to the alignment
      if (_alignment->getNumGenomes() == 0)
      {
        rootName = stTree_getLabel(node);
        genome = _alignment->addRootGenome(stTree_getLabel(node));
      }
      else
      {
        genome = _alignment->openGenome(stTree_getLabel(node));
        //case 2: add a new leaf to the alignment
        if (genome == NULL)
        {
          stTree* parent = stTree_getParent(node);
          double length = stTree_getBranchLength(node);
          genome = _alignment->addLeafGenome(stTree_getLabel(node),
                                             stTree_getLabel(parent),
                                             length);
        }
        //case 3: update an existing internal node, which must be
        //the root of the given tree.
        else
        {
          if (stTree_getChildNumber(node) == 0 || rootName.empty() == false)
          {
            throw hal_exception(string("Fatal Tree error.  converter "
                                       "does not support adding more than one non-"
                                       "leaf:") + genome->getName());
          }
          rootName = stTree_getLabel(node);
          existingGenome = true;
        }
      }
      readGenomes.push_back(pair<Genome*, bool>(genome, existingGenome));
    }

    int32_t numChildren = stTree_getChildNumber(node);
    for (int32_t i = 0; i < numChildren; ++i)
    {
      stTree* child = stTree_getChild(node, i);
      if (_outgroups.find(stTree_getLabel(child)) == _outgroups.end())
      {
        bfQueue.push_back(child);
      }
    }
  }
  
  // set the dimensions of all the added genome AFTER the tree has 
  // been imported
  for (size_t i = 0; i < readGenomes.size(); ++i)
  {
    Genome* genome = readGenomes[i].first;
    bool existingGenome = readGenomes[i].second;
    GenMapType::const_iterator mapIt = genMap->find(genome->getName());
    assert(mapIt != genMap->end());
    const vector<hal::Sequence::Info>* dims = mapIt->second;    

    // set / update the genome's dimensions
    setGenomeDimensions(genome, dims, existingGenome);
    if (existingGenome == false)
    {
      // copy in the dna sequence from the cactus database
      setGenomeSequenceStrings(genome);
    }
  }

  // keep an index of the child indexes
  vector<string> children = _alignment->getChildNames(rootName);
  for (size_t i = 0; i < children.size(); ++i)
  {
    _childIdxMap.insert(pair<Genome*, size_t>(
                          _alignment->openGenome(children[i]), i));
  }

  stTree_destruct(root);
}

void CactusHalConverter::setGenomeDimensions(
  Genome* genome, 
  const vector<hal::Sequence::Info>* dimensions, bool existingGenome)
{
  if (existingGenome == false)
  {
    genome->setDimensions(*dimensions);
  }
  else
  {
    vector<hal::Sequence::UpdateInfo> updateDimensions;
    updateDimensions.reserve(dimensions->size());
    for (size_t i = 0; i < dimensions->size(); ++i)
    {
      const hal::Sequence::Info& info = dimensions->at(i);
      hal::Sequence::UpdateInfo update(info._name, info._numBottomSegments);
      // .hal files can contain empty sequences in bottom genomes that 
      // were not present in the top view. 
      // this causes an exception in the updateBottomDimensions which is
      // not alowed to add new sequences.
      // we get around this for now by simply stripping out empty sequences
      // that would cause such a problem. 
      if (info._numBottomSegments > 0 || 
          genome->getSequence(info._name) != NULL)
      {
        updateDimensions.push_back(update);        
      }
      else
      {
        _skipSequences.insert(info._name);
      }
    }
    genome->updateBottomDimensions(updateDimensions);
  }
}

void CactusHalConverter::setGenomeSequenceStrings(Genome* genome)
{
  string buffer;
  SequenceIteratorPtr sequenceIterator = genome->getSequenceIterator();
  SequenceIteratorConstPtr end = genome->getSequenceEndIterator();
  bool castWarning = false;
  for (; sequenceIterator != end; sequenceIterator->toNext())
  {
    hal::Sequence* sequence = sequenceIterator->getSequence();
    if (sequence->getSequenceLength() > 0)
    {
      _dimensionScanner.getSequence(genome->getName(), sequence->getName(), 
                                    buffer);
      // HAL does not presently support some conventional tags such
      // as in http://en.wikipedia.org/wiki/Nucleic_acid_notation
      // so we cast them all to N
      for (size_t i = 0; i < buffer.length(); ++i)
      {
        switch (buffer[i]) {
        case 'k': case 'm': case 'r': case 'y': case 'u': case 's': case 'w':
        case 'b': case 'd': case 'h': case 'v':
          if (castWarning == false)
          {
            cerr << "cactus2hal warning: converting " << buffer[i] << " to n in "
                 << "sequence " << sequence->getName() << " in genome "
                 << genome->getName() << endl; 
            castWarning = true;
          }
          break;
          buffer[i] = 'n'; 
        case 'K': case 'M': case 'R': case 'Y': case 'U': case 'S': case 'W':
        case 'B': case 'D': case 'H': case 'V':
          if (castWarning == false)
          {
            cerr << "cactus2hal warning: converting " << buffer[i] << " to N in "
                 << "sequence " << sequence->getName() << " in genome "
                 << genome->getName() << endl; 
            castWarning = true;
          }
          buffer[i] = 'N'; 
          break;
        default: break;
        }
      }
      sequence->setString(buffer);
    }
  }
}

void CactusHalConverter::convertSegments()
{
  scan(_halFilePath);
}

void CactusHalConverter::scanSequence(CactusHalSequence& sequence)
{
  _active = _outgroups.find(sequence._event) == _outgroups.end();
  if (_active == false || 
      _skipSequences.find(sequence._name) != _skipSequences.end())
  {
    return;
  }
  // NEED CHECK HERE FOR THE CURRENT ITERATORS
  Genome* genome = _alignment->openGenome(sequence._event);
  if (genome == NULL)
  {
    throw hal_exception(string("Cannot locate genome") + sequence._event +
                        " in the database");
  }
  _halSequence = genome->getSequence(sequence._name);
  if (_halSequence == NULL)
  {
    throw hal_exception(string("Cannot locate sequence") + sequence._name +
                        "in the genome " + sequence._event);
  }
  
  if (sequence._isBottom == true)
  {
    _bottomIterator = _halSequence->getBottomSegmentIterator();
  }
  else
  {
    if (genome->getNumBottomSegments() > 0)
    {
      _parentIterator = _halSequence->getBottomSegmentIterator();
      _bottomParseIterator = _halSequence->getBottomSegmentIterator();
    }
    _topIterator = _halSequence->getTopSegmentIterator();
  }
}

void CactusHalConverter::scanBottomSegment(CactusHalBottomSegment& botSegment)
{
  if (_active == false)
  {
    return;
  }
  BottomSegment* bottomSeg = _bottomIterator->getBottomSegment();
  hal_index_t startPos = _halSequence->getStartPosition() + botSegment._start;
  bottomSeg->setStartPosition(startPos);
  bottomSeg->setLength(botSegment._length);
  bottomSeg->setNextParalogyIndex(NULL_INDEX);
  for (hal_size_t i = 0; i < bottomSeg->getNumChildren(); ++i)
  {
    bottomSeg->setChildIndex(i, NULL_INDEX);
    bottomSeg->setChildReversed(i, false);
  }
  bottomSeg->setTopParseIndex(NULL_INDEX);
  bottomSeg->setTopParseOffset(0);
  
  _nameMap.insert(pair<Name, hal_index_t>(botSegment._name,
                                          bottomSeg->getArrayIndex()));
  
  _bottomIterator->toRight();
}

void CactusHalConverter::scanTopSegment(CactusHalTopSegment& topSegment)
{
  if (_active == false)
  {
    return;
  }
  TopSegment* topSeg = _topIterator->getTopSegment();
  assert (topSeg->getArrayIndex() < topSeg->getGenome()->getNumTopSegments());
  hal_index_t startPos = _halSequence->getStartPosition() + topSegment._start;
  topSeg->setStartPosition(startPos);
  topSeg->setLength(topSegment._length);
  topSeg->setParentReversed(topSegment._reversed);
  topSeg->setParentIndex(NULL_INDEX);
  topSeg->setBottomParseIndex(NULL_INDEX);
  topSeg->setBottomParseOffset(0);
  topSeg->setNextParalogyIndex(NULL_INDEX);

  if (topSegment._parent != NULL_NAME)
  {
    NameMap::iterator nameMapIt = _nameMap.find(
      topSegment._parent);
    if (nameMapIt == _nameMap.end())
    {
      stringstream ss;
      ss << "Fatal error adding a top segment with parent " << topSegment._parent
         << ": no bottom segment with this name has been scanned. "
         "Note that bottom segments must be listed before top segments in the"
         " hal file";
      throw hal_exception(ss.str());
    }
    Genome* parGenome = topSeg->getGenome()->getParent();
    assert(parGenome != NULL);
    _parentIterator = parGenome->getBottomSegmentIterator(
      nameMapIt->second);
    updateDescent();
  }
  updateParseInfo();

  _topIterator->toRight();
}

// note: should be run while scanning topsegments
// topIterator and bottomIterator member variables must be set
void CactusHalConverter::updateDescent()
{
  TopSegment* topSegment = _topIterator->getTopSegment();
  BottomSegment* bottomSegment = _parentIterator->getBottomSegment();

  // set the edge from child to parent (note reversed flag
  // was already set)
  assert(bottomSegment->getLength() == topSegment->getLength());
  topSegment->setParentIndex(bottomSegment->getArrayIndex());

  // set the edge from parent to child
  assert(_childIdxMap.find(topSegment->getGenome()) != _childIdxMap.end());
  size_t genIdx = _childIdxMap.find(topSegment->getGenome())->second;
  hal_index_t currentChildIndex = bottomSegment->getChildIndex(genIdx);
  if (currentChildIndex == NULL_INDEX)
  {
    bottomSegment->setChildIndex(genIdx, topSegment->getArrayIndex());
    bottomSegment->setChildReversed(genIdx, topSegment->getParentReversed());
  }
  else
  {
    // edge alreay exists, so we have to add the top segment as a 
    // duplication (paralogy)
    Genome* topGenome = topSegment->getGenome();
    TopSegmentIteratorPtr firstChild = 
       topGenome->getTopSegmentIterator(currentChildIndex);
    assert(firstChild->getTopSegment()->getStartPosition() < 
           topSegment->getStartPosition());
    TopSegment* firstSegment = firstChild->getTopSegment();

    // circularlize the linked list
    topSegment->setNextParalogyIndex(firstSegment->getArrayIndex());
    topSegment->setNextParalogyReversed(firstSegment->getParentReversed() !=
                                        topSegment->getParentReversed());

    DupCache::iterator cacheIt = _dupCache.find(
      GenCoord(topGenome, firstSegment->getArrayIndex()));

    // first duplication: add it directly
    if (firstSegment->getNextParalogyIndex() == NULL_INDEX)
    {
      assert(cacheIt == _dupCache.end());
      firstSegment->setNextParalogyIndex(topSegment->getArrayIndex());
      firstSegment->setNextParalogyReversed(firstSegment->getParentReversed() !=
                                            topSegment->getParentReversed());
    }
    // otherwise, find it in the map
    else
    {
      assert(cacheIt != _dupCache.end());
      hal_index_t prevIndex = cacheIt->second;
      TopSegmentIteratorPtr prevIterator = topGenome->getTopSegmentIterator(
        prevIndex);
      TopSegment* prevSegment = prevIterator->getTopSegment();
      assert(prevSegment->getArrayIndex() > firstSegment->getArrayIndex());
      assert(prevSegment->getArrayIndex() < topSegment->getArrayIndex());
      assert(prevSegment->getParentIndex() == bottomSegment->getArrayIndex());
      assert(prevSegment->getNextParalogyIndex() == 
             firstSegment->getArrayIndex());
      prevSegment->setNextParalogyIndex(topSegment->getArrayIndex());
      prevSegment->setNextParalogyReversed(prevSegment->getParentReversed() !=
                                           topSegment->getParentReversed());
    }
    // add the segment's index to the map
    if (cacheIt == _dupCache.end())
    {
      _dupCache.insert(pair<GenCoord, hal_index_t>(
                         GenCoord(topGenome, firstSegment->getArrayIndex()),
                         topSegment->getArrayIndex()));
    }
    else
    {
      cacheIt->second = topSegment->getArrayIndex();
    }
  }
}

// note: should be run while scanning topsegments
// topIterator and bottomIterator member variables must be set
void CactusHalConverter::updateParseInfo()
{
  TopSegment* topSeg = _topIterator->getTopSegment();
  if (topSeg->getGenome()->getNumBottomSegments() > 0)
  {
    // bring the bottom iterator rightward until its start position is 
    // contained in the top segment
    BottomSegment* bottomSeg = _bottomParseIterator->getBottomSegment();
    assert(topSeg->getStartPosition() >= bottomSeg->getStartPosition());
    while (topSeg->getStartPosition() >= bottomSeg->getStartPosition() +
           (hal_index_t)bottomSeg->getLength())
    {
      _bottomParseIterator->toRight();
      bottomSeg = _bottomParseIterator->getBottomSegment();
    }
    assert(topSeg->getStartPosition() < bottomSeg->getStartPosition() +
           bottomSeg->getLength() &&
           topSeg->getStartPosition() >= bottomSeg->getStartPosition());
    topSeg->setBottomParseIndex(bottomSeg->getArrayIndex());
    topSeg->setBottomParseOffset(topSeg->getStartPosition() - 
                                 bottomSeg->getStartPosition());

    bottomSeg = NULL; // typo prevention

    // search for and update any bottom segments that may use the top
    // segment as a parse index
    BottomSegmentIteratorPtr bottomParseIterator2 = _bottomParseIterator->copy();
    BottomSegment* bottomSeg2 = bottomParseIterator2->getBottomSegment();

    // scan to the leftmost candidate
    while (bottomSeg2->getStartPosition() + (hal_index_t)bottomSeg2->getLength() 
           >= topSeg->getStartPosition())
    {
      bottomParseIterator2->toLeft();
      bottomSeg2 = bottomParseIterator2->getBottomSegment();
    }
    bottomParseIterator2->toRight();
    bottomSeg2 = bottomParseIterator2->getBottomSegment();

    // scan right, updating any bottom segments whose start position overlaps
    // with the top segment
    while (bottomSeg2->getStartPosition() >= topSeg->getStartPosition() &&
           bottomSeg2->getStartPosition() < topSeg->getStartPosition() +
           (hal_index_t)topSeg->getLength())
    {
      bottomSeg2->setTopParseIndex(topSeg->getArrayIndex());
      bottomSeg2->setTopParseOffset(bottomSeg2->getStartPosition() - 
                                    topSeg->getStartPosition());
      bottomParseIterator2->toRight();
      bottomSeg2 = bottomParseIterator2->getBottomSegment();
    }
  }
  else
  {
     topSeg->setBottomParseIndex(NULL_INDEX);
     topSeg->setBottomParseOffset(0);
  }
}

void CactusHalConverter::updateRootParseInfo()
{
  // just a terrible way to get the root genome but I'm 
  // in a rush!!!
  Genome* rootGenome = _childIdxMap.begin()->first->getParent();
  assert(rootGenome != NULL);
  
  BottomSegmentIteratorPtr bottomIterator = rootGenome->getBottomSegmentIterator();
  TopSegmentIteratorPtr topIterator = rootGenome->getTopSegmentIterator();
  BottomSegmentIteratorConstPtr bend = rootGenome->getBottomSegmentEndIterator();
  TopSegmentIteratorConstPtr tend = rootGenome->getTopSegmentEndIterator();

  while (bottomIterator != bend && topIterator != tend)
  {
    bool bright = false;
    bool tright = false;
    BottomSegment* bseg = bottomIterator->getBottomSegment();
    TopSegment* tseg = topIterator->getTopSegment();
    hal_index_t bstart = bseg->getStartPosition();
    hal_index_t bend = bstart + (hal_index_t)bseg->getLength();
    hal_index_t tstart = tseg->getStartPosition();
    hal_index_t tend = tstart + (hal_index_t)tseg->getLength();
    
    if (bstart >= tstart && bstart < tend)
    {
      bseg->setTopParseIndex(tseg->getArrayIndex());
      bseg->setTopParseOffset(bstart - tstart);
    }
    if (bend <= tend || bstart == bend)
    {
      bright = true;
    }
        
    if (tstart >= bstart && tstart < bend)
    {
      tseg->setBottomParseIndex(bseg->getArrayIndex());
      tseg->setBottomParseOffset(tstart - bstart);
    }
    if (tend <= bend || tstart == tend)
    {
      tright = true;
    }

    assert(bright || tright);
    if (bright == true)
    {
      bottomIterator->toRight();
    }
    if (tright == true)
    {
      topIterator->toRight();
    }
  }
}
