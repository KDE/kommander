### Kommander Widgets ###

INCLUDE_DIRECTORIES( 
	${CMAKE_SOURCE_DIR}/kommander/widget 
	${CMAKE_SOURCE_DIR}/kommander/factory 
	${CMAKE_SOURCE_DIR}/kommander/plugin 
	${CMAKE_CURRENT_SOURCE_DIR} 
	${CMAKE_CURRENT_BINARY_DIR} 
	${KDE4_INCLUDE_DIR} 
	${QT_INCLUDE_DIR}
)


########### next target ###############

SET(kommanderwidgets_LIB_SRCS
   buttongroup.cpp
   subdialog.cpp
   checkbox.cpp
   closebutton.cpp
   combobox.cpp
   listbox.cpp
   wizard.cpp
   dialog.cpp
   tabwidget.cpp
   execbutton.cpp
   fileselector.cpp
   groupbox.cpp
   lineedit.cpp
   radiobutton.cpp
   spinboxint.cpp
   textedit.cpp
   scriptobject.cpp
   richtexteditor.cpp
   treewidget.cpp
   plugin.cpp
   statusbar.cpp
   progressbar.cpp
   textbrowser.cpp
   slider.cpp
   label.cpp
   pixmaplabel.cpp
   konsole.cpp
   timer.cpp
   table.cpp
   )

ADD_LIBRARY(kommanderwidgets SHARED ${kommanderwidgets_LIB_SRCS})

TARGET_LINK_LIBRARIES(kommanderwidgets  ${QT_AND_KDECORE_LIBS} kio kommanderwidget kommanderplugin kommanderfactory )

SET_TARGET_PROPERTIES(kommanderwidgets PROPERTIES VERSION 4.2.0 SOVERSION 4 )
INSTALL(TARGETS kommanderwidgets DESTINATION lib} )


########### install files ###############




#original Makefile.am contents follow:

#lib_LTLIBRARIES = libkommanderwidgets.la
#
## set the include path for X, qt and KDE
#INCLUDES = -I$(top_srcdir)/kommander/widget -I$(top_srcdir)/kommander/factory -I$(top_srcdir)/kommander/plugin $(all_includes)
#
## the library search path.
##widgets_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#libkommanderwidgets_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -no-undefined
#
## the libraries to link against.
#libkommanderwidgets_la_LIBADD  = $(LIB_KIO) $(LIB_QT) \
#    $(top_builddir)/kommander/widget/libkommanderwidget.la \
#    $(top_builddir)/kommander/plugin/libkommanderplugin.la \
#    $(top_builddir)/kommander/factory/libkommanderfactory.la
#
## which sources should be compiled for widgets
#libkommanderwidgets_la_SOURCES = buttongroup.cpp subdialog.cpp checkbox.cpp \
#			closebutton.cpp combobox.cpp listbox.cpp wizard.cpp dialog.cpp tabwidget.cpp \
#			execbutton.cpp fileselector.cpp groupbox.cpp	lineedit.cpp radiobutton.cpp \
#			spinboxint.cpp textedit.cpp scriptobject.cpp richtexteditor.cpp treewidget.cpp \
#			plugin.cpp statusbar.cpp progressbar.cpp textbrowser.cpp slider.cpp label.cpp \
#			pixmaplabel.cpp konsole.cpp timer.cpp table.cpp
#
## these are the headers for your project that won't be installed
#noinst_HEADERS = buttongroup.h  checkbox.h  subdialog.h  closebutton.h  combobox.h listbox.h wizard.h dialog.h	   execbutton.h   fileselector.h  groupbox.h	 lineedit.h radiobutton.h	   spinboxint.h   textedit.h tabwidget.h scriptobject.h richtexteditor.h treewidget.h statusbar.h textbrowser.h label.h pixmaplabel.h
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
