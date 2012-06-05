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
	void loadSegments(hal::AlignmentPtr halAlignment,const std::string& halFilePath,const std::string& outgroupName);
protected:

	void scanSequence(CactusHalSequence& sequence);
	void scanTopSegment(CactusHalTopSegment& topSegment);
	void scanBottomSegment(CactusHalBottomSegment& botSegment);
	void scanEndOfFile();
	void resetCurrent();
	bool isOutgroup(std::string& aGenome);

protected:
	hal::AlignmentPtr _theAlignment;
	std::string _outgroup;
	std::string _parentGenome;
	std::vector<std::string> _parentSequences;

	std::string _currentGenome;
	std::string _currentSequence;
	hal_index_t _segmentsCounter;
};


#endif /* CACTUSHALSCANSEGMENTS_H_ */
