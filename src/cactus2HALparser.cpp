/*
 * HALparser.cpp
 *
 *  Created on: May 2, 2012
 *      Author: Vlado Uzunangelov
 *     parseOptions is based on code from
 *     Dent Earl's MAFTools library.
 */

#include <iostream>
#include <fstream>
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

#ifdef __COMMENT_OUT_DOES_NOT_COMPILE___
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

Cactus2HALparser::Cactus2HALparser(){
	theAlignment=hdf5AlignmentInstance();
}

Cactus2HALparser::Cactus2HALparser(hal::AlignmentPtr alignmentType){
	theAlignment=alignmentType;
}

Cactus2HALparser::~Cactus2HALparser(){
	close();
}

deque<hal::GenomePtr>* convertToHALGenome(CactusDbWrapper* GenomeSeq,std::string* HALSegmentsFilePath){
		//use get genomeInfo to get segments info
		//iterate over collection, extract only ones from same genome,make those into  a hal genome, repeat
	// those two should be another method - initialize Genome


		//still need to iterate over file again and add actual sequences;-after genome initialization

}


void updateHALAlignment(deque<hal::GenomePtr>* newGenomes){

	}

deque<Cactus2HALparser::SegmentCounters>* getSegmentsInfo(std::string* HALSegmentsFilePath,CactusDbWrapper* GenomeSeq){

	ifstream InFile(*HALSegmentsFilePath,ifstream::in);
	istringstream tokenizer;
	std::string aLine,start;
	char headerLine='c',sequenceLine='a';
	deque<Cactus2HALparser::SegmentCounters>* GenomeInfo=new deque<Cactus2HALparser::SegmentCounters>();

	getline(InFile,aLine,'\n');
	while(!InFile.eof()){

		if(aLine[0]==headerLine){
			tokenizer.str(aLine);
			Cactus2HALparser::SegmentCounters currSegmentCounter= Cactus2HALparser::SegmentCounters();
			tokenizer>>start>>currSegmentCounter.genomeName>>currSegmentCounter.seqInfo._name>>currSegmentCounter.isBottom;

			//get number of lines until next s starting line

		}
		else{
			//throw some ghastly exception
		}
	}
	return GenomeInfo;
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
	deque<hal::GenomePtr>* genomestoAdd;
	parseOptions(argc, argv,  HALSegmentsFilePath, SequenceDB,HALAlignFilePath);

	CactusDbWrapper *currSeqs= new CactusDbWrapper(SequenceDB);
	Cactus2HALparser myParser=Cactus2HALparser();
	myParser.open(HALAlignFilePath);

	//ToDO:should I care about parent-child information  - the HALGenome header seems to do so??
	genomestoAdd=myParser.convertToHALGenome(currSeqs,&HALAlignFilePath);
	myParser.updateHALAlignment(genomestoAdd);

	currSeqs->close();
	//do I need to release the genomesToAdd vector??

}


#endif
