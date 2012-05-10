/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.cactus
 */

#ifndef _CACTUSHALSEQUENCE_H
#define _CACTUSHALSEQUENCE_H

#include <string>
#include <istream>

#include "hal.h"

struct CactusHalSequence
{
   CactusHalSequence(const std::string event = "",
                     const std::string name = "",
                     bool isBottom = false) :
     _event(event), _name(name), _isBottom(isBottom) {}

   std::string _event;
   std::string _name;
   bool _isBottom;
   
};

inline
std::istream& operator>>(std::istream& istr, CactusHalSequence& sequence)
{
  istr >> sequence._event;
  istr >> sequence._name;
  istr >> sequence._isBottom;

  size_t pos = 0;
  size_t len = 0;
  if (sequence._event[0] == '\'')
  {
    ++pos;
    ++len;
  }
  if (sequence._event.length() > 1 && 
      sequence._event[sequence._event.length() - 1] == '\'')
  {
    ++len;
  }
  if (len > 0)
  {
    sequence._event = sequence._event.substr(pos, 
                                             sequence._event.length() - len);
  }

  pos = 0;
  len = 0;
  if (sequence._name[0] == '\'')
  {
    ++pos;
    ++len;
  }
  if (sequence._name.length() > 1 && 
      sequence._name[sequence._name.length() - 1] == '\'')
  {
    ++len;
  }
  if (len > 0)
  {
    sequence._name = sequence._name.substr(pos, 
                                           sequence._name.length() - len);
  }
  
  return istr;
}

#endif
