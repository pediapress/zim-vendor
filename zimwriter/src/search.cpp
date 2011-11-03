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

#include <zim/writer/search.h>
#include <cxxtools/convert.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

log_define("zim.writer.search")

namespace zim
{
namespace writer
{
  std::string SearchArticle::getAid() const
  {
    std::string aid;
    cxxtools::convert(aid, article.getIndex());
    return aid;
  }

  char SearchArticle::getNamespace() const
  {
    return article.getNamespace();
  }

  std::string SearchArticle::getUrl() const
  {
    return article.getUrl();
  }

  std::string SearchArticle::getTitle() const
  {
    return article.getTitle();
  }

  bool SearchArticle::isRedirect() const
  {
    return article.isRedirect();
  }

  std::string SearchArticle::getMimeType() const
  {
    return article.getMimeType();
  }

  std::string SearchArticle::getRedirectAid() const
  {
    std::string aid;
    cxxtools::convert(aid, article.getRedirectIndex());
    return aid;
  }

  SearchSource::SearchSource(const std::string& searchWords, int& argc, char* argv[])
  {
    cxxtools::Arg<std::string> inputFileName(argc, argv, 'I');
    articleFile = zim::File(inputFileName);

    cxxtools::Arg<std::string> indexFileName(argc, argv, 'X');

    log_info("execute search <" << searchWords << '>');

    if (indexFileName.isSet())
    {
      zim::File indexFile = zim::File(indexFileName);
      zim::Search search(articleFile, indexFile);
      search.search(results, searchWords);
    }
    else
    {
      zim::Search search(articleFile);
      search.search(results, searchWords);
    }

    log_info(results.size() << " articles found");

    nextArticle = results.begin();
  }

  const Article* SearchSource::getNextArticle()
  {
    if (nextArticle == results.end())
    {
      if (nextRedirect == articleFile.end())
      {
        log_info("no further redirects found");
        return 0;
      }

      log_debug("redirect found: " << nextRedirect->getIndex() << ": " << nextRedirect->getTitle());
      article.setArticle(*nextRedirect);

      for (++nextRedirect ; nextRedirect != articleFile.end(); ++nextRedirect)
      {
        if (nextRedirect->isRedirect() && aids.find(nextRedirect->getRedirectIndex()) != aids.end())
        {
          break;
        }
      }
    }
    else
    {
      log_debug("article found: " << nextRedirect->getIndex() << ": " << nextRedirect->getTitle());
      article.setArticle(nextArticle->getArticle());
      aids.insert(nextArticle->getArticle().getIndex());

      ++nextArticle;

      if (nextArticle == results.end())
      {
        // search for first redirect

        log_info("search for redirects");

        for (nextRedirect = articleFile.begin(); nextRedirect != articleFile.end(); ++nextRedirect)
          if (nextRedirect->isRedirect() && aids.find(nextRedirect->getRedirectIndex()) != aids.end())
            break;
      }
    }

    return &article;
  }

  Blob SearchSource::getData(const std::string& aid)
  {
    zim::size_type idx;
    cxxtools::convert(idx, aid);
    return articleFile.getArticle(idx).getData();
  }

}
}
