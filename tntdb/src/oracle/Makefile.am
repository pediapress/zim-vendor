INCLUDES = -I$(top_srcdir)/include

sources = \
	blob.cpp \
	connection.cpp \
	connectionmanager.cpp \
	cursor.cpp \
	datetime.cpp \
	error.cpp \
	result.cpp \
	row.cpp \
	statement.cpp \
	value.cpp \
	number.cpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)

if MAKE_ORACLE

driver_LTLIBRARIES = tntdb@abi_current@-oracle.la

tntdb@abi_current@_oracle_la_SOURCES = $(sources)
tntdb@abi_current@_oracle_la_LDFLAGS = -module -version-info @sonumber@ $(PTHREAD_LIBS)
tntdb@abi_current@_oracle_la_LIBADD = $(top_srcdir)/src/libtntdb.la -lclntsh

endif
