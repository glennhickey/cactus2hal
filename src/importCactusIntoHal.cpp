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
                {"outgroup",2, 0, 'o'},
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

    if (setOutgroup==0) {
    	OutgroupName="none";

    }
    if (!(setHALSegmentsPath && setDBPath&& setHALPath)) {
        cerr << "specify --segments --sequence --HAL\n";
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

vector<hal::Sequence::UpdateInfo>* ConvertHalDimensions(vector<hal::Sequence::Info>* DimsToFormat,bool* isParent)
{

		vector<hal::Sequence::UpdateInfo>* FormattedDims= new vector<hal::Sequence::UpdateInfo>();
		vector<hal::Sequence::Info>::const_iterator i;
		for (i=DimsToFormat->begin();i!=DimsToFormat->end();++i)
		{
			if(*isParent){
				FormattedDims->push_back(hal::Sequence::UpdateInfo(i->_name,i->_numBottomSegments));
			}
			else
			{
				FormattedDims->push_back(hal::Sequence::UpdateInfo(i->_name,i->_numTopSegments));
			}
		}

		return FormattedDims;
}

void loadDimensionsIntoHal(const CactusHalScanDimensions& DimsScanner,
		hal::AlignmentPtr newAlignment, const string& outgroupName)
{
	  const string* ParentName=DimsScanner.getParentName();
	//add the root Genome if alignment opened for the first time
	if(newAlignment->getRootName().empty()){
		newAlignment->addRootGenome(*ParentName);

	}
	GenMapType::const_iterator i;
	for (i = DimsScanner.getDimensionsMap()->begin();
			i != DimsScanner.getDimensionsMap()->end(); ++i)
	{
		//check we are not dealing with the outgroup genome
		if(i->first.compare(outgroupName)!=0) continue;
		else if(newAlignment->openGenome(i->first)!=NULL)
		{
			//entry is in the genome,updating counts
			//ToDo:need to parse in branch lengths!! - set them to zero for now!!

			bool isParent=(i->first.compare(*ParentName)==0);
			vector<hal::Sequence::UpdateInfo>* updatedDims=ConvertHalDimensions(i->second,&isParent);

			if(isParent){
				//the parent has the bottom sequences
				newAlignment->openGenome(i->first)->setBottomDimensions(*updatedDims);
			}
			else
			{
				//it's a child - top sequences
				newAlignment->openGenome(i->first)->setTopDimensions(*updatedDims);
			}



		}
		else if(newAlignment->openGenome(i->first)==NULL){
			//entry is not in the genome, adding with parent the parent name
			//ToDo:need to parse in branch lengths!! - set them to zero for now!!
			newAlignment->addLeafGenome(i->first,*ParentName,0);
			newAlignment->openGenome(i->first)->setDimensions(*i->second);
		}

	}//for loop

}


int main(int argc, char** argv)
{
	string HALSegmentsFilePath,SequenceDB,HALAlignFilePath,outgroup;
	hal::AlignmentPtr theNewAlignment=hdf5AlignmentInstance();
	parseOptions(argc, argv,  HALSegmentsFilePath, SequenceDB,HALAlignFilePath,outgroup);

	if(!fileExists(HALAlignFilePath)){
			theNewAlignment->createNew(HALAlignFilePath);
		}
		else{
			theNewAlignment->open(HALAlignFilePath,false);
		}

	CactusHalScanDimensions DimensionsScanner;
	DimensionsScanner.scanDimensions(HALSegmentsFilePath,SequenceDB);
	loadDimensionsIntoHal(DimensionsScanner,theNewAlignment,outgroup);
	theNewAlignment->close();
}
