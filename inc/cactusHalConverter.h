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
   void updateDescent();
   void updateParseInfo();
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
   hal::BottomSegmentIteratorPtr _parentIterator;
   hal::BottomSegmentIteratorPtr _bottomParseIterator;
   size_t _sequenceIndex;
   bool _active;
   
   typedef std::map<int64_t, hal_index_t> NameMap;
   typedef std::map<const hal::Genome*, size_t> ChildIdxMap;
   typedef std::map<hal_index_t, hal_index_t> DupCache;

   // name of bottom segment to its index in the genome
   // (completely dependent on assumption that there is only one 
   // parent genome in the tree)
   NameMap _nameMap;

   // convenience for checking which child number a genome is
   ChildIdxMap _childIdxMap;

   // keep track of last current duplicate segment per genome
   DupCache _dupCache;
};


#endif
