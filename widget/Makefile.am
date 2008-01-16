lib_LTLIBRARIES = libkommanderwidget.la

# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# the library search path. 
libkommanderwidget_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -no-undefined

# the libraries to link against.
libkommanderwidget_la_LIBADD  = $(top_builddir)/kommander/factory/libkommanderfactory.la $(LIB_KIO) $(LIB_KDEUI) $(LIB_QT)

libkommanderwidget_la_SOURCES = expression.cpp function.cpp functionlib.cpp \
	invokeclass.cpp kommanderfunctions.cpp kommanderwidget.cpp kommanderwindow.cpp \
	myprocess.cpp parsenode.cpp parser.cpp parserdata.cpp specialinformation.cpp \
	specials.cpp

include_HEADERS = kommanderwidget.h kommander_export.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO
noinst_HEADERS = invokeclass.h
