/*
 * cactusHalScanSegments.h
 *
 *  Created on: Jun 1, 2012
 *      Author: Vladislav Uzunangelov
 */

#ifndef CACTUSHALSCANSEGMENTS_H_
#define CACTUSHALSCANSEGMENTS_H_

#include <string>
#include <fstream>
#include <map>

#include "cactusHalScanner.h"

class CactusHalScanSegments : protected CactusHalScanner
{
public:

	CactusHalScanSegments();
	~CactusHalScanSegments();

protected:
	void scanSequence(CactusHalSequence& sequence);
	void scanTopSegment(CactusHalTopSegment& topSegment);
	void scanBottomSegment(CactusHalBottomSegment& botSegment);
	void scanEndOfFile();
	void resetCurrent();

protected:
	hal_index_t _SegmentsCounter;
};


#endif /* CACTUSHALSCANSEGMENTS_H_ */
