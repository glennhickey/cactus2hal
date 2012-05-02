/*
 * HALparser.h
 *
 *  Created on: May 1, 2012
 *      Author: Vlado Uzunangelov
 */

#ifndef HALPARSER_H_
#define HALPARSER_H_

#include "hal.h"

class HALparser{
public:
	hal::Genome* convertToHALGenome();
	hal::Alignment* updateHALAlignment();

protected:
	hal::Alignment* theAlignment;
	hal_size_t* getGenomeSize();
};


#endif /* HALPARSER_H_ */
