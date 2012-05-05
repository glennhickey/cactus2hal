/*
 * HALparser.h
 *
 *  Created on: May 1, 2012
 *      Author: Vlado Uzunangelov
 */

#ifndef CACTUS2HALPARSER_H_
#define CACTUS2HALPARSER_H_

#include "hal.h"
#include "cactusDbWrapper.h"

class Cactus2HALparser{

public:
	Cactus2HALparser(std::string& HALAlignFilePath);
	Cactus2HALparser(std::string& HALAlignFilePath,hal::AlignmentPtr HALfile);
	~Cactus2HALparser();

	hal::GenomePtr convertToHALGenome(CactusDbWrapper* GenomeSeq,
			std::string* HALSegments);
	//convertToHALGenome will uses getGenomeSize to find length first
	void updateHALAlignment(hal::GenomePtr newGenome);
	hal_size_t* getGenomeSize(CactusDbWrapper* GenomeSeq);//maybe use last entry in the segment list?
	//method for differentiating root from leaf genome?
	void open(std::string& HALAlignFilePath);
	void close();

protected:
	hal::AlignmentPtr  theAlignment;

};

	void parseOptions(int argc, char **argv, std::string &HALIntermFilePath,
				std::string &DBPath, std::string &HALAlignmentPath);


#endif /* CACTUS2HALPARSER_H_ */
