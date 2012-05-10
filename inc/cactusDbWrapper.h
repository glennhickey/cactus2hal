/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#ifndef _CACTUSDBWRAPPER_H
#define _CACTUSDBWRAPPER_H

#include <string>
extern "C" {
#include "commonC.h"
#include "sonLib.h"
#include "cactus.h"
}

/**
 * helper class to read sequence information from a Cactus Database
 */
class CactusDbWrapper
{
public:
   CactusDbWrapper();
   CactusDbWrapper(const std::string& dbString);
   ~CactusDbWrapper();
   void open(const std::string& dbString);
   void close();
   // RETURNED STRING MUST BE FREED!
   char* getSequence(const std::string& eventName,
                     const std::string& sequenceName);

   size_t getSequenceLength(const std::string& eventName,
                            const std::string& sequenceName);

   // debugging function
   void printSequenceNames();

protected:
   EventTree* _etree;
   Flower* _flower;
   CactusDisk* _disk;
};

#endif
