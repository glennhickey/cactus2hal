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
	hal::Genome* convertToHALGenome(hal_size_t* genomeSize,CactusDbWrapper* GenomeSeq,
			std::string* PreHALSegments);
	void updateHALAlignment(hal::Genome* newGenome);
	hal_size_t* getGenomeSize(CactusDbWrapper* GenomeSeq);
	//method for differentiating root from leaf genome?


protected:
	hal::AlignmentPtr  theAlignment;

};

	void parseOptions(int argc, char **argv, std::string &HALIntermFilePath,
				std::string &DBPath, std::string &HALAlignmentPath);


#endif /* CACTUS2HALPARSER_H_ */
