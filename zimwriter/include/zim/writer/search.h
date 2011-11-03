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

#ifndef ZIM_WRITER_SEARCH_H
#define ZIM_WRITER_SEARCH_H

#include <zim/writer/articlesource.h>
#include <zim/file.h>
#include <zim/fileiterator.h>
#include <zim/search.h>
#include <set>

namespace zim
{
  namespace writer
  {
    class SearchArticle : public Article
    {
        zim::Article article;

      public:
        void setArticle(const zim::Article& a)
            { article = a; }

        virtual std::string getAid() const;
        virtual char getNamespace() const;
        virtual std::string getUrl() const;
        virtual std::string getTitle() const;
        virtual bool isRedirect() const;
        virtual std::string getMimeType() const;
        virtual std::string getRedirectAid() const;
    };

    class SearchSource : public ArticleSource
    {
        zim::File articleFile;
        zim::Search::Results results;
        zim::Search::Results::const_iterator nextArticle;
        SearchArticle article;

        typedef std::set<zim::size_type> Aids;
        Aids aids;
        zim::File::const_iterator nextRedirect;

      public:
        SearchSource(const std::string& searchWords, int& argc, char* argv[]);

        virtual const Article* getNextArticle();
        virtual Blob getData(const std::string& aid);
    };

  }
}

#endif // ZIM_WRITER_SEARCH_H
