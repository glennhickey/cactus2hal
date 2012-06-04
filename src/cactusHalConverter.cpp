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
                                 const string& cactusDbString,
                                 AlignmentPtr alignment)
{
  clear();
  _halFilePath = halFilePath;
  _cactusDbString = cactusDbString;
  _alignment = alignment;
  _cactusDb.open(cactusDbString);
  _treeString = _cactusDb.getTree();
  convertGenomes();
  convertSegments();
  clear();
}

void CactusHalConverter::clear()
{
  _alignment = AlignmentPtr();
  _cactusDb.close();
  _sequenceIterator = SequenceIteratorPtr();
  _topIterator = TopSegmentIteratorPtr();
  _bottomIterator = BottomSegmentIteratorPtr();
  _sequenceIndex = 0;
  _sequenceMap.clear();
  _sequenceVec.clear();
  _parentMap.clear();
  _childIdxMap.clear();
  _dupCache.clear();
  _active = false;
}

void CactusHalConverter::convertGenomes()
{
  vector<pair<Genome*, bool> > inputGenomes;

  //pass 1: scan the genome dimensions from the .hal
  CactusHalScanDimensions dimensionScanner;
  dimensionScanner.scanDimensions(_halFilePath);
  const GenMapType* genMap = dimensionScanner.getDimensionsMap();
  
  //iterate over input tree, using map to set the information for each
  //genome
  char* ctreeString = const_cast<char*>(_treeString.c_str());
  stTree* root = stTree_parseNewickString(ctreeString);
  assert(_cactusDb.isOutgroup(stTree_getLabel(root)) == false);
  deque<stTree*> bfQueue;
  bfQueue.push_back(root);
  vector<pair<Genome*, bool> > readGenomes;
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
          if (node != root)
          {
            throw hal_exception(string("Fatal Tree error.  converter"
                                       "does not support adding the same"
                                       "leaf twice:") + genome->getName());
          }
          existingGenome = true;
        }
      }
      readGenomes.push_back(pair<Genome*, bool>(genome, existingGenome));
    }

    int32_t numChildren = stTree_getChildNumber(node);
    for (int32_t i = 0; i < numChildren; ++i)
    {
      stTree* child = stTree_getChild(node, i);
      if (_cactusDb.isOutgroup(stTree_getLabel(child)) == false)
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
  vector<string> children = _alignment->getChildNames(
    _alignment->getRootName());
  for (size_t i = 0; i < children.size(); ++i)
  {
    _childIdxMap.insert(pair<string, size_t>(children[i], i));
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
      updateDimensions.push_back(update);
    }
    genome->updateBottomDimensions(updateDimensions);
  }
}

void CactusHalConverter::setGenomeSequenceStrings(Genome* genome)
{
  SequenceIteratorPtr sequenceIterator = genome->getSequenceIterator();
  SequenceIteratorConstPtr end = genome->getSequenceEndIterator();
  for (; sequenceIterator != end; sequenceIterator->toNext())
  {
    hal::Sequence* sequence = sequenceIterator->getSequence();
    char* sequenceString = _cactusDb.getSequence(genome->getName(),
                                                 sequence->getName());
    sequence->setString(sequenceString);
    free(sequenceString);
  }
}

void CactusHalConverter::convertSegments()
{
  scan(_halFilePath);
}

void CactusHalConverter::scanSequence(CactusHalSequence& sequence)
{
  _active = _cactusDb.isOutgroup(sequence._event) == false;
  if (_active == false)
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
  hal::Sequence* halSequence = genome->getSequence(sequence._name);
  if (halSequence == NULL)
  {
    throw hal_exception(string("Cannot locate sequence") + sequence._name +
                        "in the genome " + sequence._event);
  }
  _sequenceIndex = _sequenceMap.size();
  _sequenceMap.insert(pair<hal::Sequence*, hal_index_t>(
                        halSequence, _sequenceIndex));
  _sequenceVec.push_back(halSequence);
  if (sequence._isBottom == true)
  {
    if (genome->getNumTopSegments() > 0)
    {
      _topIterator = halSequence->getTopSegmentIterator();
    }
    _bottomIterator = halSequence->getBottomSegmentIterator();
  }
  else
  {
    _topIterator = halSequence->getTopSegmentIterator();
  }
}

void CactusHalConverter::scanTopSegment(CactusHalTopSegment& topSegment)
{
  if (_active == false)
  {
    return;
  }
  TopSegment* topSeg = _topIterator->getTopSegment();
  hal_index_t startPos = 0;
  if (topSeg->getArrayIndex() > 0)
  {
    _topIterator->toLeft();
    topSeg = _topIterator->getTopSegment();
    startPos = topSeg->getStartPosition() + topSeg->getLength();
    _topIterator->toRight();
    topSeg = _topIterator->getTopSegment();
  }
  topSeg->setStartPosition(startPos);
  topSeg->setLength(topSegment._length);
  topSeg->setParentReversed(topSegment._reversed);
  topSeg->setParentIndex(NULL_INDEX);
  topSeg->setBottomParseIndex(NULL_INDEX);
  topSeg->setBottomParseOffset(0);
  topSeg->setNextParalogyIndex(NULL_INDEX);
  SegRef segRef(_sequenceIndex, topSeg->getArrayIndex());
  ParentMap::iterator mapIt = _parentMap.find(topSegment._parent);
  if (mapIt == _parentMap.end())
  {
    SegRefSet srs;
    srs.insert(segRef);
    _parentMap.insert(pair<int64_t, SegRefSet>(topSegment._parent, srs));
  }
  else
  {
    mapIt->second.insert(segRef);
  }
  _topIterator->toRight();
}

void CactusHalConverter::scanBottomSegment(CactusHalBottomSegment& botSegment)
{
  if (_active == false)
  {
    return;
  }
  BottomSegment* bottomSeg = _bottomIterator->getBottomSegment();
  hal_index_t startPos = 0;
  if (bottomSeg->getArrayIndex() > 0)
  {
    _bottomIterator->toLeft();
    bottomSeg = _bottomIterator->getBottomSegment();
    startPos = bottomSeg->getStartPosition() + bottomSeg->getLength();
    _bottomIterator->toRight();
    bottomSeg = _bottomIterator->getBottomSegment();
  }
  bottomSeg->setStartPosition(startPos);
  bottomSeg->setLength(botSegment._length);
  bottomSeg->setNextParalogyIndex(NULL_INDEX);
  for (hal_size_t i = 0; i < bottomSeg->getNumChildren(); ++i)
  {
    bottomSeg->setChildIndex(i, NULL_INDEX);
    bottomSeg->setChildReversed(i, false);
  }
  updateParseInfo(bottomSeg);
  
  ParentMap::iterator parIt = _parentMap.find(botSegment._name);

  cout << "looking for parit in map of size " << _parentMap.size() << " and res "
       << bool(parIt != _parentMap.end()) << endl;

  if (parIt != _parentMap.end())
  {
    Genome* prevGenome = NULL;
    Genome* curGenome = NULL;
    hal::Sequence* curSequence = NULL;
    TopSegment* topSegment = NULL;
    SegRefSet& refSet = parIt->second;
    size_t curGenomeIndex = 0;
    assert(refSet.empty() == false);
    SegRefSet::iterator setIt = refSet.begin();
  
    for (; setIt != refSet.end(); ++setIt)
    {
      const SegRef& segRef = *setIt;
      curSequence = _sequenceVec[segRef.first];
      curGenome = curSequence->getGenome();
      assert(curGenome != NULL);
      curGenomeIndex = _childIdxMap.at(curGenome->getName());
      assert(segRef.second < curGenome->getNumTopSegments());
      _topIterator = curGenome->getTopSegmentIterator(segRef.second);
      topSegment = _topIterator->getTopSegment();
      if (curGenome != prevGenome)
      {
        // this is the first child from curGenome.  We add an
        // edge here and all subsequent children are duplicates
        assert(topSegment->getParentIndex() == NULL_INDEX);
        topSegment->setParentIndex(segRef.second);
        bottomSeg->setChildIndex(curGenomeIndex, topSegment->getArrayIndex());
        bottomSeg->setChildReversed(curGenomeIndex, 
                                    topSegment->getParentReversed());
        bottomSeg->setNextParalogyIndex(NULL_INDEX);
        topSegment->setNextParalogyIndex(NULL_INDEX);
        topSegment->setNextParalogyReversed(false);
      }
      else
      {
        // there is already a child edge for the bottom segment.  so the
        // top segment must get added as a paralogy
        updateParalogy();
      }
      prevGenome = curGenome;
    }
  
    // that's the last time we'll ever add an edge to the bottom segment
    _parentMap.erase(parIt);
  }
  _bottomIterator->toRight();
}

void CactusHalConverter::updateParalogy()
{
  TopSegment* childSegment = _topIterator->getTopSegment();
  Genome* childGenome = childSegment->getGenome();
  TopSegmentIteratorPtr prevIt;
  TopSegment* prevSegment = NULL;

  // find the previous paralogy (which must exist)
  DupCache::iterator cacheIt = _dupCache.find(childSegment->getGenome());
  if (cacheIt == _dupCache.end())
  {
    prevIt = childGenome->getTopSegmentIterator();
    prevIt->toChild(_bottomIterator, 
                    _childIdxMap.at(childGenome->getName()));
    prevSegment = prevIt->getTopSegment();
    prevSegment->setNextParalogyIndex(prevSegment->getArrayIndex());
  }
  else
  {
    prevIt = childGenome->getTopSegmentIterator(cacheIt->second);
    prevSegment = prevIt->getTopSegment();
  }

  assert(prevSegment->getArrayIndex() <= childSegment->getArrayIndex());
  
  // append the new paralogy and circularize list
  childSegment->setNextParalogyIndex(prevSegment->getNextParalogyIndex());
  prevSegment->setNextParalogyIndex(childSegment->getNextParalogyIndex());
  prevSegment->setNextParalogyReversed(childSegment->getParentReversed() !=
                                       prevSegment->getParentReversed());
  
  // update cache
  if (cacheIt == _dupCache.end())
  {
    _dupCache.insert
       (pair<const Genome*, hal_index_t>(childSegment->getGenome(),
                                         childSegment->getArrayIndex()));
  }
  else
  {
    cacheIt->second =  childSegment->getArrayIndex();
  }
}

void CactusHalConverter::updateParseInfo(BottomSegment *bottomSeg)
{
  if (bottomSeg->getGenome()->getNumTopSegments() > 0)
  {
    // bring the top iterator rightward until its start position is 
    // contained in the bottom segment
    TopSegment* topSeg = _topIterator->getTopSegment();
    assert(bottomSeg->getStartPosition() >= topSeg->getStartPosition());
    while (bottomSeg->getStartPosition() >= topSeg->getStartPosition() +
           topSeg->getLength())
    {
      _topIterator->toRight();
      topSeg = _topIterator->getTopSegment();
    }
    assert(bottomSeg->getStartPosition() < topSeg->getStartPosition() +
           topSeg->getLength() &&
           bottomSeg->getStartPosition() >= topSeg->getStartPosition());
    bottomSeg->setTopParseIndex(topSeg->getArrayIndex());
    bottomSeg->setTopParseOffset(bottomSeg->getStartPosition() - 
                                 topSeg->getStartPosition());

    topSeg = NULL; // typo prevention

    // search for and update any top segments that may use the bottom
    // segment as a parse index
    TopSegmentIteratorPtr topIterator2 = _topIterator->copy();
    TopSegment* topSeg2 = topIterator2->getTopSegment();

    // scan to the leftmost candidate
    while (topSeg2->getStartPosition() + topSeg2->getLength() 
           >= bottomSeg->getStartPosition())
    {
      topIterator2->toLeft();
      topSeg2 = topIterator2->getTopSegment();
    }
    topIterator2->toRight();
    topSeg2 = topIterator2->getTopSegment();

    // scan right, updating any top segments whose start position overlaps
    // with the bottom segment
    while (topSeg2->getStartPosition() >= bottomSeg->getStartPosition() &&
           topSeg2->getStartPosition() < bottomSeg->getStartPosition() +
           bottomSeg->getLength())
    {
      topSeg2->setBottomParseIndex(bottomSeg->getArrayIndex());
      topSeg2->setBottomParseOffset(topSeg2->getStartPosition() - 
                                    bottomSeg->getStartPosition());
      topIterator2->toRight();
      topSeg2 = topIterator2->getTopSegment();
    }
  }
  else
  {
     bottomSeg->setTopParseIndex(NULL_INDEX);
     bottomSeg->setTopParseOffset(0);
  }
}
