noinst_LTLIBRARIES = libkommanderfactory.la

# set the include path for X, qt and KDE
AM_CPPFLAGS = -I$(top_builddir)/kommander/plugin $(all_includes)

# the library search path. 
libkommanderfactory_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
libkommanderfactory_la_LIBADD = $(top_builddir)/kommander/plugin/libkommanderplugin.la 

# which sources should be compiled for factory
libkommanderfactory_la_SOURCES = kommanderfactory.cpp domtool.cpp
include_HEADERS = kommanderfactory.h
noinst_HEADERS = domtool.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO
