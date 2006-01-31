lib_LTLIBRARIES = libkommanderwidget.la

# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# the library search path. 
libkommanderwidget_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -no-undefined

# the libraries to link against.
libkommanderwidget_la_LIBADD  = $(LIB_KIO) $(LIB_KDEUI) $(LIB_QT)

libkommanderwidget_la_SOURCES = kommanderwidget.cpp myprocess.cpp \
    specialinformation.cpp kommanderfunctions.cpp specials.cpp \
    kommanderwindow.cpp expression.cpp \
    parser.cpp parsenode.cpp parserdata.cpp function.cpp functionlib.cpp

include_HEADERS = kommanderwidget.h kommander_export.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO
