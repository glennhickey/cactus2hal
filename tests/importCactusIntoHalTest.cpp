/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */
#include "allTests.h"

void test1(CuTest *testCase)
{
  CuAssertTrue(testCase, 1);
}

CuSuite* importCactusIntoHalSuite(void) 
{
  CuSuite* suite = CuSuiteNew();
  SUITE_ADD_TEST(suite, test1);
  return suite;
}
