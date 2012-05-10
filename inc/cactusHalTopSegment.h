/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.cactus
 */

#ifndef _CACTUSHALTOPSEGMENT_H
#define _CACTUSHALTOPSEGMENT_H

#include <string>
#include <iostream>

#include "hal.h"

struct CactusHalTopSegment
{
   CactusHalTopSegment(hal_index_t start = hal::NULL_INDEX,
                       hal_size_t length = 0,
                       hal_index_t parent = hal::NULL_INDEX,
                       bool reversed = false) :
     _start(start), _length(length), _parent(parent), _reversed(reversed) {}

   hal_index_t _start;
   hal_size_t _length;
   hal_index_t _parent;
   bool _reversed;   
};

inline
std::istream& operator>>(std::istream& istr, CactusHalTopSegment& topSegment)
{
  istr >> topSegment._start;
  istr >> topSegment._length;
  
  topSegment._parent = hal::NULL_INDEX;

  istr >> std::ws;
  std::streampos pos = istr.tellg();
  if (!istr.eof())
  {
    char buffer[64];
    istr >> buffer;
    istr.seekg(pos);

    if (buffer[0] != 'a' && buffer[0] != 's')
    {
      istr >> topSegment._parent;
      int orientation;
      istr >> orientation;
      topSegment._reversed = orientation == 0 ? true : false;
    }
  }
  return istr;
}

#endif
