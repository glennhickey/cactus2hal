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
    throw runtime_error("error opening fasta file " + faPath);
  }
}

void FastaReader::close()
{
  _faFile.close();
  _bookmarks.clear();
}

void FastaReader::skip()
{
  // Skip to the next non-comment line in the FASTA. Assumes we are currently at
  // the start of a line.
  char buffer;
  while (!_faFile.bad() && 
         (isspace(_faFile.peek()) || _faFile.peek() == ';'))
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

void FastaReader::parseHeader(string& name, string& comment) 
{
  // Parse out a sequence name and a comment from a FASTA header. Populates its
  // arguments. Returns the stream position of the ">" for ther header.
  
  string header = "";
  
  while(header.size() == 0) 
  {
    if(!getline(_faFile, header)) 
    {
      stringstream ss;
      ss << "Error reading sequence header";
      throw runtime_error(ss.str());
    }
  }
  
  if(header[0] != '>') 
  {
    stringstream ss;
    ss << "Error: expected \">\"";
    throw runtime_error(ss.str());
  }
  
  // Where does the sequence name stop and the sequence comment start?
  size_t nameStop = header.find_first_of(", \t\v");
  
  if(nameStop != string::npos) 
  {
    // Parse out name and comment.
    name = header.substr(1, nameStop - 1);
    comment = header.substr(nameStop, header.size() - nameStop + 1);
  } 
  else 
  {
    // Parse out name and give a blank comment.
    name = header.substr(1, header.size() - 1);
    comment = "";
  }
  
}

streampos FastaReader::skipToSequence(const string& sequenceName)
{
  // Skip to the record for the given sequence.
  // Returns the stream position of the ">".
  string name;
  string comment;
  streampos toReturn;
  while (_faFile.bad() != true)
  {
    skip();
    if (_faFile.peek() != '>')
    {
      stringstream ss;
      ss << "Error skipping to sequence, " << sequenceName;
      ss << ".. got \"" << (char)_faFile.peek() << "\" when expecting \">\"";
      throw runtime_error(ss.str());
    }
    // We found the ">"
    toReturn = _faFile.tellg();
    parseHeader(name, comment);
    if (name == sequenceName)
    {
      break;
    }
  }
  
  // We know it was the right one. Return its location.
  return toReturn;
}

void FastaReader::bookmarkNextSequence(const string& genomeName,
                                       const string& sequenceName)
{
  // Work out where that sequence is.
  streampos seqPos = skipToSequence(sequenceName);
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
  _bookmarks.insert(pair<SeqKey, streampos>(key, seqPos));
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
  
  // Seek back to the ">" for that sequence.
  _faFile.seekg(i->second);

  if (!_faFile)
  {
    stringstream ss;
    ss << "Error reading sequence " << genomeName << ", "
       << sequenceName << " from fasta file";
    throw runtime_error(ss.str());
  }
  string name;
  string comment;
  parseHeader(name, comment);
  if (!_faFile || name.length() < 1)
  {
    stringstream ss;
    ss << "Error reading sequence " << genomeName << ", "
       << sequenceName << " from fasta file";
    throw runtime_error(ss.str());
  }
  if (name != sequenceName)
  {
    stringstream ss;
    ss << "Error reading sequence " << genomeName << ", "
       << sequenceName << "; Got >" << name << " " << comment << " instead";
    throw runtime_error(ss.str());

  }

  skip();
  
  outSequence.erase();
  string buffer;
  while (_faFile && _faFile.peek() != '>' && getline(_faFile, buffer))
  {
    // For each FASTA line
    
    if(buffer.size() == 0 || buffer[0] == ';')
    {
      // Skip empty lines and comment lines
      continue;
    }
    
    // Copy the line (without trailing newline) to the output sequence.
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
