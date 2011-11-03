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

#include <zim/writer/indexersource.h>
#include <zim/writer/zimindexer.h>
#include <zim/zintstream.h>
#include <cxxtools/arg.h>
#include <zim/file.h>
#include <zim/fileiterator.h>
#include <stdexcept>
#include <iostream>
#include <cxxtools/log.h>

log_define("zim.writer.indexersource")

#define INFO(e) \
    do { \
        log_info(e); \
        std::cout << e << std::endl; \
    } while(false)

namespace zim
{
  namespace writer
  {
    //////////////////////////////////////////////////////////////////////
    // Indexer

    Indexer::Indexer(const char* tmpfilename, const char* trivialWordsFile, unsigned memoryFactor)
      : _trivialWordsFile(trivialWordsFile),
        _mstream(tmpfilename),
        _currentArticle(_mstream)
    {
      MStream::setMinBuffersize(18);
      MStream::setMaxBuffersize(memoryFactor*18);
    }

    void Indexer::createIndex(const char* infile)
    {
      log_trace("create index for file " << infile);

      zim::File zimfile(infile);

      Zimindexer zimindexer(_mstream);

      if (_trivialWordsFile)
      {
        log_debug("read trivial words from " << _trivialWordsFile);
        std::ifstream tw(_trivialWordsFile);
        if (!tw)
          throw std::runtime_error(std::string("cannot open trivial words file ") + _trivialWordsFile);
        std::string word;
        while (tw >> word)
          zimindexer.addTrivialWord(word);
        INFO("ignore " << zimindexer.countTrivialWords() << " trivial words");
      }

      size_type count = 0;
      size_type progress = 0;
      for (zim::File::const_iterator it = zimfile.begin(); it != zimfile.end(); ++it, ++count)
      {
        zim::Article article = *it;

        if (article.isRedirect())
        {
          log_debug("skip redirect \"" << article.getTitle() << "\" id " << article.getIndex());
          continue;
        }

        log_debug("process article \"" << article.getTitle() << "\" id " << article.getIndex() << " mime type " << article.getLibraryMimeType());

        std::string mimeType = article.getMimeType();
        if (mimeType.compare(0, 9, "text/html") != 0
          && mimeType.compare(0, 8, "text/xml") != 0
          && mimeType != MimeHtmlTemplate)
        {
          log_debug("mimetype " << article.getLibraryMimeType() << " not indexed");
          continue;
        }

        if (article.getIndex() == zimfile.getFileheader().getLayoutPage())
        {
          log_debug("layout page \"" << article.getTitle() << "\" not indexed");
          continue;
        }

        zim::Blob data = article.getData();

        zimindexer.process(article.getIndex(), article.getTitle(), data.data(), data.size());

        while (progress < count * 100 / zimfile.getCountArticles() + 1)
        {
          INFO(progress << "% ready");
          progress += 10;
        }
      }

      _currentStream = _mstream.end();
    }

    void Indexer::fetchData(const std::string& aid)
    {
      log_trace("fetch data for aid \"" << aid << '"');
      MStream::iterator it = _mstream.find(aid);
      if (it == _mstream.end())
      {
        log_fatal("internal error: cannot find indexdata for word \"" << aid << '"');
        throw std::runtime_error("internal error: cannot find indexdata for word \"" + aid + '"');
      }

      std::string data;
      it->second->read(data);
      log_debug("data has " << data.size() << " bytes");

      typedef std::vector<IndexEntry> IndexEntriesVector;
      IndexEntriesVector currentData[4];

      for (unsigned off = 0; off < data.size(); off += Zimindexer::Wordentry::size)
      {
        const Zimindexer::Wordentry& w = reinterpret_cast<const Zimindexer::Wordentry&>(*(data.data() + off));
        currentData[w.weight].push_back(IndexEntry(w.aid, w.pos));
      }

      log_debug("create int-compressed data");

      std::ostringstream zdata[4];
      for (unsigned c = 0; c < 4; ++c)
      {
        log_debug("raw data of category " << c << " has " << currentData[c].size() << " entries");

        zim::ZIntStream zdatastream(zdata[c]);
        if (!currentData[c].empty())
        {
          zim::size_type lastidx = 0;
          zim::size_type lastpos = 0;
          for (IndexEntriesVector::const_iterator it = currentData[c].begin() + 1;
            it != currentData[c].end(); ++it)
          {
            zim::size_type idx = it->getIndex() - lastidx;
            zim::size_type pos = it->getPos();
            if (idx == 0)
                pos -= lastpos;  // same article as previous
            else
                lastidx = it->getIndex();  // new article

            lastpos = it->getPos();

            zdatastream.put(idx)
                       .put(pos);
          }
        }
      }

      log_debug("determine flags");

      // write flag
      unsigned flags = 0;
      for (unsigned c = 0, flag = 1; c < 4; ++c, flag <<= 1)
      {
        log_debug("check category " << c);
        if (!currentData[c].empty())
        {
          flags |= flag;
          log_debug("category " << c << " not empty: flags => " << flags);
        }
      }

      std::ostringstream parameter;
      zim::ZIntStream zparameter(parameter);

      log_debug("flags:" << flags);
      zparameter.put(flags);

      // write 1st entries
      for (unsigned c = 0; c < 4; ++c)
      {
        if (!currentData[c].empty())
        {
          log_debug("write zparameter: category:" << c << " count:" << zdata[c].str().size() << " index:" << currentData[c][0].getIndex() << " pos:" << currentData[c][0].getPos());
          zparameter.put(zdata[c].str().size())
                    .put(currentData[c][0].getIndex())
                    .put(currentData[c][0].getPos());
        }
      }

      _currentParameter = parameter.str();

      _currentZData.clear();

      std::string::size_type s = 16;
      for (unsigned c = 0; c < 4; ++c)
        s += zdata[c].str().size();
      _currentZData.reserve(s);

      for (unsigned c = 0; c < 4; ++c)
        _currentZData.append(zdata[c].str());

    }

    const Article* Indexer::getNextArticle()
    {
      log_trace("getNextArticle()");
      if (_currentStream == _mstream.end())
      {
        log_debug("article pointer is at end - start iteration");
        _countArticles = _mstream.size();
        _progress = _count = 0;
        _currentStream = _mstream.begin();
      }
      else
      {
        log_debug("skip to next article");
        ++_currentStream;
      }

      if (_currentStream == _mstream.end())
      {
        log_debug("last article found");
        return 0;
      }

      fetchData(_currentStream->first);
      _currentArticle.setWord(_currentStream->first);
      _currentArticle.setParameter(_currentParameter);

      ++_count;
      while (_progress < _count * 100 / _countArticles + 1)
      {
        INFO(_progress << "% ready");
        _progress += 10;
      }

      return &_currentArticle;
    }

    Blob Indexer::getData(const std::string& aid)
    {
      log_trace("getData(\"" << aid << "\")");
      fetchData(aid);
      return Blob(_currentZData.data(), _currentZData.size());
    }

    //////////////////////////////////////////////////////////////////////
    // IndexArticle

    std::string IndexArticle::getAid() const
    {
      return _word;
    }

    char IndexArticle::getNamespace() const
    {
      return 'X';
    }

    std::string IndexArticle::getUrl() const
    {
      return _word;
    }

    std::string IndexArticle::getTitle() const
    {
      return std::string();
    }

    bool IndexArticle::isRedirect() const
    {
      return false;
    }

    std::string IndexArticle::getMimeType() const
    {
      return "application/x-zim-index-data";
    }

    std::string IndexArticle::getRedirectAid() const
    {
      return std::string();
    }

    std::string IndexArticle::getParameter() const
    {
      return _parameter;
    }

    std::ostream& operator<< (std::ostream& out, const IndexEntry& entry)
    {
      zim::size_type data[2];
      data[0] = fromLittleEndian(&entry.index);
      data[1] = fromLittleEndian(&entry.pos);
      out.write(reinterpret_cast<const char*>(&data[0]), 2 * sizeof(zim::size_type));
      return out;
    }

  }
}
