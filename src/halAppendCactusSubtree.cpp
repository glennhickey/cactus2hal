/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "cactusHalConverter.h"

using namespace std;
using namespace hal;

static void checkOptions(int argc, char** argv)
{
  if (argc != 4)
  {
    cerr << "Usage: halAppendCactusSubtree <cactus .hal file> <cactus dbString> "
       "<output hal path>" << endl;
    exit(1);
  }
}

int main(int argc, char** argv)
{
  checkOptions(argc, argv);
  // try
  {
    string halFilePath = argv[1];
    string dbString = argv[2];
    string outputPath = argv[3];

    AlignmentPtr alignment;

    if (!ifstream(halFilePath.c_str()))
    {
      throw hal_exception("Error opening" + halFilePath);
    }

    if (ifstream(outputPath.c_str()))
    {
      alignment = openHalAlignment(outputPath);
    }
    else
    {
      alignment = hdf5AlignmentInstance();
      alignment->createNew(outputPath);
    }
    CactusHalConverter converter;
    converter.convert(halFilePath, dbString, alignment);
  }
/*  catch(hal_exception& e)
  {
    cerr << "hal exception caught: " << e.what() << endl;
    return 1;
  }
   catch(exception& e)
  {
    cerr << "Exception caught: " << e.what() << endl;
    return 1;
  }
*/
  return 0;
}
