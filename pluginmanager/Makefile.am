bin_PROGRAMS = kmdr-plugins

kmdr_plugins_SOURCES = main.cpp mainwindow.cpp pluginmanager.cpp
kmdr_plugins_LDADD   = $(top_builddir)/kommander/factory/libkommanderfactory.la  $(LIB_KIO)

#KDE_ICON = kmdr-plugins

appsdir = $(kde_appsdir)/Applications
#apps_DATA = kmdr-plugins.desktop

INCLUDES = -I$(top_srcdir)/kommander/factory $(all_includes)

METASOURCES = AUTO

# the library search path.
kmdr_plugins_LDFLAGS = $(all_libraries) $(KDE_RPATH)

noinst_HEADERS = mainwindow.h pluginmanager.h
