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

#include <zim/writer/articlesource.h>
#include <zim/writer/mstream.h>
#include <iosfwd>

namespace zim
{
  namespace writer
  {
    class IndexArticle : public Article
    {
        MStream& _mstream;
        std::string _word;
        std::string _parameter;

      public:
        explicit IndexArticle(MStream& mstream)
          : _mstream(mstream)
            { }

        std::string getAid() const;
        char getNamespace() const;
        std::string getUrl() const;
        std::string getTitle() const;
        bool isRedirect() const;
        std::string getMimeType() const;
        std::string getRedirectAid() const;
        std::string getParameter() const;

        void setWord(const std::string& word)            { _word = word; }
        void setParameter(const std::string& parameter)  { _parameter = parameter; }
    };

    class IndexEntry
    {
        friend std::ostream& operator<< (std::ostream& out, const IndexEntry& entry);
        zim::size_type index;
        zim::size_type pos;

      public:
        IndexEntry() {}
        IndexEntry(zim::size_type index_, zim::size_type pos_)
          : index(index_),
            pos(pos_)
        { }

        zim::size_type getIndex() const        { return index; }
        void        setIndex(zim::size_type o) { index = o; }

        zim::size_type getPos() const        { return pos; }
        void        setPos(zim::size_type o) { pos = o; }
    };

    std::ostream& operator<< (std::ostream& out, const IndexEntry& entry);

    class Indexer : public ArticleSource
    {
        const char* _trivialWordsFile;
        MStream _mstream;
        IndexArticle _currentArticle;
        MStream::iterator _currentStream;
        std::string _currentZData;
        std::string _currentParameter;

        MStream::size_type _countArticles;
        MStream::size_type _count;
        MStream::size_type _progress;

        void transformData(const char* srcdata, unsigned srcsize);
        void fetchData(const std::string& aid);

      public:
        Indexer(const char* tmpfilename, const char* trivialWordsFile, unsigned memoryFactor);

        void createIndex(const char* infile);

        const Article* getNextArticle();
        Blob getData(const std::string& aid);
    };

  }
}
