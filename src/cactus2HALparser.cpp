/*
 * HALparser.cpp
 *
 *  Created on: May 2, 2012
 *      Author: Vlado Uzunangelov
 *     parseOptions is based on code from
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
#include "cactus2HALparser.h"

using namespace std;
using namespace hal;


void parseOptions(int argc, char **argv, string &HALSegmentsPath,
		string &SequenceDBPath,string &HALAlignmentPath) {
    int c;
    int setHALSegmentsPath = 0, setDBPath = 0, setHALPath=0;


    while (1) {
        static struct option long_options[] = {
            //{"debug", no_argument, &debug_flag, 1},
            //{"verbose", no_argument, 0, 'v'},
            //{"help", no_argument, 0, 'h'},
            {"segments",  required_argument, 0, 's'},
            {"DB", required_argument,0,'d'},
            {"HAL",  required_argument, 0, 'h'},
            {0, 0, 0}
        };
        int option_index = 0;
        c = getopt_long(argc, argv, "p:d:h:",
                        long_options, &option_index);
        if (c == -1) {
            break;
        }
        switch (c) {
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
        case '?':
            break;
        default:
            abort();
        }
    }
    if (!(setHALSegmentsPath && setDBPath&& setHALPath)) {
        cerr << "specify --segments --DB --HAL\n";
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

Cactus2HALparser::Cactus2HALparser(std::string& HALAlignFilePath){
	theAlignment=hdf5AlignmentInstance();
	if(!fileExists(HALAlignFilePath)){
			theAlignment->createNew(HALAlignFilePath);
		}
		else{
			theAlignment->open(HALAlignFilePath,false);
		}
}

Cactus2HALparser::Cactus2HALparser(std::string& HALAlignFilePath,hal::AlignmentPtr alignmentType){
	theAlignment=alignmentType;
	if(!fileExists(HALAlignFilePath)){
				theAlignment->createNew(HALAlignFilePath);
			}
			else{
				theAlignment->open(HALAlignFilePath,false);
			}
}

Cactus2HALparser::~Cactus2HALparser(){
	theAlignment->close();
	theAlignment=NULL;
}

int main(int argc, char *argv[]){

	//create a cactus db wrapper object and a cactustoHalParser object
	// do parse options on them
	//use them to add genomes
	string PreHALFilePath,PreHalSeqDB,HALAlignFilePath;

	parseOptions(argc, argv,  PreHALFilePath, PreHalSeqDB,HALAlignFilePath);



}


