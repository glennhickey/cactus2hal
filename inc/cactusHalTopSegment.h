/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.cactus
 */

#ifndef _CACTUSHALTOPSEGMENT_H
#define _CACTUSHALTOPSEGMENT_H

#include <string>
#include <iostream>
#include <cctype>

#include "hal.h"
extern "C" {
#include "cactus.h"
}

struct CactusHalTopSegment
{
   CactusHalTopSegment(hal_index_t start = hal::NULL_INDEX,
                       hal_size_t length = 0,
                       Name parent = NULL_NAME,
                       bool reversed = false) :
     _start(start), _length(length), _parent(parent), _reversed(reversed) {}

   hal_index_t _start;
   hal_size_t _length;
   Name _parent;
   bool _reversed;   
};

inline std::istream& operator>>(std::istream& istr, CactusHalTopSegment& topSegment)
{
  istr >> topSegment._start;
  istr >> topSegment._length;
  
  char c = istr.peek();
  while (!istr.bad() && isspace(istr.peek()))
  {
    istr.get(c);
  }  
  c  = istr.peek();
  assert(!isspace(istr.peek()));
  
  if (!istr.bad() && isdigit(c))
  {
    istr >> topSegment._parent;
    int orientation;
    istr >> orientation;
    topSegment._reversed = orientation == 0 ? true : false;
  }
  else
  {
    topSegment._parent = NULL_NAME;
    topSegment._reversed = true;
  }

  return istr;
}


#endif
