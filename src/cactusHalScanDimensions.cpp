/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "cactusHalScanDimensions.h"

using namespace std;
using namespace hal;


CactusHalScanDimensions::CactusHalScanDimensions()
{

}

CactusHalScanDimensions::~CactusHalScanDimensions()
{
  GenMapType::iterator i;
  for (i = _genomeMap.begin(); i != _genomeMap.end(); ++i)
  {
    delete i->second;
  }
}

const vector<hal::Sequence::Info>* CactusHalScanDimensions::getDimensinos(
  const string& genomeName) const
{
  GenMapType::const_iterator i = _genomeMap.find(genomeName);
  return i == _genomeMap.end() ? NULL : i->second; 
}

void CactusHalScanDimensions::scanDimensions(const string& halFilePath,
                                             const string& cactusDbString)
{
  GenMapType::iterator i;
  for (i = _genomeMap.begin(); i != _genomeMap.end(); ++i)
  {
    delete i->second;
  }
  _genomeMap.clear();

  _cactusDb.open(cactusDbString);
  resetCurrent();
  scan(halFilePath);
}

void CactusHalScanDimensions::scanSequence(CactusHalSequence& sequence)
{
  if (_currentGenome.empty() == false)
  {
    GenMapType::iterator i = _genomeMap.find(_currentGenome);
    if (i != _genomeMap.end())
    {
      assert(i->second != NULL);
      i->second->push_back(_currentInfo);
    }
    else
    {
      vector<hal::Sequence::Info>* infoVec = new  vector<hal::Sequence::Info>();
      infoVec->push_back(_currentInfo);
      _genomeMap.insert(pair<string, vector<hal::Sequence::Info>*>(
                          _currentGenome, infoVec));
    }
  }
  resetCurrent();
  _currentGenome = sequence._event;
  _currentInfo._name = sequence._name;
  _currentInfo._length = _cactusDb.getSequenceLength(_currentGenome,
                                                     _currentInfo._name);
}

void CactusHalScanDimensions::scanTopSegment(CactusHalTopSegment& topSegment)
{
  ++_currentInfo._numTopSegments;
}
  
void CactusHalScanDimensions::scanBottomSegment(
  CactusHalBottomSegment& botSegment)
{
  ++_currentInfo._numBottomSegments;
}

void CactusHalScanDimensions::resetCurrent()
{
  _currentGenome.clear();
  _currentInfo._name.clear();
  _currentInfo._length = 0;
  _currentInfo._numTopSegments = 0;
  _currentInfo._numBottomSegments = 0;
}
