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

string tempFilePath="/home/bozdugan/output/tmp/test_Anc1.hal";
string DB_Path="<st_kv_database_conf type=\"tokyo_cabinet\">"
				"<tokyo_cabinet database_dir=\"/home/bozdugan/hal_multiple/blanchette0/Anc1/Anc1/Anc1_DB\" database_name=\"An10\" />"
				"</st_kv_database_conf>";
string outgroup="Anc7";
CactusHalScanDimensions scanner;
void cactusHalScanDimensionsSetup(CuTest *testCase)
{

	scanner.scanDimensions(tempFilePath,DB_Path);
}
void cactusHalScanDimensionsParentTest(CuTest *testCase)
{

	const string* parentName1=scanner.getParentName();

	CuAssertTrue(testCase,*parentName1=="Anc1");
}

void cactusHalScanDimensionsOutgroupTest(CuTest *testCase)
{

	GenMapType::const_iterator i;
	for (i = scanner.getDimensionsMap()->begin();
				i != scanner.getDimensionsMap()->end(); ++i)
	{

		CuAssertTrue(testCase,i->first!=outgroup);
	}
}

void cactusHalScanDimensionsLoadDimensionsIntoHalTest(CuTest *testCase)
{
	hal::AlignmentPtr theNewAlignment=hdf5AlignmentInstance();
	string AlignmentTempFile = getTempFile();
	theNewAlignment->createNew(AlignmentTempFile);
	scanner.loadDimensionsIntoHal(theNewAlignment,outgroup);
	GenMapType::const_iterator i;
		for (i = scanner.getDimensionsMap()->begin();
					i != scanner.getDimensionsMap()->end(); ++i)
		{

			cout<<i->first<<i->second<<"\n";
		}
}

void CactusHalScanDimensionsConvertHalDimensionsTest(CuTest *testCase)
{
	vector<hal::Sequence::Info> dimsToFormat;
	bool isParent=TRUE;
	const hal::Sequence::Info info1=hal::Sequence::Info("Rat",1975,200,300);
	const hal::Sequence::Info info2=hal::Sequence::Info("Dawg",1971,30,5);

	dimsToFormat.push_back(info1);
	dimsToFormat.push_back(info2);
	vector<hal::Sequence::UpdateInfo>* formattedDimsTrue=scanner.convertHalDimensions(&dimsToFormat,&isParent);

	CuAssertTrue(testCase,(*formattedDimsTrue)[0]._name=="Rat");
	CuAssertTrue(testCase,(*formattedDimsTrue)[0]._numSegments==300);
	CuAssertTrue(testCase,(*formattedDimsTrue)[1]._name=="Dawg");
	CuAssertTrue(testCase,(*formattedDimsTrue)[1]._numSegments==5);

	isParent=FALSE;
	vector<hal::Sequence::UpdateInfo>* formattedDimsFalse=scanner.convertHalDimensions(&dimsToFormat,&isParent);
	CuAssertTrue(testCase,(*formattedDimsFalse)[0]._name=="Rat");
	CuAssertTrue(testCase,(*formattedDimsFalse)[0]._numSegments==200);
	CuAssertTrue(testCase,(*formattedDimsFalse)[1]._name=="Dawg");
	CuAssertTrue(testCase,(*formattedDimsFalse)[1]._numSegments==30);
}



CuSuite* CactusHalScanDimensionsTestSuite(void)
{

  CuSuite* suite = CuSuiteNew();
  SUITE_ADD_TEST(suite, cactusHalScanDimensionsSetup);
  SUITE_ADD_TEST(suite, cactusHalScanDimensionsParentTest);
  //SUITE_ADD_TEST(suite,cactusHalScanDimensionsOutgroupTest);
  SUITE_ADD_TEST(suite, CactusHalScanDimensionsConvertHalDimensionsTest);
  SUITE_ADD_TEST(suite, cactusHalScanDimensionsLoadDimensionsIntoHalTest);
  return suite;
}
