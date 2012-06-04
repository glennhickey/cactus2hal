/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.cactus
 */

#ifndef _CACTUSHALCONVERTER_H
#define _CACTUSHALCONVERTER_H

#include <string>
#include <fstream>
#include <map>

#include "hal.h"
#include "cactusHalScanDimensions.h"
#include "cactusDbWrapper.h"


/**
 * Convert cactus .hal into proper hal database
 */
class CactusHalConverter : protected CactusHalScanner
{
public:

   CactusHalConverter();
   ~CactusHalConverter();

   void convert(const std::string& halFilePath,
                const std::string& cactusDbString,
                hal::AlignmentPtr alignment);

protected:

   void clear();
   void convertGenomes();
   void convertSegments();
   void setGenomeDimensions(hal::Genome* genome, 
                            const std::vector<hal::Sequence::Info>* dimensions,
                            bool existingGenome);
   void setGenomeSequenceStrings(hal::Genome* genome);

   void scanSequence(CactusHalSequence& sequence);
   void scanTopSegment(CactusHalTopSegment& topSegment);
   void scanBottomSegment(CactusHalBottomSegment& botSegment);
   void updateParalogy();
   void updateParseInfo(hal::BottomSegment* bottomSeg);
   void scanEndOfFile(){}

protected:
   
   std::string _halFilePath;
   std::string _cactusDbString;
   std::string _outDbPath;
   std::string _treeString;
   hal::AlignmentPtr _alignment;   
   CactusDbWrapper _cactusDb;
   hal::SequenceIteratorPtr _sequenceIterator;
   hal::TopSegmentIteratorPtr _topIterator;
   hal::BottomSegmentIteratorPtr _bottomIterator;
   size_t _sequenceIndex;
   bool _active;
   
   struct SeqLess { bool operator()(hal::Sequence* seq1,
                                    hal::Sequence* seq2); };
   typedef std::map<hal::Sequence*, size_t, SeqLess> SeqMap;
   typedef std::vector<hal::Sequence*> SeqVec;
   typedef std::pair<size_t, hal_index_t> SegRef;
   typedef std::set<SegRef> SegRefSet;
   typedef std::map<int64_t, SegRefSet> ParentMap;
   typedef std::map<std::string, size_t> ChildIdxMap;
   typedef std::map<const hal::Genome*, hal_index_t> DupCache;

   // go from hal sequence to unique sequence index
   SeqMap _sequenceMap;
   
   // go from unique sequence index to hal sequence
   SeqVec _sequenceVec;

   // map a parent (bottom) segment to all segments descended from mit
   ParentMap _parentMap;

   // convenience for checking which child number a genome is
   ChildIdxMap _childIdxMap;

   // keep track of last current duplicate segment per genome
   DupCache _dupCache;
};

// compare sequences by <genome name, sequence name> for purposes
// of map indexing (maybe i should be using pointers instead of 
// strings here...)
inline bool 
CactusHalConverter::SeqLess::operator()(hal::Sequence* seq1,
                                        hal::Sequence* seq2)
{
  std::string gen1 = seq1->getGenome()->getName();
  std::string gen2 = seq2->getGenome()->getName();
  if (gen1 < gen2)
  {
    return true;
  }
  else if (gen2 > gen1)
  {
    return false;
  }
  else
  {
    return seq1->getName() < seq2->getName();
  }
}

#endif
