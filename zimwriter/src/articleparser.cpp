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

#include "zim/writer/articleparser.h"
#include <map>
#include <cctype>
#include <cxxtools/log.h>
#include <zim/unicode.h>

log_define("zim.writer.articleparser")

namespace zim
{
  namespace writer
  {
    void ArticleParser::parse(char ch)
    {
      switch (state)
      {
        case state_0:
          if (ch == '<')
            state = state_tag0;
          else if (ch == '&')
          {
            state = state_ent;
          }
          else if (static_cast<unsigned char>(ch) >> 5 == 6)
          {
            utf8counter = 1;
            utf8char = ch;
            utf8value = static_cast<unsigned char>(ch) & 0x1f;
            word.clear();
            state = state_utf8;
          }
          else if (static_cast<unsigned char>(ch) >> 4 == 0xe)
          {
            utf8counter = 2;
            utf8char = ch;
            utf8value = static_cast<unsigned char>(ch) & 0xf;
            word.clear();
            state = state_utf8;
          }
          else if (static_cast<unsigned char>(ch) >> 3 == 0x1e)
          {
            utf8counter = 3;
            utf8char = ch;
            utf8value = static_cast<unsigned char>(ch) & 0x7;
            word.clear();
            state = state_utf8;
          }
          else if (std::isalnum(ch))
          {
            word = std::tolower(ch);
            state = state_word;
          }
          break;

        case state_tag0:
          if (ch == '/')
          {
            word.clear();
            state = state_endtag;
          }
          else if (std::isalnum(ch))
          {
            word = ch;
            state = state_tag;
          }
          break;

        case state_tag:
          if (ch == '>')
          {
            log_debug("onTag(" << word << ')');
            event.onTag(word, pos);
            state = state_0;
          }
          else if (std::isalnum(ch))
            word += std::tolower(ch);
          else
          {
            log_debug("onTag(" << word << ')');
            event.onTag(word, pos);
            state = state_tagskip;
          }
          break;

        case state_endtag:
          if (ch == '>')
          {
            log_debug("onEndtag(" << word << ')');
            event.onEndtag(word, pos);
            state = state_0;
          }
          else if (std::isalnum(ch))
            word += std::tolower(ch);
          else
          {
            log_debug("onEndtag(" << word << ')');
            event.onEndtag(word, pos);
            state = state_tagskip;
          }
          break;

        case state_tagskip:
          if (ch == '>')
            state = state_0;
          break;

        case state_ent:
          if (ch == ';')
          {
            state = state_0;
            parseEntityChar();
            entity.clear();
          }
          else
            entity += ch;
          break;

        case state_word:
          if (ch == '&')
            state = state_wordent;
          else if (static_cast<unsigned char>(ch) >> 5 == 6)
          {
            utf8counter = 1;
            utf8char = ch;
            utf8value = static_cast<unsigned char>(ch) & 0x1f;
            state = state_wordutf8;
          }
          else if (static_cast<unsigned char>(ch) >> 4 == 0xe)
          {
            utf8counter = 2;
            utf8char = ch;
            utf8value = static_cast<unsigned char>(ch) & 0xf;
            state = state_wordutf8;
          }
          else if (static_cast<unsigned char>(ch) >> 3 == 0x1e)
          {
            utf8counter = 3;
            utf8char = ch;
            utf8value = static_cast<unsigned char>(ch) & 0x7;
            state = state_wordutf8;
          }
          else if (std::isalnum(ch))
            word += std::tolower(ch);
          else
          {
            event.onWord(word, pos - word.size());
            state = ch == '<' ? state_tag0 : state_0;
          }
          break;

        case state_wordent:
          if (ch == ';')
          {
            state = state_word;
            parseEntityChar();
            entity.clear();
          }
          else
            entity += ch;
          break;

        case state_utf8:
        case state_wordutf8:
          if (static_cast<unsigned char>(ch) >> 6 == 2)
          {
            utf8char += ch;
            utf8value = (utf8value << 6) | static_cast<unsigned char>(ch) & 0x3f;
            if (--utf8counter == 0)
            {
              if (zim::isalnum(utf8value))
              {
                log_debug("utf8value " << utf8value << " is alphanumeric");
                // TODO use zim::tolower
                //if (utf8char != zim::tolower(utf8char))
                  // ;
                word += utf8char;
                state = state_word;
              }
              else
              {
                log_debug("utf8value " << utf8value << " is not alphanumeric");
                if (state == state_wordutf8)
                  event.onWord(word, pos - word.size() - utf8char.size() + 1);
                state = state_0;
              }
            }
          }
          else
          {
            // invalid utf8 encoding - skip it
            if (state == state_wordutf8)
              event.onWord(word, pos - word.size() - utf8char.size());
            state = state_0;
          }
          break;
      }

      ++pos;
      //log_debug("ch " << ch << " => " << state);
    }

    void ArticleParser::parseEntityChar()
    {
      const char* ct [][2] =
        {
          { "Acirc", "\xc3\x82" },
          { "acirc", "\xc3\xa2" },
          { "AElig", "\xc3\x86" },
          { "aelig", "\xc3\xa6" },
          { "Agrave", "\xc3\x80" },
          { "agrave", "\xc3\xa0" },
          { "alefsym", "\xe2\x84\xb5" },
          { "Alpha", "\xce\x91" },
          { "alpha", "\xce\xb1" },
          { "amp", "\x26" },
          { "Aring", "\xc3\x85" },
          { "aring", "\xc3\xa5" },
          { "Atilde", "\xc3\x83" },
          { "atilde", "\xc3\xa3" },
          { "Auml", "\xc3\x84" },
          { "auml", "\xc3\xa4" },
          { "Beta", "\xce\x92" },
          { "beta", "\xce\xb2" },
          { "ccedil", "\xc3\xa7" },
          { "cedil", "\xc2\xb8" },
          { "cent", "\xc2\xa2" },
          { "Chi", "\xce\xa7" },
          { "chi", "\xcf\x87" },
          { "curren", "\xc2\xa4" },
          { "deg", "\xc2\xb0" },
          { "Delta", "\xce\x94" },
          { "delta", "\xce\xb4" },
          { "Eacute", "\xc3\x89" },
          { "eacute", "\xc3\xa9" },
          { "Ecirc", "\xc3\x8a" },
          { "ecirc", "\xc3\xaa" },
          { "Egrave", "\xc3\x88" },
          { "egrave", "\xc3\xa8" },
          { "Epsilon", "\xce\x95" },
          { "epsilon", "\xce\xb5" },
          { "Eta", "\xce\x97" },
          { "eta", "\xce\xb7" },
          { "ETH", "\xc3\x90" },
          { "eth", "\xc3\xb0" },
          { "Euml", "\xc3\x8b" },
          { "euml", "\xc3\xab" },
          { "euro", "\xe2\x82\xac" },
          { "fnof", "\xc6\x92" },
          { "Gamma", "\xce\x93" },
          { "gamma", "\xce\xb3" },
          { "Iacute", "\xc3\x8d" },
          { "iacute", "\xc3\xad" },
          { "Icirc", "\xc3\x8e" },
          { "icirc", "\xc3\xae" },
          { "iexcl", "\xc2\xa1" },
          { "Igrave", "\xc3\x8c" },
          { "igrave", "\xc3\xac" },
          { "Iota", "\xce\x99" },
          { "iota", "\xce\xb9" },
          { "Iuml", "\xc3\x8f" },
          { "iuml", "\xc3\xaf" },
          { "Kappa", "\xce\x9a" },
          { "kappa", "\xce\xba" },
          { "Lambda", "\xce\x9b" },
          { "lambda", "\xce\xbb" },
          { "micro", "\xc2\xb5" },
          { "Mu", "\xce\x9c" },
          { "mu", "\xce\xbc" },
          { "Ntilde", "\xc3\x91" },
          { "ntilde", "\xc3\xb1" },
          { "Nu", "\xce\x9d" },
          { "nu", "\xce\xbd" },
          { "Oacute", "\xc3\x93" },
          { "oacute", "\xc3\xb3" },
          { "Ocirc", "\xc3\x94" },
          { "ocirc", "\xc3\xb4" },
          { "OElig", "\xc5\x92" },
          { "oelig", "\xc5\x93" },
          { "Ograve", "\xc3\x92" },
          { "ograve", "\xc3\xb2" },
          { "Omega", "\xce\xa9" },
          { "omega", "\xcf\x89" },
          { "Omicron", "\xce\x9f" },
          { "omicron", "\xce\xbf" },
          { "ordf", "\xc2\xaa" },
          { "ordm", "\xc2\xba" },
          { "Oslash", "\xc3\x98" },
          { "oslash", "\xc3\xb8" },
          { "Otilde", "\xc3\x95" },
          { "otilde", "\xc3\xb5" },
          { "Ouml", "\xc3\x96" },
          { "ouml", "\xc3\xb6" },
          { "para", "\xc2\xb6" },
          { "Phi", "\xce\xa6" },
          { "phi", "\xcf\x86" },
          { "Pi", "\xce\xa0" },
          { "pi", "\xcf\x80" },
          { "piv", "\xcf\x96" },
          { "pound", "\xc2\xa3" },
          { "Psi", "\xce\xa8" },
          { "psi", "\xcf\x88" },
          { "real", "\xe2\x84\x9c" },
          { "Rho", "\xce\xa1" },
          { "rho", "\xcf\x81" },
          { "Scaron", "\xc5\xa0" },
          { "scaron", "\xc5\xa1" },
          { "shy", "" },
          { "Sigma", "\xce\xa3" },
          { "sigma", "\xcf\x83" },
          { "sigmaf", "\xcf\x82" },
          { "szlig", "\xc3\x9f" },
          { "Tau", "\xce\xa4" },
          { "tau", "\xcf\x84" },
          { "Theta", "\xce\x98" },
          { "theta", "\xce\xb8" },
          { "thetasym", "\xcf\x91" },
          { "THORN", "\xc3\x9e" },
          { "thorn", "\xc3\xbe" },
          { "Uacute", "\xc3\x9a" },
          { "uacute", "\xc3\xba" },
          { "Ucirc", "\xc3\x9b" },
          { "ucirc", "\xc3\xbb" },
          { "Ugrave", "\xc3\x99" },
          { "ugrave", "\xc3\xb9" },
          { "upsih", "\xcf\x92" },
          { "Upsilon", "\xce\xa5" },
          { "upsilon", "\xcf\x85" },
          { "Uuml", "\xc3\x9c" },
          { "uuml", "\xc3\xbc" },
          { "weierp", "\xe2\x84\x98" },
          { "Xi", "\xce\x9e" },
          { "xi", "\xce\xbe" },
          { "Yacute", "\xc3\x9d" },
          { "yacute", "\xc3\xbd" },
          { "yen", "\xc2\xa5" },
          { "Yuml", "\xc5\xb8" },
          { "yuml", "\xc3\xbf" },
          { "Zeta", "\xce\x96" },
          { "zeta", "\xce\xb6" },
          { 0, 0 }
        };
      typedef std::map<std::string, const char*> EntityMapType;
      static EntityMapType entityMap;

      if (entityMap.empty())
      {
        for (unsigned n = 0; ct[n][0]; ++n)
        {
          entityMap[ct[n][0]] = ct[n][1];
        }
      }

      EntityMapType::const_iterator it = entityMap.find(entity);
      if (it != entityMap.end())
        parse(it->second);
      else
        parse(' ');
    }

    void ArticleParser::endparse()
    {
      switch (state)
      {
        case state_0:
        case state_ent:
        case state_tag0:
        case state_tag:
        case state_endtag:
        case state_tagskip:
          break;

        case state_word:
        case state_wordent:
          {
            event.onWord(word, pos - word.size());
            state = state_0;
          }
          break;

        case state_utf8:
          break;

        case state_wordutf8:
          if (!word.empty())
            event.onWord(word, pos - word.size());
          break;
      }
    }

    void ArticleParseEventEx::onTag(const std::string& tag_, unsigned pos)
    {
      if (tag_ == "h1" || tag_ == "H1"
        || tag_ == "h2" || tag_ == "H2"
        || tag_ == "h3" || tag_ == "H3"
        || tag_ == "b" || tag_ == "B")
        tag = tag_;
    }

    void ArticleParseEventEx::onEndtag(const std::string& tag_, unsigned pos)
    {
      if (tag_ == "h1" || tag_ == "H1"
        || tag_ == "h2" || tag_ == "H2"
        || tag_ == "h3" || tag_ == "H3"
        || tag_ == "b" || tag_ == "B")
        tag.clear();
    }

    void ArticleParseEventEx::onWord(const std::string& word, unsigned pos)
    {
      log_debug("onWord(\"" << word << "\", " << pos << "); tag=<" << tag << '>');
      if (tag == "h1" || tag == "H1")
      {
        log_debug("onH1(\"" << word << "\", " << pos << ')');
        onH1(word, pos);
      }
      else if (tag == "h2" || tag == "H2")
      {
        log_debug("onH2(\"" << word << "\", " << pos << ')');
        onH2(word, pos);
      }
      else if (tag == "h3" || tag == "H3")
      {
        log_debug("onH3(\"" << word << "\", " << pos << ')');
        onH3(word, pos);
      }
      else if (tag == "b" || tag == "B")
      {
        log_debug("onB(\"" << word << "\", " << pos << ')');
        onB(word, pos);
      }
      else
      {
        onP(word, pos);
        log_debug("onP(\"" << word << "\", " << pos << ')');
      }
    }

  }
}

