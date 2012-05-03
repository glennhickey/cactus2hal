/*
 * HALparser.cpp
 *
 *  Created on: May 2, 2012
 *      Author: Vlado Uzunangelov
 *     parseOptions code has been copied from
 *     Dent Earl's MAFTools library.
 */

#include <iostream>
#include <sstream>
#include <string>
#include <getopt.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "utils.h"
#include "halAlignmentInstance.h"

using namespace std;

//AlignmentPtr hal::Alignment* theAlignment;


void parseOptions(int argc, char **argv, string &MAFFilePath,
		string &DBPath,string &HALAlignmentPath) {
    int c;
    int setMAFpath = 0, setDBPath = 0, setHALPath=0;


    while (1) {
        static struct option long_options[] = {
            //{"debug", no_argument, &debug_flag, 1},
            //{"verbose", no_argument, 0, 'v'},
            //{"help", no_argument, 0, 'h'},
            {"MAF",  required_argument, 0, 'm'},
            {"DB", required_argument,0,'d'},
            {"HAL",  required_argument, 0, 'h'},
            {0, 0, 0}
        };
        int option_index = 0;
        c = getopt_long(argc, argv, "m:d:h:",
                        long_options, &option_index);
        if (c == -1) {
            break;
        }
        switch (c) {
        case 0:
            break;
        case 'm':
            setMAFpath = 1;
            MAFFilePath=optarg;

            break;
        case 'd':
        	setDBPath = 1;
        	DBPath=optarg;

        	break;
        case 'h':
        	setHALPath = 1;
        	HALAlignmentPath=optarg;

            break;
        case '?':
            break;
        default:
            abort();
        }
    }
    if (!(setMAFpath && setDBPath&& setHALPath)) {
        cerr << "specify --MAF --DB --HAL\n";
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


int main(int argc, char *argv[]){
	string MAFfilepath,MAFdb,HALAlignFilePath;

	parseOptions(argc, argv,  MAFfilepath, MAFdb,HALAlignFilePath);
	if(!fileExists(HALAlignFilePath)){
		//theAlignment
	}

}


