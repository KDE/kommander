lib_LTLIBRARIES = libkommanderwidgets.la

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/kommander/editor -I$(top_srcdir)/kommander/factory -I$(top_srcdir)/kommander/plugin $(all_includes)

# the library search path.
#widgets_LDFLAGS = $(KDE_RPATH) $(all_libraries)
libkommanderwidgets_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
libkommanderwidgets_la_LIBADD  = $(LIB_KIO)

# which sources should be compiled for widgets
libkommanderwidgets_la_SOURCES = assoctextwidget.cpp myprocess.cpp buttongroup.cpp  subdialog.cpp checkbox.cpp closebutton.cpp combobox.cpp listbox.cpp wizard.cpp dialog.cpp tabwidget.cpp execbutton.cpp fileselector.cpp groupbox.cpp	lineedit.cpp radiobutton.cpp  spinboxint.cpp textedit.cpp scriptobject.cpp richtexteditor.cpp treewidget.cpp plugin.cpp

# these are the headers for your project that won't be installed
noinst_HEADERS = assoctextwidget.h  myprocess.h buttongroup.h  checkbox.h  subdialog.h  closebutton.h  combobox.h listbox.h wizard.h dialog.h	   execbutton.h   fileselector.h  groupbox.h	 lineedit.h radiobutton.h	   spinboxint.h   textedit.h tabwidget.h scriptobject.h richtexteditor.h treewidget.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO
