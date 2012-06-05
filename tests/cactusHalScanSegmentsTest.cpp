/*
 * cactusHalScanSegmentsTest.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: Vladislav Uzunangelov
 */

extern "C" {
#include "commonC.h"
}

#include "allTests.h"
#include "cactusHalScanSegments.h"
#include "cactusHalScanDimensions.h"

using namespace std;

string tempFilePath="/home/bozdugan/output/tmp/test_Anc1.hal";
string DB_Path="<st_kv_database_conf type=\"tokyo_cabinet\">"
				"<tokyo_cabinet database_dir=\"/home/bozdugan/hal_multiple/blanchette0/Anc1/Anc1/Anc1_DB\" database_name=\"An10\" />"
				"</st_kv_database_conf>";
string scannerOutgroup="Anc7";


class SegmentsTester: public CactusHalScanSegments
{

public:

	string* getParent()
	{
		return &_parentGenome;
	}

	vector<string>* getParentSequences()
	{
		return &_parentSequences;
	}

	string* getCurrentGenome()
	{
		return &_currentGenome;
	}

	string* getCurrentSeq()
	{
		return &_currentSequence;
	}

	hal::AlignmentPtr getAlignment()
	{
		return _theAlignment;
	}

	hal_index_t* getCurrentIndex()
	{
		return &_segmentsCounter;
	}

	void setCurrentGenome(string& genomeName)
	{
		_currentGenome=genomeName;
	}

	void setCurrentSequence(string& seqName)
	{
		_currentSequence=seqName;
	}
	void setAlignment(hal::AlignmentPtr halAlignment)
	{
		_theAlignment=halAlignment;
	}

	void executeScanSequence(CactusHalSequence& sequence)
	{
		scanSequence(sequence);
	}

	void executeScanTopSegment(CactusHalTopSegment& topSegment)
	{
		scanTopSegment(topSegment);
	}

	void executeScanBottomSegment(CactusHalBottomSegment& botSegment)
	{
		scanBottomSegment(botSegment);
	}

	void executeResetCurrent()
	{
		resetCurrent();
	}

};




void cactusHalScanSegmentsScanSequenceTest(CuTest *testCase)
{
	SegmentsTester testScanner;
	CactusHalSequence testSeq1("Rat","Rat1.0",true);
	testScanner.executeScanSequence(testSeq1);
	CuAssertTrue(testCase,*testScanner.getParent()=="Rat");
	CuAssertTrue(testCase,testScanner.getParentSequences()->at(0)=="Rat1.0");

	CactusHalSequence testSeq2("Rat","Rat2.0",true);
	testScanner.executeScanSequence(testSeq2);
	CuAssertTrue(testCase,testScanner.getParentSequences()->at(0)=="Rat1.0");
	CuAssertTrue(testCase,testScanner.getParentSequences()->at(1)=="Rat2.0");

	CactusHalSequence testSeq3("Dawg","Dawg1.0",false);
	testScanner.executeScanSequence(testSeq3);
	CuAssertTrue(testCase,*testScanner.getCurrentGenome()=="Dawg");
	CuAssertTrue(testCase,*testScanner.getCurrentSeq()=="Dawg1.0");
	CuAssertTrue(testCase,*testScanner.getParent()=="Rat");
	CuAssertTrue(testCase,testScanner.getParentSequences()->size()==2);
}

void cactusHalScanSegmentsScanTopSegmentTest(CuTest *testCase)
{
	SegmentsTester testScanner;
	char* AlignmentTempFile = getTempFile();
	testScanner.setAlignment(hal::hdf5AlignmentInstance());
	testScanner.getAlignment()->createNew(AlignmentTempFile);
	CactusHalScanDimensions dimTester;
	dimTester.scanDimensions(tempFilePath,DB_Path);
	dimTester.loadDimensionsIntoHal(testScanner.getAlignment(),scannerOutgroup);

	CuAssertTrue(testCase,*testScanner.getCurrentIndex()==0);

	string testGenome="Anc2";
	testScanner.setCurrentGenome(testGenome);
	string testSequence="Anc2.0";
	testScanner.setCurrentSequence(testSequence);
	CactusHalTopSegment testSegment1(3,55,12,true);
	testScanner.executeScanTopSegment(testSegment1);

	hal::TopSegmentIteratorPtr currIterator=testScanner.getAlignment()->
			openGenome(testGenome)->getSequence(testSequence)->
			getTopSegmentIterator(*testScanner.getCurrentIndex()-1);
	hal::TopSegment* currSegment=currIterator->getTopSegment();
	CuAssertTrue(testCase,*testScanner.getCurrentIndex()==1);
	CuAssertTrue(testCase,currSegment->getArrayIndex()==0);
	CuAssertTrue(testCase,currSegment->getLength()==55);
	CuAssertTrue(testCase,currSegment->getStartPosition()==3);

	CactusHalTopSegment testSegment2(17,12);
	testScanner.executeScanTopSegment(testSegment2);
	currIterator=testScanner.getAlignment()->
				openGenome(testGenome)->getSequence(testSequence)->
				getTopSegmentIterator(*testScanner.getCurrentIndex()-1);
	currSegment=currIterator->getTopSegment();
	CuAssertTrue(testCase,*testScanner.getCurrentIndex()==2);
	CuAssertTrue(testCase,currSegment->getArrayIndex()==1);
	CuAssertTrue(testCase,currSegment->getLength()==12);
	CuAssertTrue(testCase,currSegment->getStartPosition()==17);

	testScanner.executeResetCurrent();
	CuAssertTrue(testCase,*testScanner.getCurrentIndex()==0);
	removeTempFile(AlignmentTempFile);
}

void cactusHalScanSegmentsScanBottomSegmentTest(CuTest *testCase)
{
	SegmentsTester testScanner;
	char* AlignmentTempFile = getTempFile();
	testScanner.setAlignment(hal::hdf5AlignmentInstance());
	testScanner.getAlignment()->createNew(AlignmentTempFile);
	CactusHalScanDimensions dimTester;
	dimTester.scanDimensions(tempFilePath,DB_Path);
	dimTester.loadDimensionsIntoHal(testScanner.getAlignment(),scannerOutgroup);

	CuAssertTrue(testCase,*testScanner.getCurrentIndex()==0);

	string testGenome="Anc1";
	testScanner.setCurrentGenome(testGenome);
	string testSequence="Anc1.0";
	testScanner.setCurrentSequence(testSequence);
	CactusHalBottomSegment testSegment(345,55,12);
	//craps out here
	testScanner.executeScanBottomSegment(testSegment);

	hal::BottomSegmentIteratorPtr currIterator=testScanner.getAlignment()->
			openGenome(testGenome)->getSequence(testSequence)->
			getBottomSegmentIterator(*testScanner.getCurrentIndex()-1);
	hal::BottomSegment* currSegment=currIterator->getBottomSegment();
	CuAssertTrue(testCase,*testScanner.getCurrentIndex()==1);
	CuAssertTrue(testCase,currSegment->getArrayIndex()==0);
	CuAssertTrue(testCase,currSegment->getLength()==12);
	CuAssertTrue(testCase,currSegment->getStartPosition()==55);

	testScanner.executeResetCurrent();
	CuAssertTrue(testCase,*testScanner.getCurrentIndex()==0);
	removeTempFile(AlignmentTempFile);
}

void cactusHalScanSegmentsLoadSegmentsTest(CuTest *testCase)
{
	SegmentsTester testScanner;
	testScanner.setAlignment(hal::hdf5AlignmentInstance());
	char* AlignmentTempFile = getTempFile();
	testScanner.getAlignment()->createNew(AlignmentTempFile);
	CactusHalScanDimensions dimTester;
	dimTester.scanDimensions(tempFilePath,DB_Path);
	dimTester.loadDimensionsIntoHal(testScanner.getAlignment(),scannerOutgroup);
	testScanner.loadSegments(testScanner.getAlignment(),tempFilePath,scannerOutgroup);


	//1st sequence
	hal::Sequence* currSequence=testScanner.getAlignment()->
			openGenome("Anc3")->getSequence("Anc3.0");

	CuAssertTrue(testCase,currSequence->getNumTopSegments()==9);

	hal::TopSegmentIteratorPtr topIterator=currSequence->
				getTopSegmentIterator(0);
	hal::TopSegment* topSegment=topIterator->getTopSegment();

	CuAssertTrue(testCase,topSegment->getArrayIndex()==0);
	CuAssertTrue(testCase,topSegment->getLength()==7);
	CuAssertTrue(testCase,topSegment->getStartPosition()==0);

	topIterator=currSequence->getTopSegmentIterator(1);
	topSegment=topIterator->getTopSegment();

	CuAssertTrue(testCase,topSegment->getArrayIndex()==1);
	CuAssertTrue(testCase,topSegment->getLength()==2);
	CuAssertTrue(testCase,topSegment->getStartPosition()==7);

	topIterator=currSequence->getTopSegmentIterator(8);
	topSegment=topIterator->getTopSegment();

	CuAssertTrue(testCase,topSegment->getArrayIndex()==8);
	CuAssertTrue(testCase,topSegment->getLength()==81);
	CuAssertTrue(testCase,topSegment->getStartPosition()==183);

	//2nd sequence
	currSequence=testScanner.getAlignment()->openGenome("Anc2")->getSequence("Anc2.0");

	topIterator=currSequence->getTopSegmentIterator(4);
	topSegment=topIterator->getTopSegment();

	CuAssertTrue(testCase,topSegment->getArrayIndex()==4);
	CuAssertTrue(testCase,topSegment->getLength()==5);
	CuAssertTrue(testCase,topSegment->getStartPosition()==85);

	topIterator=currSequence->getTopSegmentIterator(10);
	topSegment=topIterator->getTopSegment();

	CuAssertTrue(testCase,topSegment->getArrayIndex()==10);
	CuAssertTrue(testCase,topSegment->getLength()==32);
	CuAssertTrue(testCase,topSegment->getStartPosition()==265);

	//3rd sequence - bottom segments
	currSequence=testScanner.getAlignment()->openGenome("Anc1")->getSequence("Anc1.0");
	hal::BottomSegmentIteratorPtr bottomIterator=currSequence->getBottomSegmentIterator(0);
	hal::BottomSegment* bottomSegment=bottomIterator->getBottomSegment();

	CuAssertTrue(testCase,bottomSegment->getArrayIndex()==0);
	CuAssertTrue(testCase,bottomSegment->getLength()==7);
	CuAssertTrue(testCase,bottomSegment->getStartPosition()==0);


	bottomIterator=currSequence->getBottomSegmentIterator(3);
	bottomSegment=bottomIterator->getBottomSegment();

	CuAssertTrue(testCase,bottomSegment->getArrayIndex()==3);
	CuAssertTrue(testCase,bottomSegment->getLength()==75);
	CuAssertTrue(testCase,bottomSegment->getStartPosition()==10);

	bottomIterator=currSequence->getBottomSegmentIterator(5);
	bottomSegment=bottomIterator->getBottomSegment();

	CuAssertTrue(testCase,bottomSegment->getArrayIndex()==5);
	CuAssertTrue(testCase,bottomSegment->getLength()==12);
	CuAssertTrue(testCase,bottomSegment->getStartPosition()==90);


	removeTempFile(AlignmentTempFile);
}

CuSuite* cactusHalScanSegmentsTestSuite(void)
{

  CuSuite* suite = CuSuiteNew();
  SUITE_ADD_TEST(suite, cactusHalScanSegmentsScanSequenceTest);
  SUITE_ADD_TEST(suite, cactusHalScanSegmentsScanTopSegmentTest);
  SUITE_ADD_TEST(suite, cactusHalScanSegmentsScanBottomSegmentTest);
  SUITE_ADD_TEST(suite, cactusHalScanSegmentsLoadSegmentsTest);
  return suite;
}
