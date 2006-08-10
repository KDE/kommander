bin_PROGRAMS = kmdr-executor

kmdr_executor_SOURCES = instance.cpp main.cpp dcopkommanderif.skel
kmdr_executor_LDADD   = $(top_builddir)/widget/libkommanderwidget.la $(top_builddir)/widgets/libkommanderwidgets.la $(top_builddir)/factory/libkommanderfactory.la  $(LIB_KIO)

#KDE_ICON = kmdr_executor

appsdir = $(kde_appsdir)/.hidden
apps_DATA = kmdr-executor.desktop

AM_CPPFLAGS = -I$(top_srcdir)/widgets -I$(top_srcdir)/widget -I$(top_srcdir)/factory $(all_includes)


METASOURCES = AUTO

# the library search path.
kmdr_executor_LDFLAGS = $(all_libraries) $(KDE_RPATH)

noinst_HEADERS =  dcopkommanderif.h
