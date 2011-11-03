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

#ifndef ZIM_WRIER_ARTICLEPARSER_H
#define ZIM_WRIER_ARTICLEPARSER_H

#include <string>
#include <stdint.h>

namespace zim
{
  namespace writer
  {
    class ArticleParseEvent
    {
      public:
        virtual void onTag(const std::string& tag, unsigned pos) = 0;
        virtual void onEndtag(const std::string& tag, unsigned pos) = 0;
        virtual void onWord(const std::string& word, unsigned pos) = 0;
    };

    class ArticleParseEventEx : public ArticleParseEvent
    {
        std::string tag;

      public:
        virtual void onTag(const std::string& tag, unsigned pos);
        virtual void onEndtag(const std::string& tag, unsigned pos);
        virtual void onWord(const std::string& word, unsigned pos);

        virtual void onH1(const std::string& word, unsigned pos) = 0;
        virtual void onH2(const std::string& word, unsigned pos) = 0;
        virtual void onH3(const std::string& word, unsigned pos) = 0;
        virtual void onB(const std::string& word, unsigned pos) = 0;
        virtual void onP(const std::string& word, unsigned pos) = 0;
    };

    class ArticleParser
    {
        enum State
        {
          state_0,
          state_tag0,
          state_tag,
          state_endtag,
          state_tagskip,
          state_ent,
          state_word,
          state_wordent,
          state_utf8,
          state_wordutf8,
        } state;

        ArticleParseEvent& event;
        std::string word;
        std::string entity;
        unsigned pos;
        std::string utf8char;
        uint32_t utf8value;
        unsigned utf8counter;

        void parseEntityChar();

      public:
        ArticleParser(ArticleParseEvent& event_)
          : state(state_0),
            event(event_),
            pos(0)
          { }
        void parse(char ch);
        void parse(const char* str)
        {
          while (*str)
            parse(*str++);
        }
        void parse(const char* str, unsigned n)
        {
          for (unsigned nn = 0; nn < n; ++nn)
            parse(str[nn]);
        }
        void parse(const std::string& str)
        {
          for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
            parse(*it);
        }

        void endparse();
        void reset()    { pos = 0; }
    };

  }
}

#endif //  ZIM_ARTICLEPARSER_H
