/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#include <string>
#include <iostream>
#include <cstdlib>
#include <sstream>
extern "C" {
#include "commonC.h"
}

#include "allTests.h"
#include "cactusHalScanDimensions.h"
#include "importCactusIntoHal.h"


using namespace std;
using namespace hal;

string tempFilePath1="/home/bozdugan/output/tmp/test_Anc0.hal";
string DB_Path1="<st_kv_database_conf type=\"tokyo_cabinet\">"
				"<tokyo_cabinet database_dir=\"/home/bozdugan/hal_multiple/blanchette0/Anc0/Anc0/Anc0_DB\" database_name=\"An10\" />"
				"</st_kv_database_conf>";
string tempFilePath2="/home/bozdugan/output/tmp/test_Anc1.hal";
string DB_Path2="<st_kv_database_conf type=\"tokyo_cabinet\">"
				"<tokyo_cabinet database_dir=\"/home/bozdugan/hal_multiple/blanchette0/Anc1/Anc1/Anc1_DB\" database_name=\"An10\" />"
				"</st_kv_database_conf>";
string scanner1outgroup="none";
string scanner2outgroup="Anc7";


class DimsTester:public CactusHalScanDimensions
{
public:

	void setCurrGenome(string& testGenome){
		_currentGenome=testGenome;
	}

	string* getCurrGenome(){
		return &_currentGenome;
	}


	void setCurrInfo(hal::Sequence::Info& testInfo){
		_currentInfo._length=testInfo._length;
		_currentInfo._name=testInfo._name;
		_currentInfo._numBottomSegments=testInfo._numBottomSegments;
		_currentInfo._numTopSegments=testInfo._numTopSegments;
	}

	CactusDbWrapper* getDb(){
		return &_cactusDb;
	}

	void executeFlush(){
		flushCurrentIntoMap();
	}


};


void cactusHalScanDimensionsScanDimensionsTest(CuTest *testCase)
{
	CactusHalScanDimensions scanner1;
	scanner1.scanDimensions(tempFilePath1,DB_Path1);

	const GenMapType* testMap=scanner1.getDimensionsMap();

	CuAssertTrue(testCase,(*testMap).size()==3);
	CuAssertTrue(testCase,(*testMap).at("Anc0")->size()==1);
	CuAssertTrue(testCase,(*testMap).at("Anc0")->operator [](0)._name=="Anc0.0");
	CuAssertTrue(testCase,(*testMap).at("Anc0")->operator [](0)._numBottomSegments==10);
	CuAssertTrue(testCase,(*testMap).at("Anc0")->operator [](0)._numTopSegments==0);


	CuAssertTrue(testCase,(*testMap).at("Anc1")->size()==1);
	CuAssertTrue(testCase,(*testMap).at("Anc1")->operator [](0)._name=="Anc1.0");
	CuAssertTrue(testCase,(*testMap).at("Anc1")->operator [](0)._numBottomSegments==0);
	CuAssertTrue(testCase,(*testMap).at("Anc1")->operator [](0)._numTopSegments==8);


	CuAssertTrue(testCase,(*testMap).at("Anc4")->size()==1);
	CuAssertTrue(testCase,(*testMap).at("Anc4")->operator [](0)._name=="Anc4.0");
	CuAssertTrue(testCase,(*testMap).at("Anc4")->operator [](0)._numBottomSegments==0);
	CuAssertTrue(testCase,(*testMap).at("Anc4")->operator [](0)._numTopSegments==7);

}

void cactusHalScanDimensionsFlushCurrentIntoMapTest(CuTest *testCase)
{
	DimsTester testScanner;
	testScanner.scanDimensions(tempFilePath1,DB_Path1);
	string testName="testGenome";
	testScanner.setCurrGenome(testName);
	hal::Sequence::Info testSeqInfo=hal::Sequence::Info("seq1",100,20,30);
	testScanner.setCurrInfo(testSeqInfo);

	testScanner.executeFlush();
	const GenMapType* testMap=testScanner.getDimensionsMap();
	CuAssertTrue(testCase,(*testMap).size()==4);
	CuAssertTrue(testCase,(*testMap).at("testGenome")->size()==1);
	CuAssertTrue(testCase,(*testMap).at("testGenome")->at(0)._name=="seq1");
	CuAssertTrue(testCase,(*testMap).at("testGenome")->at(0)._length==100);
	CuAssertTrue(testCase,(*testMap).at("testGenome")->at(0)._numBottomSegments==30);
	CuAssertTrue(testCase,(*testMap).at("testGenome")->at(0)._numTopSegments==20);

	testName="Anc4";
	testSeqInfo=hal::Sequence::Info("Anc4.1",100,2,3);
	testScanner.setCurrGenome(testName);

	testScanner.setCurrInfo(testSeqInfo);
	testScanner.executeFlush();

	CuAssertTrue(testCase,(*testMap).size()==4);
	CuAssertTrue(testCase,(*testMap).at("Anc4")->size()==2);


	CuAssertTrue(testCase,(*testMap).at("Anc4")->at(1)._name=="Anc4.1");

	CuAssertTrue(testCase,(*testMap).at("Anc4")->at(1)._length==100);
	CuAssertTrue(testCase,(*testMap).at("Anc4")->at(1)._numBottomSegments==3);
	CuAssertTrue(testCase,(*testMap).at("Anc4")->at(1)._numTopSegments==2);
}
void cactusHalScanDimensionsParentTest(CuTest *testCase)
{
	CactusHalScanDimensions scanner2;
	scanner2.scanDimensions(tempFilePath2,DB_Path2);
	const string* parentName1=scanner2.getParentName();

	CuAssertTrue(testCase,*parentName1=="Anc1");
}



void cactusHalScanDimensionsLoadDimensionsIntoHalTest(CuTest *testCase)
{
	CactusHalScanDimensions scanner1,scanner2;
	scanner1.scanDimensions(tempFilePath1,DB_Path1);
	scanner2.scanDimensions(tempFilePath2,DB_Path2);
	hal::AlignmentPtr theNewAlignment=hdf5AlignmentInstance();
	char* AlignmentTempFile = getTempFile();
	std::string StrTempFile(AlignmentTempFile);
	theNewAlignment->createNew(StrTempFile);
	scanner1.loadDimensionsIntoHal(theNewAlignment,scanner1outgroup);

	//load scanner1, check again what has happened
	CuAssertTrue(testCase,theNewAlignment->getRootName()=="Anc0");
	CuAssertTrue(testCase,theNewAlignment->getNumGenomes()==3);
	CuAssertTrue(testCase,theNewAlignment->getParentName("Anc1")=="Anc0");
	CuAssertTrue(testCase,theNewAlignment->getParentName("Anc4")=="Anc0");

	CuAssertTrue(testCase,theNewAlignment->getChildNames("Anc0").size()==2);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc0")->getNumBottomSegments()==10);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc0")->getNumTopSegments()==0);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc0")->getNumSequences()==1);

	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc1")->getNumBottomSegments()==0);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc1")->getNumTopSegments()==8);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc1")->getNumSequences()==1);

	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc4")->getNumBottomSegments()==0);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc4")->getNumTopSegments()==7);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc4")->getNumSequences()==1);

	//load scanner2, check again what has happened
	scanner2.loadDimensionsIntoHal(theNewAlignment,scanner2outgroup);
	CuAssertTrue(testCase,theNewAlignment->getParentName("Anc2")=="Anc1");
	CuAssertTrue(testCase,theNewAlignment->getParentName("Anc3")=="Anc1");
	CuAssertTrue(testCase,theNewAlignment->getNumGenomes()==5);

	CuAssertTrue(testCase,theNewAlignment->getChildNames("Anc1").size()==2);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc1")->getNumBottomSegments()==6);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc1")->getNumTopSegments()==8);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc1")->getNumSequences()==1);

	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc2")->getNumBottomSegments()==0);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc2")->getNumTopSegments()==11);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc2")->getNumSequences()==1);

	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc3")->getNumBottomSegments()==0);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc3")->getNumTopSegments()==9);
	CuAssertTrue(testCase,theNewAlignment->openGenome("Anc3")->getNumSequences()==1);


	removeTempFile(AlignmentTempFile);
}

void CactusHalScanDimensionsConvertHalDimensionsTest(CuTest *testCase)
{
	CactusHalScanDimensions scanner1;
	vector<hal::Sequence::Info> dimsToFormat;
	bool isParent=TRUE;
	const hal::Sequence::Info info1=hal::Sequence::Info("Rat",1975,200,300);
	const hal::Sequence::Info info2=hal::Sequence::Info("Dawg",1971,30,5);

	dimsToFormat.push_back(info1);
	dimsToFormat.push_back(info2);
	vector<hal::Sequence::UpdateInfo>* formattedDimsTrue=scanner1.convertHalDimensions(&dimsToFormat,&isParent);

	CuAssertTrue(testCase,(*formattedDimsTrue)[0]._name=="Rat");
	CuAssertTrue(testCase,(*formattedDimsTrue)[0]._numSegments==300);
	CuAssertTrue(testCase,(*formattedDimsTrue)[1]._name=="Dawg");
	CuAssertTrue(testCase,(*formattedDimsTrue)[1]._numSegments==5);

	isParent=FALSE;
	vector<hal::Sequence::UpdateInfo>* formattedDimsFalse=scanner1.convertHalDimensions(&dimsToFormat,&isParent);
	CuAssertTrue(testCase,(*formattedDimsFalse)[0]._name=="Rat");
	CuAssertTrue(testCase,(*formattedDimsFalse)[0]._numSegments==200);
	CuAssertTrue(testCase,(*formattedDimsFalse)[1]._name=="Dawg");
	CuAssertTrue(testCase,(*formattedDimsFalse)[1]._numSegments==30);
}

void cactusHalScanDimensionsLoadSequencesIntoHalTest(CuTest *testCase)
{
	DimsTester testScanner1;
	hal::AlignmentPtr theAlignment=hdf5AlignmentInstance();
	char* AlignmentTempFile = getTempFile();
	theAlignment->createNew(AlignmentTempFile);
	testScanner1.scanDimensions(tempFilePath2,DB_Path2);
	testScanner1.loadDimensionsIntoHal(theAlignment,scanner2outgroup);
	testScanner1.loadSequencesIntoHal(theAlignment,scanner2outgroup);

	//first sequence - root

	char* seq=testScanner1.getDb()->getSequence("Anc1","Anc1.0");
	string retrievedSeq;
	theAlignment->openGenome("Anc1")->getSequence("Anc1.0")->getString(retrievedSeq);
	CuAssertTrue(testCase,retrievedSeq.compare(seq)==0);

	free(seq);
	//second sequence -child
	seq=testScanner1.getDb()->getSequence("Anc3","Anc3.0");

	theAlignment->openGenome("Anc3")->getSequence("Anc3.0")->getString(retrievedSeq);
	CuAssertTrue(testCase,retrievedSeq.compare(seq)==0);

	free(seq);
	removeTempFile(AlignmentTempFile);
}



CuSuite* cactusHalScanDimensionsTestSuite(void)
{

  CuSuite* suite = CuSuiteNew();

  SUITE_ADD_TEST(suite, cactusHalScanDimensionsScanDimensionsTest);
  SUITE_ADD_TEST(suite, cactusHalScanDimensionsFlushCurrentIntoMapTest);
  SUITE_ADD_TEST(suite, cactusHalScanDimensionsParentTest);
  SUITE_ADD_TEST(suite, CactusHalScanDimensionsConvertHalDimensionsTest);
  SUITE_ADD_TEST(suite, cactusHalScanDimensionsLoadDimensionsIntoHalTest);
  SUITE_ADD_TEST(suite, cactusHalScanDimensionsLoadSequencesIntoHalTest);
  return suite;
}
