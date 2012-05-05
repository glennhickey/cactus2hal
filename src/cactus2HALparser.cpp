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
	open(HALAlignFilePath);
}

Cactus2HALparser::Cactus2HALparser(std::string& HALAlignFilePath,hal::AlignmentPtr alignmentType){
	theAlignment=alignmentType;
	open(HALAlignFilePath);
}

Cactus2HALparser::~Cactus2HALparser(){
	close();
}

hal::GenomePtr convertToHALGenome(CactusDbWrapper* GenomeSeq,std::string* HALSegments){

}

//convertToHALGenome will uses getGenomeSize to find length first
void updateHALAlignment(hal::GenomePtr newGenome){

	}

hal_size_t* getGenomeSize(CactusDbWrapper* GenomeSeq){

}

void Cactus2HALparser::open(std::string& HALAlignFilePath){
	if(!fileExists(HALAlignFilePath)){
		theAlignment->createNew(HALAlignFilePath);
	}
	else{
		theAlignment->open(HALAlignFilePath,false);
	}
}


void Cactus2HALparser::close(){
	theAlignment->close();
}

int main(int argc, char *argv[]){

	string HALSegmentsFilePath,SequenceDB,HALAlignFilePath;
	hal::GenomePtr genometoAdd;

	parseOptions(argc, argv,  HALSegmentsFilePath, SequenceDB,HALAlignFilePath);

	CactusDbWrapper *currSeq= new CactusDbWrapper(SequenceDB);
	Cactus2HALparser myParser=Cactus2HALparser(HALAlignFilePath);
	genometoAdd=myParser.convertToHALGenome(currSeq,&HALAlignFilePath);
	myParser.updateHALAlignment(genometoAdd);
	myParser.close();
	currSeq->close();
	//what do I need to do for all other objects?



}


