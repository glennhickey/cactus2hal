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

const string* CactusHalScanDimensions::getParentName() const
{
	return &_parentGenome;
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
  if (sequence._isBottom==true)
  {
    _parentGenome=sequence._event;
  }
  flushCurrentIntoMap();
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

void CactusHalScanDimensions::scanEndOfFile()
{
  flushCurrentIntoMap();
}

void CactusHalScanDimensions::loadDimensionsIntoHal(hal::AlignmentPtr newAlignment, const string& outgroupName)
{
	  const string* ParentName=getParentName();
	//add the root Genome if alignment opened for the first time
	if(newAlignment->getNumGenomes()==0)
	{

		newAlignment->addRootGenome(*ParentName);
		newAlignment->openGenome(*ParentName)->setDimensions((*_genomeMap[*ParentName]));
		loadSequencesIntoHal(newAlignment,_genomeMap.find(*ParentName));
	}

	GenMapType::const_iterator i;
	for (i = getDimensionsMap()->begin();
			i != getDimensionsMap()->end(); ++i)
	{

		//check we are not dealing with the outgroup genome
		if(i->first.compare(outgroupName)==0)
		{}//do nothing
		else if(newAlignment->openGenome(i->first)!=NULL)
		{
			//entry is in the genome,updating counts
			//ToDo:need to parse in branch lengths!! - set them to zero for now!!

			bool isParent=(i->first.compare(*ParentName)==0);
			vector<hal::Sequence::UpdateInfo>* updatedDims=convertHalDimensions(i->second,&isParent);

			if(isParent){
				//the parent has the bottom sequences
				newAlignment->openGenome(i->first)->setBottomDimensions(*updatedDims);
			}
			else
			{
				//it's a child - top sequences
				newAlignment->openGenome(i->first)->setTopDimensions(*updatedDims);
			}
			loadSequencesIntoHal(newAlignment,i);


		}
		else if(newAlignment->openGenome(i->first)==NULL){
			//entry is not in the genome, adding with parent the parent name
			//ToDo:need to parse in branch lengths!! - set them to zero for now!!

			newAlignment->addLeafGenome(i->first,*ParentName,0.1);
			newAlignment->openGenome(i->first)->setDimensions(*i->second);
			loadSequencesIntoHal(newAlignment,i);
		}

	}//for loop

}

vector<hal::Sequence::UpdateInfo>* CactusHalScanDimensions::convertHalDimensions(vector<hal::Sequence::Info>* dimsToFormat,bool* isParent)
{

		vector<hal::Sequence::UpdateInfo>* formattedDims= new vector<hal::Sequence::UpdateInfo>();
		vector<hal::Sequence::Info>::const_iterator i;
		for (i=dimsToFormat->begin();i!=dimsToFormat->end();++i)
		{
			if(*isParent){
				formattedDims->push_back(hal::Sequence::UpdateInfo(i->_name,i->_numBottomSegments));
			}
			else
			{
				formattedDims->push_back(hal::Sequence::UpdateInfo(i->_name,i->_numTopSegments));
			}
		}

		return formattedDims;
}

void CactusHalScanDimensions::loadSequencesIntoHal(hal::AlignmentPtr theAlignment,GenMapType::const_iterator GenomeInfo){
	vector<hal::Sequence::Info>::const_iterator i;
	for(i=GenomeInfo->second->begin();i!=GenomeInfo->second->end();++i)
	{
		string currSeq(_cactusDb.getSequence(GenomeInfo->first,i->_name));
		theAlignment->openGenome(GenomeInfo->first)->setString(currSeq);
	}
}

void CactusHalScanDimensions::resetCurrent()
{
  _currentGenome.clear();
  _currentInfo._name.clear();
  _currentInfo._length = 0;
  _currentInfo._numTopSegments = 0;
  _currentInfo._numBottomSegments = 0;
}

void CactusHalScanDimensions::flushCurrentIntoMap()
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
}
