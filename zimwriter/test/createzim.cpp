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

#include <iostream>
#include <zim/writer/articlesource.h>
#include <zim/writer/zimcreator.h>
#include <zim/blob.h>
#include <cxxtools/log.h>

class Article : public zim::writer::Article
{
  public:
    virtual std::string getAid() const;
    virtual char getNamespace() const;
    virtual std::string getUrl() const;
    virtual std::string getTitle() const;
    virtual bool isRedirect() const;
    virtual std::string getMimeType() const;
    virtual std::string getRedirectAid() const;
};

std::string Article::getAid() const
{
  return std::string();
}

char Article::getNamespace() const
{
  return 'A';
}

std::string Article::getUrl() const
{
  return "foo";
}

std::string Article::getTitle() const
{
  return "foo";
}

bool Article::isRedirect() const
{
  return false;
}

std::string Article::getMimeType() const
{
  return "text/html";
}

std::string Article::getRedirectAid() const
{
  return std::string();
}

class ArticleSource : public zim::writer::ArticleSource
{
    bool first;
    Article article;

  public:
    ArticleSource()
      : first(true)
      { }

    virtual const Article* getNextArticle();
    virtual zim::Blob getData(const std::string& aid);
};

const Article* ArticleSource::getNextArticle()
{
  if (first)
  {
    first = false;
    return &article;
  }
  return 0;
}

zim::Blob ArticleSource::getData(const std::string& aid)
{
  static char content[] = "Hello";
  return zim::Blob(content, sizeof(content));
}

int main(int argc, char* argv[])
{
  try
  {
    log_init();
    zim::writer::ZimCreator creator(argc, argv);
    ArticleSource source;
    creator.create("foo.zim", source);
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

