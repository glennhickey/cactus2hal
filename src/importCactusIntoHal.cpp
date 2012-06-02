/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
extern "C" {
#include "commonC.h"
#include "sonLib.h"
#include "cactus.h"
}


#include "cactusDbWrapper.h"
#include "utils.h"
#include "cactus2HALparser.h"
#include "cactusHalScanDimensions.h"

using namespace std;
using namespace hal;

void parseOptions(int argc, char **argv, string &HALSegmentsPath,
		string &SequenceDBPath,string &HALAlignmentPath,string &OutgroupName) {
    int opt;
    int setHALSegmentsPath = 0, setDBPath = 0, setHALPath=0,setOutgroup=0;
    static struct option long_options[] = {
                {"segments",1, 0, 's'},
                {"sequence",1,0,'d'},
                {"HAL",1, 0, 'h'},
                {"outgroup",1, 0, 'o'},
                {0,0,0,0}	};

    while (1) {
        int option_index = 0;
        opt = getopt_long(argc, argv, "s:d:h:o:",
                        long_options, &option_index);
        if (opt == -1) { break; }
        switch (opt) {
        case 0:
            break;
        case 's':
            setHALSegmentsPath = 1;
            HALSegmentsPath=optarg;

            break;
        case 'd':
        	setDBPath = 1;
        	SequenceDBPath=optarg;

        	break;
        case 'h':
        	setHALPath = 1;
        	HALAlignmentPath=optarg;

            break;
        case 'o':
        	setOutgroup=1;
        	OutgroupName=optarg;

        	break;
        case '?':
            break;
        }
    }


    if (!(setHALSegmentsPath && setDBPath&& setHALPath && setOutgroup)) {
        cerr << "specify --segments --sequence --HAL --outgroup\n";
    }
    // Check there's nothing left over on the command line
    if (optind < argc) {
        string errorString= "Unexpected arguments:";
        while (optind < argc) {
            errorString.append(" ");
            errorString.append(argv[optind++]);
        }
        cerr<<errorString;
    }
}


int main(int argc, char** argv)
{

	string halSegmentsFilePath,sequenceDB,hallignFilePath,outgroup;
	hal::AlignmentPtr theNewAlignment=hdf5AlignmentInstance();
	parseOptions(argc, argv,  halSegmentsFilePath, sequenceDB,hallignFilePath,outgroup);

	if(!fileExists(hallignFilePath)){
			theNewAlignment->createNew(hallignFilePath);

		}
		else{

			theNewAlignment->open(hallignFilePath,false);
		}

	CactusHalScanDimensions DimensionsScanner;
	DimensionsScanner.scanDimensions(halSegmentsFilePath,sequenceDB);
	DimensionsScanner.loadDimensionsIntoHal(theNewAlignment,outgroup);
	DimensionsScanner.loadSequencesIntoHal(theNewAlignment);
	theNewAlignment->close();

}
