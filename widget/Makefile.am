lib_LTLIBRARIES = libkommanderwidget.la

# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# the library search path. 
libkommanderwidget_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)

libkommanderwidget_la_SOURCES = kommanderwidget.cpp myprocess.cpp

include_HEADERS = kommanderwidget.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO
