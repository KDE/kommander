lib_LTLIBRARIES = libkommanderwidgets.la

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/kommander/widget -I$(top_srcdir)/kommander/factory -I$(top_srcdir)/kommander/plugin $(all_includes)

# the library search path.
#widgets_LDFLAGS = $(KDE_RPATH) $(all_libraries)
libkommanderwidgets_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
libkommanderwidgets_la_LIBADD  = $(LIB_KIO) $(top_builddir)/kommander/widget/libkommanderwidget.la

# which sources should be compiled for widgets
libkommanderwidgets_la_SOURCES = buttongroup.cpp  subdialog.cpp checkbox.cpp closebutton.cpp combobox.cpp listbox.cpp wizard.cpp dialog.cpp tabwidget.cpp execbutton.cpp fileselector.cpp groupbox.cpp	lineedit.cpp radiobutton.cpp  spinboxint.cpp textedit.cpp scriptobject.cpp richtexteditor.cpp treewidget.cpp plugin.cpp statusbar.cpp progressbar.cpp textbrowser.cpp slider.cpp label.cpp pixmaplabel.cpp

# these are the headers for your project that won't be installed
noinst_HEADERS = buttongroup.h  checkbox.h  subdialog.h  closebutton.h  combobox.h listbox.h wizard.h dialog.h	   execbutton.h   fileselector.h  groupbox.h	 lineedit.h radiobutton.h	   spinboxint.h   textedit.h tabwidget.h scriptobject.h richtexteditor.h treewidget.h statusbar.h textbrowser.h label.h pixmaplabel.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO
