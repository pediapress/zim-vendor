/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef CXXTOOLS_STDIN_H
#define CXXTOOLS_STDIN_H

#include <iostream>
#include <iostream>
#include <fstream>
#include <cxxtools/arg.h>

namespace cxxtools
{
    /**
     * Helper class for redirecting input to stdin or file using command line switch.
     *
     * Using this class it is easy to provide a command line switch to the user
     * to read input from a file or stdin.
     *
     * Examples:
     * \code
     * int main(int argc, char* argv[])
     * {
     *   cxxtools::ArgIn in(argc, argv, 'i');
     *   double d;
     *   while (in >> d)
     *   {
     *     std::cout << d*2 << std::endl;
     *   }
     * }
     * \endcode
     *
     * This program reads data from stdin or a file, when provided using the
     * -i option and doubles the value
     *
     * \code
     * int main(int argc, char* argv[])
     * {
     *   cxxtools::ArgIn in(argc, argv);
     *   double d;
     *   while (in >> d)
     *   {
     *     std::cout << d*2 << std::endl;
     *   }
     * }
     * \endcode
     *
     * This program reads data from stdin or a file, when provided as a
     * parameter and doubles the value. This is similar to the <> operator
     * in perl.
     *
     */
    class ArgIn : public std::istream
    {
            std::ifstream _ifile;

            void doInit(const Arg<const char*>& ifile)
            {
                std::streambuf* buf;
                if (ifile.isSet())
                {
                    _ifile.open(ifile);
                    buf = _ifile.rdbuf();
                }
                else
                    buf = std::cin.rdbuf();
                init(buf);
            }

        public:
            /// Constructor processes short options like: -i something.
            ArgIn(int& argc, char* argv[], char option)
                : std::istream(0)
            {
                Arg<const char*> ifile(argc, argv, option);
                doInit(ifile);
            }

            /// Constructor processes long options like: --input something".
            ArgIn(int& argc, char* argv[], const char* option)
                : std::istream(0)
            {
                Arg<const char*> ifile(argc, argv, option);
                doInit(ifile);
            }

            /// Constructor processes normal parameters.
            ArgIn(int& argc, char* argv[])
                : std::istream(0)
            {
                Arg<const char*> ifile(argc, argv);
                doInit(ifile);
            }

            /// returns true, if the input is read from a file.
            bool redirected() const
            {
                return rdbuf() != std::cin.rdbuf();
            }
    };
}

#endif // CXXTOOLS_STDIN_H
