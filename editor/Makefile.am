SUBDIRS = pics
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kmdr-editor

# set the include path for X, qt and KDE
AM_CPPFLAGS = -I$(top_srcdir)/kommander/widget -I$(top_srcdir)/kommander/widgets -I$(top_srcdir)/kommander/factory $(all_includes) -Ishared -Iinterfaces -Ipics

# the library search path.
kmdr_editor_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
kmdr_editor_LDADD = $(top_builddir)/kommander/factory/libkommanderfactory.la \
	$(top_builddir)/kommander/widget/libkommanderwidget.la \
	$(top_builddir)/kommander/widgets/libkommanderwidgets.la \
	$(top_builddir)/kommander/plugin/libkommanderplugin.la

# which sources should be compiled for kmdr_editor
kmdr_editor_SOURCES = actiondnd.cpp actioneditorimpl.cpp \
	assoctexteditorimpl.cpp actionlistview.cpp asciivalidator.cpp assistproc.cpp command.cpp \
	connectioneditorimpl.cpp defs.cpp filechooser.cpp formfile.cpp formsettingsimpl.cpp \
	formwindow.cpp hierarchyview.cpp iconvieweditorimpl.cpp layout.cpp \
	listboxeditorimpl.cpp listvieweditorimpl.cpp main.cpp mainwindow.cpp mainwindowactions.cpp \
	metadatabase.cpp multilineeditorimpl.cpp newformimpl.cpp orderindicator.cpp \
	paletteeditoradvancedimpl.cpp paletteeditorimpl.cpp parser.cpp pixmapchooser.cpp previewframe.cpp \
	previewwidgetimpl.cpp propertyeditor.cpp qcompletionedit.cpp resource.cpp sizehandle.cpp \
	styledbutton.cpp tableeditorimpl.cpp widgetdatabase.cpp widgetfactory.cpp \
	wizardeditorimpl.cpp workspace.cpp actioneditor.ui assoctexteditor.ui connectioneditor.ui \
	createtemplate.ui formsettings.ui iconvieweditor.ui listboxeditor.ui listeditor.ui \
	listvieweditor.ui multilineeditor.ui newform.ui paletteeditor.ui paletteeditoradvanced.ui \
	pixmapfunction.ui preferences.ui previewwidget.ui tableeditor.ui wizardeditor.ui \
	choosewidget.ui choosewidgetimpl.cpp functions.ui functionsimpl.cpp messagelog.cpp 

# these are the headers for your project that won't be installed
noinst_HEADERS = actiondnd.h actioneditorimpl.h actionlistview.h \
	asciivalidator.h assistproc.h assoctexteditorimpl.h command.h connectioneditorimpl.h \
	defs.h filechooser.h formfile.h formsettingsimpl.h formwindow.h globaldefs.h \
	hierarchyview.h iconvieweditorimpl.h layout.h listboxeditorimpl.h listvieweditorimpl.h \
	mainwindow.h metadatabase.h multilineeditorimpl.h newformimpl.h orderindicator.h \
	paletteeditoradvancedimpl.h paletteeditorimpl.h parser.h pixmapchooser.h previewframe.h \
	previewwidgetimpl.h propertyeditor.h qcompletionedit.h resource.h sizehandle.h \
	styledbutton.h tableeditorimpl.h widgetdatabase.h widgetfactory.h widgetinterface.h \
	wizardeditorimpl.h workspace.h choosewidgetimpl.h messagelog.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

xdg_apps_DATA = kmdr-editor.desktop
