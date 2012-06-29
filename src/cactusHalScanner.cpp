/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include "cactusHalScanner.h"

using namespace std;
using namespace hal;


CactusHalScanner::CactusHalScanner()
{

}

CactusHalScanner::~CactusHalScanner()
{

}

void CactusHalScanner::scan(const std::string& halFilePath)
{
  _halFile.open(halFilePath.c_str());

  if (!_halFile)
  {
    throw runtime_error("error opening path: " + halFilePath);
  }
  
  string buffer;
  CactusHalSequence sequenceBuffer;
  CactusHalBottomSegment bsegBuffer;
  CactusHalTopSegment tsegBuffer;

  bool isBottom = false;
  while (!_halFile.eof() && _halFile.good())
  {
    buffer.clear();
    _halFile >> buffer;
    if (buffer == "s")
    {
      _halFile >> sequenceBuffer;
      if (!_halFile.good())
      {
        throw runtime_error("error parsing sequence " + 
                            sequenceBuffer._name);
      }
      isBottom = sequenceBuffer._isBottom;
      scanSequence(sequenceBuffer);
    }
    else if (buffer == "a")
    {
      if (isBottom == true)
      {
        _halFile >> bsegBuffer;
        if (!_halFile.good())
        {
          stringstream ss;
          ss << "error parsing bottom segment with name " 
             << bsegBuffer._name << " and start position " 
             << bsegBuffer._start << " in sequence " 
             << sequenceBuffer._name;
          throw runtime_error(ss.str());
        }
        scanBottomSegment(bsegBuffer);
      }
      else
      {
        _halFile >> tsegBuffer;
        if (!_halFile.good())
        {
          stringstream ss;
          ss << "error parsing top segment with start position " 
             << tsegBuffer._start << " in sequence " 
             << sequenceBuffer._name;
          throw runtime_error(ss.str());
        }
        scanTopSegment(tsegBuffer);
      }
    }
    else if (buffer.empty() == false)
    {
      throw runtime_error("expected a or s token, got " + buffer);
    }
  }

  scanEndOfFile();  
  _halFile.close();
}





