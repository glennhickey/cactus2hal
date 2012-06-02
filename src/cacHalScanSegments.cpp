/*
 * cacHalScanSegments.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: Vladislav Uzunangelov
 */
#include "cactusHalScanSegments.h"

using namespace hal;

CactusHalScanSegments::CactusHalScanSegments(AlignmentPtr halAlignment)
{
	_theAlignment=halAlignment;
}

CactusHalScanSegments::~CactusHalScanSegments()
{
	_theAlignment->close();
	//release parentSequences??
}


void CactusHalScanSegments::scanSequence(CactusHalSequence& sequence)
{
	if (sequence._isBottom==true)
	  {
	    _parentGenome=sequence._event;
	    _parentSequences.push_back(sequence._name);
	  }

	_currentGenome=sequence._event;
	_currentSequence=sequence._name;
}

void CactusHalScanSegments::scanTopSegment(CactusHalTopSegment& topSegment)
{
	Genome* currGenome=_theAlignment->openGenome(_currentGenome);
	hal::Sequence* currSequence = currGenome->getSequence(_currentSequence);
	TopSegmentIteratorPtr currIterator=currSequence->getTopSegmentIterator(_segmentsCounter);
	TopSegment* currSegment=currIterator->getTopSegment();
	currSegment->setStartPosition(topSegment._start);
	currSegment->setLength(topSegment._length);


	_segmentsCounter++;
}

void CactusHalScanSegments::scanBottomSegment(CactusHalBottomSegment& botSegment)
{
	Genome* currGenome=_theAlignment->openGenome(_currentGenome);
	hal::Sequence* currSequence = currGenome->getSequence(_currentSequence);
	BottomSegmentIteratorPtr currIterator=currSequence->getBottomSegmentIterator(_segmentsCounter);
	BottomSegment* currSegment=currIterator->getBottomSegment();
	currSegment->setStartPosition(botSegment._start);
	currSegment->setLength(botSegment._length);


	_segmentsCounter++;
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
