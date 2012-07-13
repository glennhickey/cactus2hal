/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cctype>
#include "fastaReader.h"

using namespace std;

FastaReader::FastaReader() 
{

}

FastaReader::FastaReader(const string& faPath)
{
  open(faPath);
}

FastaReader::~FastaReader()
{
  close();
}

void FastaReader::open(const string& faPath)
{
  close();
  _faFile.open(faPath.c_str());
  if (!_faFile)
  {
    throw runtime_error("error opening fasta file" + faPath);
  }
}

void FastaReader::close()
{
  _faFile.close();
  _bookmarks.clear();
}

void FastaReader::skip()
{
  char buffer;
  while (!_faFile.bad() && 
         (isspace(_faFile.peek()) == true || _faFile.peek() == ';'))
  {
    _faFile.get(buffer);
    if (buffer == ';')
    {
      while (_faFile && buffer != '\n')
      {
        _faFile.get(buffer);
      }
    }
  }
}

void FastaReader::skipToSequence(const string& sequenceName)
{
  string buffer;
  char c;
  while (_faFile.bad() != true)
  {
    skip();
    if (_faFile.peek() != '>')
    {
      stringstream ss;
      ss << "Error skipping to sequence, " << sequenceName;
      throw runtime_error(ss.str());
    }
    _faFile.get(c);
    _faFile >> buffer;
    if (buffer == sequenceName)
    {
      break;
    }
  }    
}

void FastaReader::bookmarkNextSequence(const string& genomeName,
                                       const string& sequenceName)
{
  skipToSequence(sequenceName);
  if (!_faFile)
  {
    stringstream ss;
    ss << "Error scanning next sequence for " << genomeName << ", "
       << sequenceName << " from fasta file";
    throw runtime_error(ss.str());
  }
  SeqKey key(genomeName, sequenceName);
  if (_bookmarks.find(key) != _bookmarks.end())
  {
    stringstream ss;
    ss << "Error bookmarking sequence for " << genomeName << ", "
       << sequenceName << ": already added to map";
    throw runtime_error(ss.str());
  }
  streampos pos = _faFile.tellg() - (streamoff)sequenceName.length();
  _bookmarks.insert(pair<SeqKey, streampos>(key, pos));
  char buf;
  do
  {
    _faFile.get(buf);
  } while (_faFile && _faFile.peek() != '>');
}

void FastaReader::getSequence(const string& genomeName,
                              const string& sequenceName,
                              string& outSequence)
{
  _faFile.clear();

  Bookmarks::const_iterator i = _bookmarks.find(SeqKey(genomeName, 
                                                       sequenceName));
  if (i == _bookmarks.end())
  {
    stringstream ss;
    ss << "Sequence index for " << genomeName << ", "
       << sequenceName << ": was not scanned from fasta file";
    throw runtime_error(ss.str());
  }
  
  _faFile.seekg(i->second);

  if (!_faFile)
  {
    stringstream ss;
    ss << "Error reading sequence " << genomeName << ", "
       << sequenceName << " from fasta file";
    throw runtime_error(ss.str());
  }
  string header;
  _faFile >> header;
  if (!_faFile || header.length() < 1)
  {
    stringstream ss;
    ss << "Error reading sequence " << genomeName << ", "
       << sequenceName << " from fasta file";
    throw runtime_error(ss.str());
  }
  if (header != sequenceName)
  {
    stringstream ss;
    ss << "Error reading sequence " << genomeName << ", "
       << sequenceName << " differs from header " << header;
    throw runtime_error(ss.str());

  }

  skip();
  
  outSequence.erase();
  string buffer;
  while (_faFile && !isspace(_faFile.peek()) && _faFile.peek() != ';' &&
         _faFile.peek() != '>')
  {
    _faFile >> buffer;
    outSequence += buffer;
  }
  
  if (outSequence.length() == 0)
  {
    stringstream ss;
    ss << "Error reading sequence " << genomeName << ", "
       << sequenceName << " from fasta file (was empty)";
    throw runtime_error(ss.str());
  }
}
