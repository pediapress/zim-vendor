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

#include "zim/writer/mstream.h"
#include <stdexcept>
#include <algorithm>
#include <cxxtools/log.h>
#include <string.h>
#include <stdio.h>  // remove
#include <errno.h>

log_define("zim.writer.mstream")

namespace zim
{
  namespace writer
  {
    ////////////////////////////////////////////////////////////////////
    // Pagefile

    Pagefile::~Pagefile()
    {
      int r = ::remove(backfilename.c_str());
      if (r != 0)
        log_error("error " << errno << " removing temporary file \"" << backfilename << '"');
    }

    Pagefile::PageNumber Pagefile::getNewPage()
    {
      PageNumber newPage = nextPage;
      ++nextPage;
      return newPage;
    }

    void Pagefile::getPage(PageNumber num, Page& page)
    {
      log_debug("get page " << num);

      backfile.seekg(static_cast<std::streampos>(num) * PageSize, std::ios::beg);
      backfile.read(reinterpret_cast<char*>(&page), sizeof(Page));

      if (backfile.fail())
        throw std::runtime_error("file operation failed");
    }

    void Pagefile::putPage(PageNumber num, const Page& page, unsigned size)
    {
      log_debug("put page " << num << " (size=" << size << ", ppos=" << page.ppos << ')');

      backfile.seekp(static_cast<std::streampos>(num) * PageSize, std::ios::beg);
      backfile.write(reinterpret_cast<const char*>(&page), size);

      if (backfile.fail())
        throw std::runtime_error("file operation failed");
    }

    const Pagefile::PageNumber Pagefile::noPage;
    const unsigned Pagefile::Page::dataSize;
    unsigned MStream::minBuffersize = 18;
    unsigned MStream::maxBuffersize = 64*18;

    void MStream::Stream::overflow()
    {
      log_trace("overflow");

      if (!pagefile)
        throw std::logic_error("no pagefile");

      if (ppos == 0)
        return;

      if (buffersize < maxBuffersize)
      {
        char* newdata = new char[buffersize * 2];
        memcpy(newdata, data, buffersize);
        buffersize *= 2;
        delete[] data;
        data = newdata;
        return;
      }

      Pagefile::Page page;

      uint16_t off = 0;

      if (currentPage != Pagefile::noPage)
      {
        log_debug("read current page " << currentPage);

        // read current page
        //
        pagefile->getPage(currentPage, page);

        log_debug("page.nextPage=" << page.nextPage << " page.ppos=" << page.ppos << " page.free=" << page.free());

        if (page.free())
        {
          // write as much data as possible
          size_t count = std::min(ppos, page.free());
          log_debug("current page free: " << page.free() << " write " << count << " bytes");

          memcpy(page.data + page.ppos, data, count);
          page.ppos += count;

          memmove(data, data + count, ppos - count);
          ppos -= count;

          log_debug("ppos=" << ppos);

          //off += count;

          // write current page back to disk
          pagefile->putPage(currentPage, page, sizeof(page) - page.free());

          return;
        } 

        log_debug("no page left on current page");
      }

      if (off < ppos)
      {
        // we have data left to write - allocate new page from pagefile
        Pagefile::PageNumber newPage = pagefile->getNewPage();
        log_debug("new page " << newPage);
        page.nextPage = Pagefile::noPage;
        memcpy(page.data, data + off, ppos - off);
        page.ppos = ppos - off;
        pagefile->putPage(newPage, page);

        if (firstPage == Pagefile::noPage)
        {
          log_debug("firstPage=" << newPage);
          firstPage = newPage;
        }
        else
        {
          log_debug("link " << currentPage << " with " << newPage);
          page.nextPage = newPage;
          pagefile->putPage(currentPage, page, sizeof(Pagefile::PageNumber));
        }

        log_debug("currentPage=" << newPage);
        currentPage = newPage;
        ppos = 0;
      }
    }

    void MStream::Stream::read(std::string& s)
    {
      log_trace("read");
      log_debug("ppos=" << ppos);

      s.clear();

      Pagefile::PageNumber p = firstPage;
      while (p != Pagefile::noPage)
      {
        Pagefile::Page page;
        pagefile->getPage(p, page);
        log_debug("page " << p << " " << page.ppos << " bytes");
        s.append(page.data, page.ppos);
        p = page.nextPage;
      }

      if (data)
      {
        log_debug("append " << ppos << " bytes");
        s.append(data, ppos);
      }
    }

    //////////////////////////////////////////////////////////////////////
    // MStream
    //
    void MStream::write(const std::string& streamname, const char* ptr, unsigned size)
    {
      log_debug("write " << size << " bytes to stream \"" << streamname << '"');
      StreamMap::iterator it = streams.find(streamname);
      if (it == streams.end())
        it = streams.insert(StreamMap::value_type(streamname, new Stream(pagefile))).first;
      it->second->write(ptr, size);
    }
  }
}
