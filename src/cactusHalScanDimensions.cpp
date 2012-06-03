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

const GenMapType* CactusHalScanDimensions::getDimensionsMap() const
{
  return &_genomeMap;
}

void CactusHalScanDimensions::scanDimensions(const string& halFilePath)
{
  GenMapType::iterator i;
  for (i = _genomeMap.begin(); i != _genomeMap.end(); ++i)
  {
    delete i->second;
  }
  _genomeMap.clear();

  resetCurrent();
  scan(halFilePath);
}

void CactusHalScanDimensions::scanSequence(CactusHalSequence& sequence)
{
  if (sequence._isBottom==true)
  {
    _parentGenome=sequence._event;
  }
  flushCurrentIntoMap();
  _currentGenome = sequence._event;
  _currentInfo._name = sequence._name;
}

void CactusHalScanDimensions::scanTopSegment(CactusHalTopSegment& topSegment)
{
  ++_currentInfo._numTopSegments;
  _currentSeqLength += topSegment._length;
}
  
void CactusHalScanDimensions::scanBottomSegment(
  CactusHalBottomSegment& botSegment)
{
  ++_currentInfo._numBottomSegments;
  _currentSeqLength += botSegment._length;
}

void CactusHalScanDimensions::scanEndOfFile()
{
  flushCurrentIntoMap();
}

void CactusHalScanDimensions::resetCurrent()
{
  _currentGenome.clear();
  _currentInfo._name.clear();
  _currentInfo._length = 0;
  _currentInfo._numTopSegments = 0;
  _currentInfo._numBottomSegments = 0;
  _currentSeqLength = 0;
}

void CactusHalScanDimensions::flushCurrentIntoMap()
{
  _currentInfo._length = _currentSeqLength;
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
}
