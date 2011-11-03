#! /bin/sh
# Regenerate the files autoconf / automake

libtoolize --copy --force --automake

rm -f config.cache
rm -f config.log
aclocal -I m4
autoheader
autoconf
automake --copy -a
