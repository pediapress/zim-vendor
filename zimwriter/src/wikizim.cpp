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
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/net/uri.h>
#include <cxxtools/xml/xmlreader.h>
#include <cxxtools/xml/startelement.h>
#include <cxxtools/xml/endelement.h>
#include <cxxtools/http/client.h>
#include <cxxtools/http/request.h>
#include <cxxtools/query_params.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/clock.h>
#include <zim/writer/articlesource.h>
#include <zim/writer/zimcreator.h>
#include <zim/blob.h>
#include "config.h"

log_define("zim.writer.wiki")

class WikiArticle : public zim::writer::Article
{
    friend class WikiSource;

    char ns;
    std::string aid;
    std::string title;
    std::string redirectAid;

  public:
    virtual std::string getAid() const;
    virtual char getNamespace() const;
    virtual std::string getUrl() const;
    virtual std::string getTitle() const;
    virtual zim::size_type getVersion() const;
    virtual bool isRedirect() const;
    virtual std::string getMimeType() const;
    virtual std::string getRedirectAid() const;
};

std::string WikiArticle::getAid() const
{
  return title;
}

char WikiArticle::getNamespace() const
{
  return 'A';
}

std::string WikiArticle::getUrl() const
{
  return title;
}

std::string WikiArticle::getTitle() const
{
  return title;
}

zim::size_type WikiArticle::getVersion() const
{
  return 0;
}

bool WikiArticle::isRedirect() const
{
  return !redirectAid.empty();
}

std::string WikiArticle::getMimeType() const
{
  return "text/html";
}

std::string WikiArticle::getRedirectAid() const
{
  return redirectAid;
}

class WikiSource : public zim::writer::ArticleSource
{
    // parameters to fetch
    cxxtools::http::Client client;
    std::string url;
    std::string userAgent;

    // xml parser stuff
    std::istringstream xmlStream;
    cxxtools::xml::XmlReader xmlReader;
    cxxtools::xml::XmlReader::Iterator xmlIterator;

    // current article
    WikiArticle article;
    bool readingRedirects;

    // body of the last article fetched
    std::string body;

    // statistics
    unsigned long bytesRead;

    void queryPagesInfo(const std::string& apfrom);
    std::string getRedirectAid(const std::string& aid);

  public:
    WikiSource(const std::string& host, unsigned short int port, const std::string& url_)
      : client(host, port),
        url(url_),
        xmlReader(xmlStream),
        readingRedirects(false),
        bytesRead(0)
    {
      if (url.empty() || url[url.size()-1] != '/')
        url += '/';
    }

    virtual const zim::writer::Article* getNextArticle();
    virtual zim::Blob getData(const std::string& aid);

    void setUserAgent(const std::string& ua)
        { userAgent = ua; }

    unsigned long getBytesRead() const
        { return bytesRead; }
};

void WikiSource::queryPagesInfo(const std::string& apfrom)
{
  log_info("read pages starting from \"" << apfrom << "\" redirect=" << readingRedirects);
  cxxtools::QueryParams q;
  q.add("action", "query")
   .add("list", "allpages")
   .add("format", "xml")
   .add("aplimit", "500")
   .add("apfilterredir", readingRedirects ? "redirects" : "nonredirects");
  if (!apfrom.empty())
   q.add("apfrom", apfrom);

  log_debug("request "<< url << "api.php?" << q.getUrl());
  cxxtools::http::Request request(url + "api.php?" + q.getUrl());
  request.setHeader("User-Agent", userAgent.c_str());

  client.execute(request);
  std::string body = client.readBody();

  bytesRead += body.size();

  xmlStream.str(body);
  xmlStream.seekg(0);
  xmlReader.reset(xmlStream);
  xmlIterator = xmlReader.current();
}

std::string WikiSource::getRedirectAid(const std::string& aid)
{
  cxxtools::QueryParams q;
  q.add("action", "query")
   .add("format", "xml")
   .add("titles", aid)
   .add("redirects");

  log_debug("request redirect aid "<< url << "api.php?" << q.getUrl());
  cxxtools::http::Request request(url + "api.php?" + q.getUrl());
  request.setHeader("User-Agent", userAgent.c_str());

  client.execute(request);
  std::string body = client.readBody();

  std::istringstream xmlStream(body);
  cxxtools::xml::XmlReader xmlReader(xmlStream);
  cxxtools::xml::XmlReader::Iterator xmlIterator = xmlReader.current();

  while ((++xmlIterator)->type() != cxxtools::xml::Node::EndDocument)
  {
    if (xmlIterator->type() == cxxtools::xml::Node::StartElement)
    {
      const cxxtools::xml::StartElement& startElement = dynamic_cast<const cxxtools::xml::StartElement&>(*xmlIterator);
      if (startElement.name() == L"r")
      {
        std::string to = cxxtools::Utf8Codec::encode(startElement.attribute(L"to"));
        log_debug("redirect article <" << aid << "> is <" << to << '>');
        return to;
      }
    }
  }

  log_warn("redirect for article <" << aid << "> not found");
  return std::string();
}

const zim::writer::Article* WikiSource::getNextArticle()
{
  if (xmlIterator == xmlReader.end())
  {
    queryPagesInfo(std::string());
  }

  while (true)
  {
    ++xmlIterator;

    if (xmlIterator->type() == cxxtools::xml::Node::EndDocument)
    {
      if (readingRedirects)
      {
        log_debug("reading redirects finished");
        break;
      }
      readingRedirects = true;
      log_debug("read redirects");
      queryPagesInfo(std::string());
    }
    else if (xmlIterator->type() == cxxtools::xml::Node::StartElement)
    {
      const cxxtools::xml::StartElement& startElement = dynamic_cast<const cxxtools::xml::StartElement&>(*xmlIterator);
      if (startElement.name() == L"p")
      {
        // page found - fill attributes and return article

        // get attribute pageid => article.aid
        // get attribute ns => article.ns
        // get attribute title => article.title
        article.aid   = cxxtools::Utf8Codec::encode(startElement.attribute(L"pageid"));
        article.ns    = cxxtools::Utf8Codec::encode(startElement.attribute(L"ns"))[0];
        article.title = cxxtools::Utf8Codec::encode(startElement.attribute(L"title"));
        if (readingRedirects)
          article.redirectAid = getRedirectAid(article.title);
        else
          article.redirectAid.clear();

        log_debug("title=\"" << article.title << '"');

        return &article;
      }
      else if (startElement.name() == L"allpages")
      {
        // get attribute apfrom
        cxxtools::String s = startElement.attribute(L"apfrom");
        if (!s.empty())
        {
          cxxtools::String apfrom = startElement.attribute(L"apfrom");
          log_debug("apfrom=" << apfrom.narrow());
          queryPagesInfo(cxxtools::Utf8Codec::encode(apfrom));
        }
      }
    }
  }

  return 0;
}

zim::Blob WikiSource::getData(const std::string& aid)
{
  log_debug("fetch data for aid " << aid);

  cxxtools::QueryParams q;
  q.add("action", "render")
   .add("title", aid);

  cxxtools::http::Request request(url + "index.php?" + q.getUrl());
  request.setHeader("User-Agent", userAgent.c_str());

  client.execute(request);
  body = client.readBody();

  bytesRead += body.size();
  return zim::Blob(body.data(), body.size());
}

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    if (argc != 3)
    {
      std::cout << "usage: " << argv[0] << " [options] wiki-url output-filename\n"
                   "example: " << argv[0] << " http://openzim.org/ openzim-org\n"
                   "            generates openzim-org.zim with the content of the openzim.org wiki\n"
                   "options:\n"
                   "\t-s <number>        specify chunk size for compression in kB (default 1024)\n"
                   "\t--user-agent <ua>  set the user agent used for downloading (default \"wikizim " PACKAGE_VERSION "\")\n";
      return 1;
    }

    cxxtools::net::Uri uri = cxxtools::net::Uri(cxxtools::Arg<std::string>(argc, argv));
    cxxtools::Arg<std::string> userAgent(argc, argv, "--user-agent", "wikizim " PACKAGE_VERSION);

    std::cout << "host: " << uri.host() << "\n"
                 "port: " << uri.port() << "\n"
                 "url: " << uri.path() << std::endl;

    WikiSource source(uri.host(), uri.port(), uri.path());
    zim::writer::ZimCreator creator(argc, argv);

    std::string fname = cxxtools::Arg<std::string>(argc, argv);
    source.setFilename(fname);

    cxxtools::Clock clock;
    clock.start();

    creator.create(fname, source);

    cxxtools::Timespan t = clock.stop();

    std::cout << source.getBytesRead() << " bytes in " << t.totalMSecs() / 1000.0 << " seconds; " << (source.getBytesRead() * 1000.0 / t.totalMSecs()) << " bytes per second" << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

