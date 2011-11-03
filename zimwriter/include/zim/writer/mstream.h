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

#ifndef ZIM_WRITER_MSTREAM_H
#define ZIM_WRITER_MSTREAM_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <stdint.h>
#include <cxxtools/smartptr.h>
#include <cxxtools/refcounted.h>

#ifndef MSTREAM_PAGESIZE
#define MSTREAM_PAGESIZE 8192
#endif

namespace zim
{
  namespace writer
  {
    class Pagefile
    {
      public:
        typedef uint32_t PageNumber;
        static const PageNumber noPage = static_cast<PageNumber>(-1);
        static const unsigned PageSize = MSTREAM_PAGESIZE;

      private:
        std::string backfilename;
        std::fstream backfile;
        PageNumber nextPage;

      public:
        explicit Pagefile(const char* fname)
          : backfilename(fname),
            backfile(fname, std::ios::in | std::ios::out | std::ios::trunc),
            nextPage(0)
          { }
        ~Pagefile();

        struct Page
        {
          static const unsigned dataSize = PageSize - sizeof(uint16_t) - sizeof(PageNumber);

          PageNumber nextPage;
          uint16_t ppos;
          char data[dataSize];

          uint16_t free() const  { return dataSize - ppos; }
        };

        PageNumber getNewPage();
        void getPage(PageNumber num, Page& page);
        void putPage(PageNumber num, const Page& page, unsigned size = sizeof(Page));

    };

    class MStream
    {
        friend class Stream;

        static unsigned minBuffersize;
        static unsigned maxBuffersize;

        class Stream : public cxxtools::RefCounted
        {
            Pagefile* pagefile;

            Pagefile::PageNumber firstPage;
            Pagefile::PageNumber currentPage;
            uint16_t ppos;

            unsigned buffersize;
            char* data;

            void overflow();

          public:
            Stream() {}
            explicit Stream(Pagefile& pagefile_)
              : pagefile(&pagefile_),
                firstPage(Pagefile::noPage),
                currentPage(Pagefile::noPage),
                ppos(0),
                buffersize(minBuffersize),
                data(new char[minBuffersize])
              { }

            ~Stream()
            { delete[] data; }

            void put(char ch)
            {
              if (ppos >= buffersize)
                overflow();
              data[ppos++] = ch;
            }

            void write(const char* data, unsigned size)
            {
              for (unsigned n = 0; n < size; ++n)
                put(data[n]);
            }

            void read(std::string& s);
        };

        typedef std::map<std::string, cxxtools::SmartPtr<Stream> > StreamMap;
        StreamMap streams;
        Pagefile pagefile;

      public:
        explicit MStream(const char* pagefilename)
          : pagefile(pagefilename)
          { }

        typedef StreamMap::const_iterator const_iterator;
        typedef StreamMap::iterator iterator;

        typedef StreamMap::value_type value_type;
        typedef StreamMap::size_type size_type;

        const_iterator begin() const   { return streams.begin(); }
        const_iterator end() const     { return streams.end(); }

        iterator begin()         { return streams.begin(); }
        iterator end()           { return streams.end(); }

        size_type size() const   { return streams.size(); }

        void write(const std::string& streamname, const char* ptr, unsigned size);
        void write(const std::string& streamname, const std::string& str)
          { write(streamname, str.data(), str.size()); }

        iterator find(const std::string& streamname)
          { return streams.find(streamname); }

        void read(const std::string& streamname, std::string& s)
        {
          iterator it = find(streamname);
          if (it != end())
            it->second->read(s);
        }

        std::string read(const std::string& streamname)
        {
          std::string s;
          read(streamname, s);
          return s;
        }

        static void setMinBuffersize(unsigned m)   { minBuffersize = m; }
        static void setMaxBuffersize(unsigned m)   { maxBuffersize = m; }
        static unsigned getMinBuffersize()         { return minBuffersize; }
        static unsigned getMaxBuffersize()         { return maxBuffersize; }
    };

  }
}

#endif //  ZIM_WRITER_MSTREAM_H
