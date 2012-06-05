/*
 * cacHalScanSegments.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: Vladislav Uzunangelov
 */
#include "cactusHalScanSegments.h"

using namespace std;
using namespace hal;

CactusHalScanSegments::CactusHalScanSegments()
{

}

CactusHalScanSegments::~CactusHalScanSegments()
{
	//release parentSequences??
}

void CactusHalScanSegments::loadSegments(hal::AlignmentPtr halAlignment,const string& halFilePath,const std::string& outgroupName)
{
	resetCurrent();
	_theAlignment=halAlignment;
	_outgroup=outgroupName;
	scan(halFilePath);
}


void CactusHalScanSegments::scanSequence(CactusHalSequence& sequence)
{

		if (sequence._isBottom==true)
		{
			_parentGenome=sequence._event;
			_parentSequences.push_back(sequence._name);
		}
		resetCurrent();
		_currentGenome=sequence._event;
		_currentSequence=sequence._name;
}

void CactusHalScanSegments::scanTopSegment(CactusHalTopSegment& topSegment)
{
	if (!isOutgroup(_currentGenome))
	{
		Genome* currGenome=_theAlignment->openGenome(_currentGenome);
		hal::Sequence* currSequence = currGenome->getSequence(_currentSequence);
		TopSegmentIteratorPtr currIterator=currSequence->getTopSegmentIterator(_segmentsCounter);
		TopSegment* currSegment=currIterator->getTopSegment();
		currSegment->setStartPosition(topSegment._start);
		currSegment->setLength(topSegment._length);

		_segmentsCounter++;
	}
}

void CactusHalScanSegments::scanBottomSegment(CactusHalBottomSegment& botSegment)
{
	if (!isOutgroup(_currentGenome))
	{
		Genome* currGenome=_theAlignment->openGenome(_currentGenome);
		hal::Sequence* currSequence = currGenome->getSequence(_currentSequence);
		BottomSegmentIteratorPtr currIterator=currSequence->getBottomSegmentIterator(_segmentsCounter);
		BottomSegment* currSegment=currIterator->getBottomSegment();
		currSegment->setStartPosition(botSegment._start);
		currSegment->setLength(botSegment._length);

		_segmentsCounter++;
	}
}

void CactusHalScanSegments::scanEndOfFile()
{
	resetCurrent();
}
void CactusHalScanSegments::resetCurrent()
{
	_segmentsCounter=0;
	_currentGenome.clear();
	_currentSequence.clear();

}

bool CactusHalScanSegments::isOutgroup(string& aGenome)
{
	return _outgroup==aGenome;
}

