INCLUDES = -I$(top_srcdir)/kommander/executor -I$(top_srcdir)/kommander/factory $(all_includes)
METASOURCES = AUTO
kde_module_LTLIBRARIES = libkommander_part.la
libkommander_part_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libkommander_part_la_LIBADD = $(top_builddir)/kommander/executor/libinstance.la $(top_builddir)/kommander/widget/libkommanderwidget.la $(LIB_KFILE) $(LIB_KPARTS) $(LIB_KHTML)
kde_services_DATA = kommander_part.desktop
libkommander_part_la_SOURCES = kommander_part.cpp
noinst_HEADERS = kommander_part.h
