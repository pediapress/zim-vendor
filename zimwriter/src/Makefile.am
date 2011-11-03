lib_LTLIBRARIES = libzimwriter.la
bin_PROGRAMS = zimwriterdb zimindexer zimcreatorsearch wikizim

AM_CPPFLAGS=-I$(top_builddir)/include

# libzimwriter
#
libzimwriter_la_SOURCES = \
    articlesource.cpp \
    zimcreator.cpp
libzimwriter_la_LDFLAGS = -lcxxtools -lzim

# wikizim
#
wikizim_SOURCES = \
	wikizim.cpp
wikizim_LDFLAGS = -lcxxtools -lcxxtools-http -lcxxtools-xml -lzim
wikizim_LDADD = $(top_builddir)/src/libzimwriter.la

# zimwriterdb
#
zimwriterdb_SOURCES = \
    dbsource.cpp \
    filesource.cpp \
    zimwriterdb.cpp

zimwriterdb_LDADD = $(top_builddir)/src/libzimwriter.la
zimwriterdb_LDFLAGS = -lcxxtools -lzim -ltntdb

# zimindexer
#
zimindexer_SOURCES = \
    articleparser.cpp \
    indexersource.cpp \
    mstream.cpp \
    zimindexer.cpp
zimindexer_LDADD = $(top_builddir)/src/libzimwriter.la

# zimcreataorsearch
#
zimcreatorsearch_SOURCES = \
    zimcreatorsearch.cpp \
    search.cpp
zimcreatorsearch_LDADD = $(top_builddir)/src/libzimwriter.la
