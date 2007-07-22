### Kommander Factory ###
MESSAGE(STATUS, ${CMAKE_CURRENT_SOURCE_DIR}/kommander/widget)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/../widget
 ${CMAKE_CURRENT_SOURCE_DIR}/../plugin
 ${CMAKE_CURRENT_BINARY_DIR}
 ${KDE4_INCLUDE_DIR} 
 ${QT_INCLUDE_DIR} )


########### next target ###############

SET(kommanderfactory_STAT_SRCS
   kommanderfactory.cpp
   domtool.cpp
   )

ADD_LIBRARY(kommanderfactory STATIC ${kommanderfactory_STAT_SRCS})


########### install files ###############

INSTALL(FILES kommanderfactory.h DESTINATION ${INCLUDE_INSTALL_DIR})


#original Makefile.am contents follow:

#noinst_LTLIBRARIES = libkommanderfactory.la
#
## set the include path for X, qt and KDE
#AM_CPPFLAGS = -I$(top_srcdir)/kommander/widget -I$(top_srcdir)/kommander/plugin $(all_includes)
#
## the library search path. 
#libkommanderfactory_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -no-undefined
#libkommanderfactory_la_LIBADD = $(top_builddir)/kommander/plugin/libkommanderplugin.la 
#
## which sources should be compiled for factory
#libkommanderfactory_la_SOURCES = kommanderfactory.cpp domtool.cpp
#include_HEADERS = kommanderfactory.h
#noinst_HEADERS = domtool.h
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
