lib_LTLIBRARIES = libkommanderwidgets.la

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/kommander/widget -I$(top_srcdir)/kommander/factory -I$(top_srcdir)/kommander/plugin $(all_includes)

# the library search path.
#widgets_LDFLAGS = $(KDE_RPATH) $(all_libraries)
libkommanderwidgets_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -no-undefined

# the libraries to link against.
libkommanderwidgets_la_LIBADD  = $(LIB_KIO) $(LIB_QT) \
    $(top_builddir)/kommander/widget/libkommanderwidget.la \
    $(top_builddir)/kommander/plugin/libkommanderplugin.la \
    $(top_builddir)/kommander/factory/libkommanderfactory.la

# which sources should be compiled for widgets
libkommanderwidgets_la_SOURCES = buttongroup.cpp checkbox.cpp closebutton.cpp \
	colordialog.cpp combobox.cpp datepicker.cpp dialog.cpp execbutton.cpp fileselector.cpp \
	groupbox.cpp	lineedit.cpp konsole.cpp label.cpp listbox.cpp pixmaplabel.cpp plugin.cpp \
	popupmenu.cpp progressbar.cpp radiobutton.cpp richtexteditor.cpp scriptobject.cpp \
	slider.cpp spinboxint.cpp statusbar.cpp subdialog.cpp table.cpp tabwidget.cpp \
	textbrowser.cpp textedit.cpp timer.cpp toolbox.cpp treewidget.cpp wizard.cpp

# these are the headers for your project that won't be installed
noinst_HEADERS = buttongroup.h checkbox.h closebutton.h colordialog.h \
	combobox.h dialog.h	 execbutton.h fileselector.h groupbox.h	 label.h lineedit.h \
	listbox.h pixmaplabel.h popupmenu.h radiobutton.h	 richtexteditor.h \
	scriptobject.h spinboxint.h statusbar.h subdialog.h tabwidget.h textbrowser.h \
	textedit.h toolbox.h treewidget.h wizard.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO
