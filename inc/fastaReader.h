/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#ifndef _FASTAREADER_H
#define _FASTAREADER_H

#include <string>
#include <map>
#include <fstream>

/**
 * read fasta file.
 */
class FastaReader
{
public:
   FastaReader();
   FastaReader(const std::string& faPath);
   ~FastaReader();
   void open(const std::string& faPath);
   void close();

   void getSequence(const std::string& genomeName,
                    const std::string& sequenceName,
                    std::string& outSequence);

   void bookmarkNextSequence(const std::string& genomeName,
                             const std::string& sequenceName);

protected:

   void skip();
   void skipToSequence(const std::string& sequenceName);

   typedef std::pair<std::string, std::string> SeqKey;
   typedef std::map<SeqKey, std::streampos> Bookmarks;

   Bookmarks _bookmarks;
   std::ifstream _faFile;
};

#endif
