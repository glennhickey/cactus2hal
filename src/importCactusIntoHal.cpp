/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */
#include <cassert>
#include <stdexcept>
#include <iostream>
#include "cactus.h"
#include "cactusDbWrapper.h"

using namespace std;

static void usage(const char* exName)
{
  cout << "USAGE " << exName << " <cactus disk string>" << endl;
}

static void verifyArgs(int argc, char** argv)
{
  if (argc != 2)
  {
    usage(argv[0]);
    throw runtime_error("Invalid arguments");
  }
}

int main(int argc, char** argv)
{
  try
  {
    verifyArgs(argc, argv);
    CactusDbWrapper cactusDb(argv[1]);
    cactusDb.printSequenceNames();
  }
  catch (exception e)
  {
    cerr << e.what() <<endl;
    return -1;
  }
  return 0;
}
