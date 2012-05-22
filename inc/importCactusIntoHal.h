/*
 * importCactusIntoHal.h
 *
 *  Created on: May 21, 2012
 *      Author: Vladislav Uzunangelov
 */

#ifndef IMPORTCACTUSINTOHAL_H_
#define IMPORTCACTUSINTOHAL_H_

#include <string>

void parseOptions(int argc, char **argv, std::string &HALSegmentsPath,
		std::string &SequenceDBPath,std::string &HALAlignmentPath,std::string &OutgroupName);


#endif /* IMPORTCACTUSINTOHAL_H_ */
