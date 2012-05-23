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
#include "cactusHalScanner.h"

using namespace std;
using namespace hal;

// TEST INPUT STREAM OPERATORS FOR BASIC STRUCUTURES

void cactusHalScannerSequenceTest(CuTest *testCase)
{
  stringstream ss;
  ss << "	'simMouse'	'simMouse.chrP'	0";
  CactusHalSequence seq;
  ss >> seq;

  CuAssertTrue(testCase, seq._event == "simMouse");
  CuAssertTrue(testCase, seq._name == "simMouse.chrP");
  CuAssertTrue(testCase, seq._isBottom == false);

  stringstream ss2;
  ss2 << "	'sMouse-sRat'	'sMouse-sRat.0'	1";
  CactusHalSequence seq2;
  ss2 >> seq2;

  CuAssertTrue(testCase, seq2._event == "sMouse-sRat");
  CuAssertTrue(testCase, seq2._name == "sMouse-sRat.0");
  CuAssertTrue(testCase, seq2._isBottom == true);
}

void cactusHalScannerTopSegmentTest(CuTest *testCase)
{
  stringstream ss;
  ss << "	4797	16	197032483697481381	0";
  CactusHalTopSegment tseg;
  ss >> tseg;

  CuAssertTrue(testCase, tseg._start == 4797);
  CuAssertTrue(testCase, tseg._length == 16);
  CuAssertTrue(testCase, tseg._parent == 197032483697481381);
  CuAssertTrue(testCase, tseg._reversed == true);
  
  stringstream ss2;
  ss2 << "	4797	16	197032483697481381	1";
  CactusHalTopSegment tseg2;
  ss2 >> tseg2;

  CuAssertTrue(testCase, tseg2._reversed == false);

  stringstream ss3;
  ss3 << "	1	2000";
  CactusHalTopSegment tseg3;
  ss3 >> tseg3;

  CuAssertTrue(testCase, tseg3._start == 1);
  CuAssertTrue(testCase, tseg3._length == 2000);
  CuAssertTrue(testCase, tseg3._parent == hal::NULL_INDEX);
}

void cactusHalScannerBottomSegmentTest(CuTest *testCase)
{
  stringstream ss;
  ss << "	3105231943071985139	28	8";
  CactusHalBottomSegment bseg;
  ss >> bseg;

  CuAssertTrue(testCase, bseg._name == 3105231943071985139);
  CuAssertTrue(testCase, bseg._start == 28);
  CuAssertTrue(testCase, bseg._length == 8);
}


// TEST THE FILE PARSER
static char* tempFilePath;
static void setupSmallTempFile()
{
  tempFilePath = getTempFile();
  ofstream ofile(tempFilePath);
  ofile << "s	\'sMouse-sRat\'	\'sMouse-sRat.0\'	1\n";
  ofile << "a	3105231943071985136	0	28\n";
  ofile << "a	3105231943071985139	28	8\n";
  ofile << "s	\'simMouse\'	\'simMouse.chrP\'	0\n";
  ofile << "a	0	67	1918814916236546103	1\n";
  ofile << "a	7132328	2\n";
  ofile << "a	152	33	2772809995576667241	0\n";
  ofile.close();
}

static void tearDownTempFile()
{
  removeTempFile(tempFilePath);
}

class TestScanner : public CactusHalScanner
{
public:
   vector<CactusHalSequence> _sequences;
   vector<CactusHalTopSegment> _topSegments;
   vector<CactusHalBottomSegment> _bottomSegments;
   void scanSequence(CactusHalSequence& sequence)
   {
     _sequences.push_back(sequence);
   }
   void scanTopSegment(CactusHalTopSegment& topSegment)
   {
     _topSegments.push_back(topSegment);
   }
   void scanBottomSegment(CactusHalBottomSegment& botSegment)
   {
     _bottomSegments.push_back(botSegment);
   }
   void scanEndOfFile()
   {

   }
};

void cactusHalScannerSmallTest(CuTest *testCase)
{
  setupSmallTempFile();

  TestScanner scanner;
  scanner.scan(tempFilePath);
  
  CuAssertTrue(testCase, scanner._sequences.size() == 2);
  CuAssertTrue(testCase, scanner._sequences[0]._event == "sMouse-sRat");
  CuAssertTrue(testCase, scanner._sequences[0]._name == "sMouse-sRat.0");
  CuAssertTrue(testCase, scanner._sequences[0]._isBottom == 1);
  CuAssertTrue(testCase, scanner._sequences[1]._event == "simMouse");
  CuAssertTrue(testCase, scanner._sequences[1]._name == "simMouse.chrP");
  CuAssertTrue(testCase, scanner._sequences[1]._isBottom == 0);

  CuAssertTrue(testCase, scanner._bottomSegments.size() == 2);
  CuAssertTrue(testCase, 
               scanner._bottomSegments[0]._name == 3105231943071985136);
  CuAssertTrue(testCase, scanner._bottomSegments[0]._start == 0);
  CuAssertTrue(testCase, scanner._bottomSegments[0]._length == 28);
  CuAssertTrue(testCase, 
               scanner._bottomSegments[1]._name == 3105231943071985139);
  CuAssertTrue(testCase, scanner._bottomSegments[1]._start == 28);
  CuAssertTrue(testCase, scanner._bottomSegments[1]._length == 8);
  
  CuAssertTrue(testCase, scanner._topSegments.size() == 3);
  CuAssertTrue(testCase, scanner._topSegments[0]._start == 0);
  CuAssertTrue(testCase, scanner._topSegments[0]._length == 67);
  CuAssertTrue(testCase, scanner._topSegments[0]._parent == 1918814916236546103);
  CuAssertTrue(testCase, scanner._topSegments[0]._reversed == false);

  CuAssertTrue(testCase, scanner._topSegments[1]._start == 7132328);
  CuAssertTrue(testCase, scanner._topSegments[1]._length == 2);
  CuAssertTrue(testCase, scanner._topSegments[1]._parent == hal::NULL_INDEX);
  
  CuAssertTrue(testCase, scanner._topSegments[2]._start == 152);
  CuAssertTrue(testCase, scanner._topSegments[2]._length == 33);
  CuAssertTrue(testCase, scanner._topSegments[2]._parent == 2772809995576667241);
  CuAssertTrue(testCase, scanner._topSegments[2]._reversed == true);

  tearDownTempFile();
}

CuSuite* cactusHalScannerTestSuite(void) 
{
  CuSuite* suite = CuSuiteNew();
  SUITE_ADD_TEST(suite, cactusHalScannerSequenceTest);
  SUITE_ADD_TEST(suite, cactusHalScannerTopSegmentTest);
  SUITE_ADD_TEST(suite, cactusHalScannerBottomSegmentTest);
  SUITE_ADD_TEST(suite, cactusHalScannerSmallTest);
  return suite;
}

