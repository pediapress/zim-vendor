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

#include <zim/writer/filesource.h>
#include <zim/blob.h>
#include <zim/zim.h>
#include <cxxtools/systemerror.h>
#include <cxxtools/log.h>
#include <fstream>
#include <unistd.h>
#include <errno.h>
#include <iterator>

log_define("zim.writer.filesource")

namespace zim
{
  namespace writer
  {
    std::string FileArticle::getAid() const
    {
      return path;
    }

    char FileArticle::getNamespace() const
    {
      return ns;
    }

    std::string FileArticle::getUrl() const
    {
      return fname;
    }

    std::string FileArticle::getTitle() const
    {
      return std::string();
    }

    bool FileArticle::isRedirect() const
    {
      char p[256];
      ssize_t s = ::readlink(path.c_str(), p, sizeof(p));

      if (s < 0 && errno != EINVAL)
        throw cxxtools::SystemError( CXXTOOLS_ERROR_MSG("readlink failed") );

      return s > 0;
    }

    std::string FileArticle::getMimeType() const
    {
      return "text/html";  // TODO
    }

    std::string FileArticle::getRedirectAid() const
    {
      char p[256];
      ssize_t s = ::readlink(path.c_str(), p, sizeof(p));

      if (s < 0)
        throw cxxtools::SystemError( CXXTOOLS_ERROR_MSG("readlink failed") );

      return p;
    }

    ////////////////////////////////////////////////////////////////////

    FileSource::FileSource(const std::string& path)
    {
      cxxtools::Directory dir(path + '/');
      current.push(dir.begin());
      dirName = dir.dirName();
      log_debug("dirName=" << dirName);
      cxxtools::FileInfo fi(current.top().path());
      if (!fi.isFile())
        advance();
    }

    const Article* FileSource::getNextArticle()
    {
      if (current.empty())
        return 0;

      ++current.top();
      return advance();
    }

    const Article* FileSource::advance()
    {
      while (true)
      {
        if (current.top() == cxxtools::DirectoryIterator())
        {
          // last entry reached
          log_debug("last entry reached");
          current.pop();
          if (current.empty())
          {
            log_debug("top level directory finished - end iteration");
            return 0;
          }
          ++current.top();
        }
        else if (*current.top() == "." || *current.top() == "..")
        {
          log_debug("skip " << *current.top());
          ++current.top();
        }
        else
        {
          cxxtools::FileInfo fi(current.top().path());
          if (fi.isDirectory())
          {
            log_debug("directory " << current.top().path());
            current.push(current.top().path());
          }
          else
          {
            std::string f = current.top().path().substr(dirName.size());
            log_debug("return file " << f);
            article = FileArticle('A', dirName + f, f);
            break;
          }
        }
      }

      return &article;
    }

    Blob FileSource::getData(const std::string& aid)
    {
      std::ifstream in(aid.c_str());
      data.clear();
      std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(), std::back_inserter(data));
      log_debug("read data from " << aid << "; " << data.size() << " bytes read");
      return Blob(data.c_str(), data.size());
    }

  }

}
