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
#include <sstream>
#include <zim/writer/articlesource.h>
#include <zim/writer/zimcreator.h>
#include <zim/blob.h>
#include <cxxtools/log.h>

struct A
{
  char ns;
  const char* title;
  const char* mime;  // 0 for redirect
  const char* content;  // or redirect target
} a[] = {
  { 'A', "Auto", "text/html", "<h1>Auto</h1>" },
  { 'B', "Auto", "text/plain", "Auto" },
  { 'A', "Automobile", 0, "0" },
  { '\0', 0, 0, 0 }
};

class Article : public zim::writer::Article
{
    unsigned n;

  public:
    explicit Article(unsigned n_ = 0)
      : n(n_)
      { }

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
  std::ostringstream s;
  s << n;
  return s.str();
}

char Article::getNamespace() const
{
  return a[n].ns;
}

std::string Article::getUrl() const
{
  return a[n].title;
}

std::string Article::getTitle() const
{
  return a[n].title;
}

bool Article::isRedirect() const
{
  return a[n].mime == 0;
}

std::string Article::getMimeType() const
{
  return a[n].mime;
}

std::string Article::getRedirectAid() const
{
  return a[n].content;
}

class ArticleSource : public zim::writer::ArticleSource
{
    Article article;
    unsigned n;

  public:
    ArticleSource()
      : n(0)
      { }

    virtual const Article* getNextArticle();
    virtual zim::Blob getData(const std::string& aid);
};

const Article* ArticleSource::getNextArticle()
{
  if (a[n].ns)
  {
    article = Article(n++);
    return &article;
  }
  return 0;
}

zim::Blob ArticleSource::getData(const std::string& aid)
{
  static char content[] = "Hello";
  std::istringstream s(aid);
  unsigned n;
  s >> n;
  const char* d = a[n].content;
  return zim::Blob(d, strlen(d));
}

int main(int argc, char* argv[])
{
  try
  {
    log_init();
    zim::writer::ZimCreator creator(argc, argv);
    ArticleSource source;
    creator.create("foo-t.zim", source);
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

