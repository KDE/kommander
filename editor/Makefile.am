# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
#SUBDIRS =

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kmdr-editor

# set the include path for X, qt and KDE
INCLUDES = $(all_includes) -Ishared -Iinterfaces -Ipics -I$(top_srcdir)/kommander/widgets

# the library search path. 
kmdr_editor_LDFLAGS = -L$(top_builddir)/kommander/widgets $(KDE_RPATH) $(all_libraries) 

# the libraries to link against.
kmdr_editor_LDADD   = -lkommanderwidgets $(LIB_KFILE)

# which sources should be compiled for kmdr_editor
kmdr_editor_SOURCES = actiondnd.cpp actioneditorimpl.cpp assoctexteditorimpl.cpp actionlistview.cpp asciivalidator.cpp assistproc.cpp command.cpp config.cpp	connectioneditorimpl.cpp  connectionviewerimpl.cpp  customwidgeteditorimpl.cpp defs.cpp designerapp.cpp designerappiface.cpp domtool.cpp editslotsimpl.cpp filechooser.cpp formfile.cpp formsettingsimpl.cpp formwindow.cpp hierarchyview.cpp iconvieweditorimpl.cpp layout.cpp listboxeditorimpl.cpp listvieweditorimpl.cpp main.cpp	mainwindow.cpp mainwindowactions.cpp  metadatabase.cpp multilineeditorimpl.cpp newformimpl.cpp orderindicator.cpp outputwindow.cpp paletteeditoradvancedimpl.cpp  paletteeditorimpl.cpp parser.cpp  pixmapchooser.cpp previewframe.cpp  previewwidgetimpl.cpp propertyeditor.cpp qcompletionedit.cpp	resource.cpp sizehandle.cpp sourceeditor.cpp sourcefile.cpp ewidgetfactory.cpp splashloader.cpp styledbutton.cpp tableeditorimpl.cpp timestamp.cpp widgetdatabase.cpp widgetfactory.cpp wizardeditorimpl.cpp workspace.cpp about.ui actioneditor.ui assoctexteditor.ui connectioneditor.ui connectionviewer.ui createtemplate.ui customwidgeteditor.ui editslots.ui formsettings.ui iconvieweditor.ui listboxeditor.ui listeditor.ui listvieweditor.ui multilineeditor.ui newform.ui paletteeditor.ui paletteeditoradvanced.ui pixmapfunction.ui preferences.ui previewwidget.ui tableeditor.ui wizardeditor.ui

# these are the headers for your project that won't be installed
noinst_HEADERS = actiondnd.h actioneditorimpl.h actioninterface.h  actionlistview.h asciivalidator.h assistproc.h  assoctexteditorimpl.h classbrowserinterface.h command.h config.h connectioneditorimpl.h connectionviewerimpl.h customwidgeteditorimpl.h defs.h designerapp.h designerappiface.h designerinterface.h domtool.h editorinterface.h editslotsimpl.h eventinterface.h ewidgetfactory.h filechooser.h filterinterface.h formfile.h formsettingsimpl.h formwindow.h globaldefs.h hierarchyview.h iconvieweditorimpl.h  interpreterinterface.h languageinterface.h layout.h listboxeditorimpl.h listvieweditorimpl.h mainwindow.h metadatabase.h multilineeditorimpl.h newformimpl.h orderindicator.h outputwindow.h paletteeditoradvancedimpl.h  paletteeditorimpl.h parser.h pixmapchooser.h  preferenceinterface.h previewframe.h previewwidgetimpl.h programinterface.h  projectsettingsiface.h propertyeditor.h qcompletionedit.h resource.h sizehandle.h sourceeditor.h sourcefile.h sourcetemplateiface.h splashloader.h	styledbutton.h tableeditorimpl.h templatewizardiface.h timestamp.h widgetdatabase.h widgetfactory.h widgetinterface.h wizardeditorimpl.h workspace.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kmdr-editor.pot

# this is where the desktop file will go 
desktopdir   = $(kde_appsdir)/Editors
desktop_DATA = kmdr-editor.desktop
