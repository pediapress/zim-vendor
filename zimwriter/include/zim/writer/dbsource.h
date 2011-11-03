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

#ifndef ZIM_WRITER_DBSOURCE_H
#define ZIM_WRITER_DBSOURCE_H

#include <zim/writer/articlesource.h>
#include <tntdb/row.h>
#include <tntdb/connection.h>
#include <tntdb/statement.h>
#include <tntdb/value.h>

namespace zim
{
  namespace writer
  {
    class DbArticle : public Article
    {
        tntdb::Row row;

      public:
        DbArticle()   { }
        explicit DbArticle(tntdb::Row row_)
          : row(row_)
          { }

        virtual std::string getAid() const;
        virtual char getNamespace() const;
        virtual std::string getUrl() const;
        virtual std::string getTitle() const;
        virtual bool isRedirect() const;
        virtual std::string getMimeType() const;
        virtual bool shouldCompress() const;
        virtual std::string getRedirectAid() const;
    };

    class DbSource : public ArticleSource
    {
        std::string dburl;
        tntdb::Connection conn;
        tntdb::Statement stmt;
        tntdb::Statement::const_iterator current;
        bool initialized;
        DbArticle article;
        tntdb::Statement selData;
        tntdb::Blob dataBlob;
        tntdb::Value dataValue;
        unsigned zid;

      public:
        DbSource(int& argc, char* argv[]);

        virtual void setFilename(const std::string& fname);
        virtual const Article* getNextArticle();
        virtual Blob getData(const std::string& aid);
        virtual std::string getMainPage();
        virtual std::string getLayoutPage();
    };

  }
}

#endif // ZIM_WRITER_DBSOURCE_H
