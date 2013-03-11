/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <fstream>

#include "cactusHalConverter.h"

using namespace std;
using namespace hal;


int main(int argc, char** argv)
{
  CLParserPtr optionsParser = hdf5CLParserInstance(true);
  optionsParser->addArgument("cactus .c2h file", 
                             "path to cactus hal file to import");
  optionsParser->addArgument("cactus .fa file", 
                             "path to cactus sequences file to import");
  optionsParser->addArgument("newick tree", 
                             "event tree for cactus db in Newick format");
  optionsParser->addArgument("output hal path",
                             "path of hal file to append cactus subtree");
  optionsParser->addOption("outgroups",
                           "comma-separated list of outgroup events "
                           "which will be skipped by the conversion.",
                           "\"\"");
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
  try
  {
    string halFilePath = optionsParser->getArgument<string>("cactus .c2h file");
    string faFilePath = optionsParser->getArgument<string>("cactus .fa file");
    string treeString = optionsParser->getArgument<string>("newick tree");
    string outputPath = optionsParser->getArgument<string>("output hal path");
    string ogString = optionsParser->getOption<string>("outgroups");
    vector<string> outgroups;
    if (ogString != "\"\"")
    {
      outgroups = chopString(ogString, ",");
    }

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
    converter.convert(halFilePath, faFilePath, treeString, alignment, 
                      outgroups);
  }
  catch(hal_exception& e)
  {
    cerr << "hal exception caught: " << e.what() << endl;
    return 1;
  }
   catch(exception& e)
  {
    cerr << "Exception caught: " << e.what() << endl;
    return 1;
  }

  return 0;
}
