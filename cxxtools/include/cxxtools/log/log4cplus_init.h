/*
 * Copyright (C) 2004 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef CXXTOOLS_LOG_LOG4CPLUS_INIT_H
#define CXXTOOLS_LOG_LOG4CPLUS_INIT_H

#include <log4cplus/configurator.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <sys/stat.h>

#define log_init_fatal()   log_init(log4cplus::FATAL_LOG_LEVEL)
#define log_init_error()   log_init(log4cplus::ERROR_LOG_LEVEL)
#define log_init_warn()    log_init(log4cplus::WARN_LOG_LEVEL)
#define log_init_info()    log_init(log4cplus::INFO_LOG_LEVEL)
#define log_init_debug()   log_init(log4cplus::DEBUG_LOG_LEVEL)
#define log_init_trace()   log_init(log4cplus::TRACE_LOG_LEVEL)

inline void log_init(const std::string& propertyfilename)
{
  log4cplus::PropertyConfigurator logconfig(propertyfilename);
  logconfig.configure();
}

inline void log_init(log4cplus::LogLevel level)
{
  log4cplus::SharedAppenderPtr appender(new log4cplus::ConsoleAppender(true, true));
  appender->setName("Main");
  appender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::TTCCLayout()));

  log4cplus::Logger root = log4cplus::Logger::getRoot();
  root.addAppender(appender);
  root.setLogLevel(level);
}

inline void log_init()
{
  char* LOGPROPERTIES = ::getenv("LOGPROPERTIES");
  if (LOGPROPERTIES)
    log_init(LOGPROPERTIES);
  else
  {
    struct stat s;
    if (stat("log4cplus.properties", &s) == 0)
      log_init("log4cplus.properties");
    else
      log_init(log4cplus::ERROR_LOG_LEVEL);
  }
}

#endif // CXXTOOLS_LOG_LOG4CPLUS_INIT_H
