/*
 * Copyright (C) 2008 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include "zim/writer/zimindexer.h"
#include "zim/writer/indexersource.h"
#include "zim/writer/zimcreator.h"
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

log_define("zim.writer.indexer")

namespace zim
{
  namespace writer
  {
    const unsigned Zimindexer::Wordentry::size;

    void Zimindexer::insertWord(const std::string& word, unsigned char weight, unsigned pos)
    {
      log_debug(word << '\t' << pos << '\t' << aid << '\t' << static_cast<unsigned>(weight));

      Wordentry w;
      w.aid = aid;
      w.pos = pos;
      w.weight = weight;
      ostream.write(word, reinterpret_cast<char*>(&w), Wordentry::size);
    }

    void Zimindexer::process(zim::size_type aid_, const std::string& title, const char* data, unsigned size)
    {
      aid = aid_;

      ArticleParser parser(*this);

      inTitle = true;
      parser.parse(title);
      parser.endparse();

      inTitle = false;
      parser.parse(data, size);
      parser.endparse();
    }

    void Zimindexer::onH1(const std::string& word, unsigned pos)
    {
      if (trivialWords.find(word) == trivialWords.end())
        insertWord(word, 0, pos);
    }

    void Zimindexer::onH2(const std::string& word, unsigned pos)
    {
      if (trivialWords.find(word) == trivialWords.end())
        insertWord(word, 1, pos);
    }

    void Zimindexer::onH3(const std::string& word, unsigned pos)
    {
      if (trivialWords.find(word) == trivialWords.end())
        insertWord(word, 2, pos);
    }

    void Zimindexer::onB(const std::string& word, unsigned pos)
    {
      if (trivialWords.find(word) == trivialWords.end())
        insertWord(word, 2, pos);
    }

    void Zimindexer::onP(const std::string& word, unsigned pos)
    {
      if (trivialWords.find(word) == trivialWords.end())
        insertWord(word, inTitle ? 0 : 3, pos);
    }

  }
}

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<const char*> tmpfilename(argc, argv, 't', "zimindexer.tmp");
    cxxtools::Arg<const char*> trivialWordsFile(argc, argv, 'T');
    cxxtools::Arg<unsigned> memoryFactor(argc, argv, 'M', 64);

    zim::writer::ZimCreator creator(argc, argv);
    zim::writer::Indexer indexer(tmpfilename, trivialWordsFile, memoryFactor);

    if (argc != 3)
    {
        std::cout << "usage: " << argv[0] << " [options] zim-file index-filename\n"
                     "options:\n"
                     "\t-s <number>       specify chunk size for compression in kB (default 1024)\n"
                     "\t-T <file>         trivial words file for full text index (a text file with words, which are not indexed)\n"
                     "\t-M <number>       memory factor (default 64, smaller factors reduce memory usage but makes indexer slower,\n"
                     "\t                  try smaller values when you run out of memory)\n"
                     "\t-t <filename>     temporary file name (default zimindexer.tmp)\n";
        return -1;
    }

    const char* sourcefile = argv[1];
    const char* outfile = argv[2];

    indexer.setFilename(outfile);
    indexer.createIndex(sourcefile);
    creator.create(outfile, indexer);
  }
  catch (const std::exception& e)
  {
    log_fatal(e.what());
    std::cerr << e.what() << std::endl;
  }
}

