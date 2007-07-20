### Kommander Widget ###

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} 
${CMAKE_CURRENT_BINARY_DIR} 
${KDE4_INCLUDE_DIR} 
${QT_INCLUDE_DIR}  )


########### next target ###############

SET(kommanderwidget_LIB_SRCS
   kommanderwidget.cpp
   myprocess.cpp
   specialinformation.cpp
   kommanderfunctions.cpp
   specials.cpp
   kommanderwindow.cpp
   expression.cpp
   parser.cpp
   parsenode.cpp
   parserdata.cpp
   function.cpp
   functionlib.cpp
   )

KDE4_ADD_LIBRARY(kommanderwidget SHARED ${kommanderwidget_LIB_SRCS}) 
TARGET_LINK_LIBRARIES(kommanderwidget  ${KDE4_KIO_LIBS} )

SET_TARGET_PROPERTIES(kommanderwidget PROPERTIES VERSION 4.2.0 SOVERSION 4 )
INSTALL(TARGETS kommanderwidget DESTINATION ${LIB_INSTALL_DIR} )


########### install files ###############

INSTALL(FILES kommanderwidget.h kommander_export.h DESTINATION ${INCLUDE_INSTALL_DIR})


#original Makefile.am contents follow:

#lib_LTLIBRARIES = libkommanderwidget.la
#
## set the include path for X, qt and KDE
#INCLUDES = $(all_includes)
#
## the library search path. 
#libkommanderwidget_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -no-undefined
#
## the libraries to link against.
#libkommanderwidget_la_LIBADD  = $(LIB_KIO) $(LIB_KDEUI) $(LIB_QT)
#
#libkommanderwidget_la_SOURCES = kommanderwidget.cpp myprocess.cpp \
#    specialinformation.cpp kommanderfunctions.cpp specials.cpp \
#    kommanderwindow.cpp expression.cpp \
#    parser.cpp parsenode.cpp parserdata.cpp function.cpp functionlib.cpp
#
#include_HEADERS = kommanderwidget.h kommander_export.h
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
