/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.cactus
 */

#ifndef _HALCACTUSSCANNER_H
#define _HALCACTUSSCANNER_H

#include <string>
#include <fstream>

extern "C" {
#include "commonC.h"
#include "sonLib.h"
#include "cactus.h"
}

#include "cactusHalTopSegment.h"
#include "cactusHalBottomSegment.h"
#include "cactusHalSequence.h"

/**
 * abstract base class for scanning through a .hal file as output by cactus.  
 */
class CactusHalScanner
{
public:
   
   CactusHalScanner();
   virtual ~CactusHalScanner();

   void scan(const std::string& halFilePath);

protected:

   virtual void scanSequence(CactusHalSequence& sequence) = 0;
   virtual void scanTopSegment(CactusHalTopSegment& topSegment) = 0;
   virtual void scanBottomSegment(CactusHalBottomSegment& botSegment) = 0;

protected:

   std::ifstream _halFile;
};

#endif
