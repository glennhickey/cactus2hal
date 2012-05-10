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
 * base class for scanning through a .hal file as output by cactus.  
 */
class CactusHalScanner
{
public:
   
   CactusHalScanner();
   ~CactusHalScanner();

   void scan(const std::string& halFilePath);

protected:

   virtual void scanSequence(CactusHalSequence& sequence);
   virtual void scanTopSegment(CactusHalTopSegment& topSegment);
   virtual void scanBottomSegment(CactusHalBottomSegment& botSegment);

protected:

   std::ifstream _halFile;
};

#endif
