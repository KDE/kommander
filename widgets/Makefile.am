KDE_OPTIONS = nofinal

lib_LTLIBRARIES = libkommanderwidgets.la

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/kommander/editor $(all_includes)

# the library search path. 
#widgets_LDFLAGS = $(KDE_RPATH) $(all_libraries)
libkommanderwidgets_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
libkommanderwidgets_la_LIBADD  = $(LIB_KIO) 

# which sources should be compiled for widgets
libkommanderwidgets_la_SOURCES = assoctextwidget.cpp myprocess.cpp buttongroup.cpp ewidgetfactory.cpp subdialog.cpp checkbox.cpp closebutton.cpp combobox.cpp listbox.cpp wizard.cpp dialog.cpp tabwidget.cpp execbutton.cpp fileselector.cpp groupbox.cpp	lineedit.cpp radiobutton.cpp  spinboxint.cpp textedit.cpp

# these are the headers for your project that won't be installed
noinst_HEADERS = assoctextwidget.h  myprocess.h buttongroup.h  checkbox.h ewidgetfactory.h subdialog.h  closebutton.h  combobox.h listbox.h wizard.h dialog.h	   execbutton.h   fileselector.h  groupbox.h	 lineedit.h radiobutton.h	   spinboxint.h   textedit.h tabwidget.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

AM_CFLAGS = 
AM_CXXFLAGS = 
AM_FFLAGS = 
SUBDIRS = 
