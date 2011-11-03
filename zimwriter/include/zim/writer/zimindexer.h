/*
 * Copyright (C) 2009 Tommi Maekitalo
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

#ifndef ZIM_WRITER_ZIMINDEXER_H
#define ZIM_WRITER_ZIMINDEXER_H

#include "zim/writer/articleparser.h"
#include "zim/writer/mstream.h"
#include "zim/zim.h"
#include <set>
#include <string>

namespace zim
{
  namespace writer
  {
    class Zimindexer : public ArticleParseEventEx
    {
      public:
        struct Wordentry
        {
          uint32_t aid;
          uint32_t pos;
          unsigned char weight;
          static const unsigned size = 9;
        };

      private:
        MStream& ostream;
        bool inTitle;
        zim::size_type aid;

        typedef std::set<std::string> TrivialWordsType;
        TrivialWordsType trivialWords;

        void insertWord(const std::string& word, unsigned char weight, unsigned pos);

      public:
        explicit Zimindexer(MStream& mstream)
          : ostream(mstream)
          { }

        void addTrivialWord(const std::string& word)
          { trivialWords.insert(word); }
        TrivialWordsType::size_type countTrivialWords() const
          { return trivialWords.size(); }

        void process(unsigned aid_, const std::string& title, const char* data, unsigned size);

        void onH1(const std::string& word, unsigned pos);
        void onH2(const std::string& word, unsigned pos);
        void onH3(const std::string& word, unsigned pos);
        void onB(const std::string& word, unsigned pos);
        void onP(const std::string& word, unsigned pos);
    };

  }
}

#endif // ZIM_WRITER_ZIMINDEXER_H
