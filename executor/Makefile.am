bin_PROGRAMS = kmdr-executor

kmdr_executor_SOURCES =  instance.cpp main.cpp
kmdr_executor_LDADD   = $(top_builddir)/kommander/widgets/libkommanderwidgets.la $(top_builddir)/kommander/factory/libkommanderfactory.la  $(LIB_KIO)

KDE_ICON = kmdr-executor

appsdir = $(kde_appsdir)/Applications
apps_DATA = kmdr-executor.desktop

INCLUDES = -I$(top_srcdir)/kommander/editor -I$(top_srcdir)/kommander/widgets -I$(top_srcdir)/kommander/factory $(all_includes)
METASOURCES = AUTO

# the library search path.
kmdr_executor_LDFLAGS = $(all_libraries) $(KDE_RPATH)

