/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <zim/writer/search.h>
#include <zim/writer/zimcreator.h>

log_define("zim.writer.search")

#define INFO(e) \
    do { \
        log_info(e); \
        std::cout << e << std::endl; \
    } while(false)

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<std::string> search(argc, argv, 'S'); // create zimfile from search result

    if (argc != 2)
    {
      std::cout << "usage: " << argv[0] << " [options] output-filename\n"
                   "\t\".zim\" is appended to the output filename if not already given.\n"
                   "\n"
                   "options:\n"
                   "\t-s <number>       specify chunk size for compression in kB (default 1024)\n"
                   "\t-I <articlefile>  article file for search\n"
                   "\t-S <words>        search in zim file for articles\n"
                   "\t-X <indexfile>    use indexfile as full text search index\n"
                   "\t-T <file>         trivial words file for full text index (a text file with words, which are not indexed)\n"
                   "\t-M <number>       memory factor (default 64, smaller factors reduce memory usage but makes indexer slower,\n"
                   "\t                  try smaller values when you run out of memory)\n"
                   "\t-t <filename>     temporary file name (default zimwriter.tmp)\n";
      return 1;
    }

    INFO("create zim file from search result");
    std::string fname = argv[1];
    zim::writer::ZimCreator creator(argc, argv);
    zim::writer::SearchSource source(search, argc, argv);
    creator.create(fname, source);
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

