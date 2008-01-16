lib_LTLIBRARIES = libkommanderplugin.la

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/kommander/widget $(all_includes)

# the library search path. 
libkommanderplugin_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -no-undefined

# the libraries to link against.
libkommanderplugin_la_LIBADD  =  $(LIB_KDEUI) $(LIB_QT)

libkommanderplugin_la_SOURCES = kommanderplugin.cpp specialinformation.cpp

include_HEADERS = kommanderplugin.h specials.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO
