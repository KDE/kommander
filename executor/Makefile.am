bin_PROGRAMS = kmdr-executor

kmdr_executor_SOURCES = domtool.cpp instance.cpp main.cpp ewidgetfactory.cpp
kmdr_executor_LDADD   = $(top_builddir)/kommander/widgets/libkommanderwidgets.la $(LIB_KIO)

KDE_ICON = kmdr-executor

appsdir = $(kde_appsdir)/Applications
apps_DATA = kmdr-executor.desktop

INCLUDES = -I$(top_srcdir)/kommander/editor -I$(top_srcdir)/kommander/widgets $(all_includes)
METASOURCES = AUTO

# the library search path.
kmdr_executor_LDFLAGS = $(all_libraries) $(KDE_RPATH)

