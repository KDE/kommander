bin_PROGRAMS = kmdr-executor

kmdr_executor_SOURCES = instance.cpp main.cpp dcopkommanderif.skel
kmdr_executor_LDADD   = $(top_builddir)/kommander/widget/libkommanderwidget.la $(top_builddir)/kommander/widgets/libkommanderwidgets.la $(top_builddir)/kommander/factory/libkommanderfactory.la  $(LIB_KIO)

KDE_ICON = kmdr-executor

appsdir = $(kde_appsdir)/.hidden
apps_DATA = kmdr-executor.desktop

AM_CPPFLAGS = -I$(top_srcdir)/kommander/widgets -I$(top_srcdir)/kommander/widget -I$(top_srcdir)/kommander/factory $(all_includes)


METASOURCES = AUTO

# the library search path.
kmdr_executor_LDFLAGS = $(all_libraries) $(KDE_RPATH)

noinst_HEADERS =  dcopkommanderif.h
