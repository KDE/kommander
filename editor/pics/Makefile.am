noinst_LTLIBRARIES = libsplash.la

# set the include path for X, qt and KDE
AM_CPPFLAGS = $(all_includes)

# the library search path. 
libsplash_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# which sources should be compiled for factory
libsplash_la_SOURCES = splashloader.cpp

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO
