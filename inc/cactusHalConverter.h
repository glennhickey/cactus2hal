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
#include <set>
#ifdef __GNUG__ 
#include <ext/hash_map>
#endif
#include <limits>

#include "hal.h"
#include "cactusHalScanDimensions.h"

/**
 * Convert cactus .hal into proper hal database
 */
class CactusHalConverter : protected CactusHalScanner
{
public:

   CactusHalConverter();
   ~CactusHalConverter();

   void convert(const std::string& halFilePath,
                const std::string& faFilePath,
                const std::string& treeString,
                hal::AlignmentPtr alignment,
                const std::vector<std::string>& outgroups);

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
   void updateRootParseInfo();
   void scanEndOfFile(){}

protected:
   
   std::string _halFilePath;
   std::string _faFilePath;
   std::string _outDbPath;
   std::string _treeString;
   CactusHalScanDimensions _dimensionScanner;
   std::set<std::string> _outgroups;
   hal::AlignmentPtr _alignment;   
   hal::SequenceIteratorPtr _sequenceIterator;
   hal::TopSegmentIteratorPtr _topIterator;
   hal::BottomSegmentIteratorPtr _bottomIterator;
   hal::BottomSegmentIteratorPtr _parentIterator;
   hal::BottomSegmentIteratorPtr _bottomParseIterator;
   size_t _sequenceIndex;
   hal::Sequence* _halSequence;
   bool _active;
   
   typedef std::pair<hal::Genome*, hal_index_t> GenCoord;
   
   // C++ hash tables have only recently become standard (unordered_map in 
   // c++Ox).  but while we are stuck with older compilers, we use the
   // deprecated __gnu_cxx::hash_map which should at least work for any
   // somewhat recent (>= v4.0 ??) gcc.  other compilers revert to the
   // potentitally less-efficient map for now
#ifdef __GNUG__ 
   struct NameHash { size_t operator()(Name key) const {
     return (size_t) key % std::numeric_limits<size_t>::max();}
   };
   struct GenCoordHash { size_t operator()(const GenCoord& key) const {
     int i1 = ((size_t)key.first) % std::numeric_limits<int>::max();
     int i2 = key.second % std::numeric_limits<int>::max();
     return __gnu_cxx::hash<int>()(i1 + i2);}};
   typedef __gnu_cxx::hash_map<Name, hal_index_t, NameHash> NameMap;
   typedef __gnu_cxx::hash_map<GenCoord, hal_index_t, GenCoordHash> DupCache;
#else
   typedef std::map<Name, hal_index_t> NameMap;
   typedef std::map<GenCoord, hal_index_t> DupCache;
#endif
   typedef std::map<hal::Genome*, size_t> ChildIdxMap;

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
