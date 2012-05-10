/*
 * HALparser.h
 *
 *  Created on: May 1, 2012
 *      Author: Vlado Uzunangelov
 */

#ifndef CACTUS2HALPARSER_H_
#define CACTUS2HALPARSER_H_

#include <deque>
#include "hal.h"
#include "cactusDbWrapper.h"

class Cactus2HALparser{

public:

	typedef struct {
			string genomeName;
			bool isBottom;
			hal::Sequence::Info seqInfo;
		}SegmentCounters;

		typedef struct{
				hal_size_t segmentID;
				hal_size_t startPos;
				hal_size_t length;
			}topSegmentInfo;

		typedef struct{
			bool isInsertion;
			hal_size_t startPos;
			hal_size_t length;
			hal_size_t segmentID;
			hal_size_t orientation;

		}bottomSegmentInfo;

		typedef struct {
			bool isBottom;
			map<hal_size_t,topSegmentInfo> topSegmentList;
			map<hal_size_t,bottomSegmentInfo>bottomSegmentList;
		}SegmentInfoList;


	Cactus2HALparser();
	Cactus2HALparser(hal::AlignmentPtr HALfile);
	~Cactus2HALparser();

	deque<hal::GenomePtr>* convertToHALGenome(CactusDbWrapper* GenomeSeq,
			std::string* HALSegmentsFilePath);

	void updateHALAlignment(deque<hal::GenomePtr>* newGenomes);
	deque<SegmentCounters>* getSegmentsInfo(string* HALSegmentsFilePath,CactusDbWrapper* GenomeSeq);

	void open(string& HALAlignFilePath);
	void close();

protected:
	hal::AlignmentPtr theAlignment;




};

	void parseOptions(int argc, char **argv, std::string &HALIntermFilePath,
				std::string &DBPath, std::string &HALAlignmentPath);


#endif /* CACTUS2HALPARSER_H_ */
