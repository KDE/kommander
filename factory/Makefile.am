noinst_LTLIBRARIES = libkommanderfactory.la

# set the include path for X, qt and KDE
AM_CPPFLAGS = -I$(top_srcdir)/widget -I$(top_srcdir)/plugin $(all_includes)

# the library search path. 
libkommanderfactory_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -no-undefined
libkommanderfactory_la_LIBADD = $(top_builddir)/plugin/libkommanderplugin.la 

# which sources should be compiled for factory
libkommanderfactory_la_SOURCES = kommanderfactory.cpp domtool.cpp
include_HEADERS = kommanderfactory.h
noinst_HEADERS = domtool.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO
