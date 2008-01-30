bin_PROGRAMS = kmdr-executor

noinst_LTLIBRARIES = libinstance.la

libinstance_la_SOURCES = instance.cpp  dcopkommanderif.skel
libinstance_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -no-undefined


kmdr_executor_SOURCES = main.cpp
kmdr_executor_LDADD   = libinstance.la $(top_builddir)/kommander/widget/libkommanderwidget.la  $(top_builddir)/kommander/widgets/libkommanderwidgets.la $(top_builddir)/kommander/factory/libkommanderfactory.la  $(LIB_KIO)

#KDE_ICON = kmdr_executor

appsdir = $(kde_appsdir)/.hidden
apps_DATA = kmdr-executor.desktop

AM_CPPFLAGS = -I$(top_srcdir)/kommander/plugin -I$(top_srcdir)/kommander/widgets -I$(top_srcdir)/kommander/widget -I$(top_srcdir)/kommander/factory $(all_includes)


METASOURCES = AUTO

# the library search path.
kmdr_executor_LDFLAGS = $(all_libraries) $(KDE_RPATH)

noinst_HEADERS =  dcopkommanderif.h
