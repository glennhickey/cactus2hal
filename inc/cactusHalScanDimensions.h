/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.cactus
 */

#ifndef _HALCACTUSSCANDIMENSIONS_H
#define _HALCACTUSSCANDIMENSIONS_H

#include <string>
#include <fstream>
#include <map>

#include "cactusHalScanner.h"
#include "cactusDbWrapper.h"

typedef  std::map<std::string, std::vector<hal::Sequence::Info>* > GenMapType;
/**
 * Scan the .hal file to determine the demensions of each genome (event)
 */
class CactusHalScanDimensions : protected CactusHalScanner
{
public:


   CactusHalScanDimensions();
   ~CactusHalScanDimensions();

   const GenMapType* getDimensionsMap() const;

   void scanDimensions(const std::string& halFilePath,
                       const std::string& cactusDbString);
   const std::string* getParentName() const;

protected:

   void scanSequence(CactusHalSequence& sequence);
   void scanTopSegment(CactusHalTopSegment& topSegment);
   void scanBottomSegment(CactusHalBottomSegment& botSegment);

   void resetCurrent();
   
protected:

   GenMapType _genomeMap;
   CactusDbWrapper _cactusDb;
   std::string _ParentGenome;

   std::string _currentGenome;
   hal::Sequence::Info _currentInfo;
};

#endif
