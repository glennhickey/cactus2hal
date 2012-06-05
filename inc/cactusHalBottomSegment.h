/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.cactus
 */

#ifndef _CACTUSHALBOTTOMSEGMENT_H
#define _CACTUSHALBOTTOMSEGMENT_H

#include <string>
#include <istream>

#include "hal.h"
extern "C" {
#include "cactus.h"
}

/** structure containing the information from a bottom segment line of 
 * the cactus .hal file */
struct CactusHalBottomSegment
{
   CactusHalBottomSegment(Name name = NULL_NAME,
                          hal_index_t start = hal::NULL_INDEX,
                          hal_size_t length = 0) :
     _name(name),
     _start(start), 
     _length(length) {}
   
   Name _name;
   hal_index_t _start;
   hal_size_t _length;
};

inline
std::istream& operator>>(std::istream& istr, 
                         CactusHalBottomSegment& bottomSegment)
{
  istr >> bottomSegment._name;
  istr >> bottomSegment._start;
  istr >> bottomSegment._length;
  assert(bottomSegment._name != NULL_NAME);
  return istr;
}

#endif
