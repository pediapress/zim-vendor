INCLUDES = -I$(top_srcdir)/include

sources = bindutils.cpp bindvalues.cpp boundrow.cpp boundvalue.cpp connection.cpp connectionmanager.cpp cursor.cpp error.cpp resultrow.cpp rowcontainer.cpp rowvalue.cpp statement.cpp result.cpp

AM_CPPFLAGS = @MYSQL_CFLAGS@
AM_CXXFLAGS = $(PTHREAD_CFLAGS)

if MAKE_MYSQL

driver_LTLIBRARIES = tntdb@abi_current@-mysql.la

tntdb@abi_current@_mysql_la_SOURCES = $(sources)
tntdb@abi_current@_mysql_la_LDFLAGS = -module -version-info @sonumber@ $(PTHREAD_LIBS) @MYSQL_LDFLAGS@
tntdb@abi_current@_mysql_la_LIBADD = $(top_srcdir)/src/libtntdb.la

endif
