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


int main(int argc, char** argv)
{
  CLParserPtr optionsParser = hdf5CLParserInstance(true);
  optionsParser->addArgument("cactus .hal file", 
                             "path to cactus hal file to import");
  optionsParser->addArgument("cactus dbString", 
                             "XML database element as string");
  optionsParser->addArgument("output hal path",
                             "path of hal file to append cactus subtree");
  optionsParser->setDescription("Append a cactus databse to a hal database"
                                ". If the hal database doesn't exist, a new "
                                "one is created");
  try
  {
    optionsParser->parseOptions(argc, argv);
  }
  catch(exception& e)
  {
    cerr << e.what() << endl;
    optionsParser->printUsage(cerr);
    exit(1);
  }
  // try
  {
    string halFilePath = optionsParser->getArgument<string>("cactus .hal file");
    string dbString = optionsParser->getArgument<string>("cactus dbString");
    string outputPath = optionsParser->getArgument<string>("output hal path");

    AlignmentPtr alignment;

    if (!ifstream(halFilePath.c_str()))
    {
      throw hal_exception("Error opening" + halFilePath);
    }

    if (ifstream(outputPath.c_str()))
    {
      alignment = openHalAlignment(outputPath, optionsParser);
    }
    else
    {
      alignment = hdf5AlignmentInstance();
      alignment->setOptionsFromParser(optionsParser);
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
