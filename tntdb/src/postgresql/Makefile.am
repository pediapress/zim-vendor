INCLUDES = -I$(top_srcdir)/include

sources = connection.cpp connectionmanager.cpp cursor.cpp error.cpp result.cpp resultrow.cpp resultvalue.cpp statement.cpp

AM_CPPFLAGS = @PG_CXXFLAGS@
AM_CXXFLAGS = $(PTHREAD_CFLAGS)

if MAKE_POSTGRESQL

driver_LTLIBRARIES = tntdb@abi_current@-postgresql.la

tntdb@abi_current@_postgresql_la_SOURCES = $(sources)
tntdb@abi_current@_postgresql_la_LDFLAGS = -module -version-info @sonumber@ $(PTHREAD_LIBS) @PG_LDFLAGS@
tntdb@abi_current@_postgresql_la_LIBADD = $(top_srcdir)/src/libtntdb.la

endif
