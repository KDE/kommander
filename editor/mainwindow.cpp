 /**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef KOMMANDER
#include "designerappiface.h"
#endif
#include "designerapp.h"

#include "mainwindow.h"
#include "defs.h"
#include "globaldefs.h"
#include "formwindow.h"
#include "widgetdatabase.h"
#include "widgetfactory.h"
#include "propertyeditor.h"
#include "metadatabase.h"
#include "resource.h"
#include "pixmapchooser.h"
#include "config.h"
#include "hierarchyview.h"
#include "newformimpl.h"
#include "workspace.h"
#include "about.h"
#include "multilineeditorimpl.h"
#include "wizardeditorimpl.h"
#ifndef KOMMANDER
#include "outputwindow.h"
#endif
#include <klineeditdlg.h>
#if defined(HAVE_KDE)
#include <ktoolbar.h>
#else
#include <qtoolbar.h>
#endif
#ifdef HAVE_KDE
#include <kstatusbar.h>
#include <kmenubar.h>
#endif
#include <qfeatures.h>
#include <qmetaobject.h>
#include <qaction.h>
#include <qpixmap.h>
#include <qworkspace.h>
#include <qfiledialog.h>
#include <qclipboard.h>
#include <qmessagebox.h>
#include <qbuffer.h>
#include <qdir.h>
#include <qstyle.h>
#include <qlabel.h>
#include <qstatusbar.h>
#include <qfile.h>
#include <qcheckbox.h>
#include <qwhatsthis.h>
#include <qwizard.h>
#include <qtimer.h>
#include <qlistbox.h>
#include <stdio.h>
#include <stdlib.h>
#include <qdockwindow.h>
#include <qregexp.h>
#include <qstylefactory.h>
#include <qwidget.h>
#include "actioneditorimpl.h"
#include "actiondnd.h"
#ifndef KOMMANDER
#include "project.h"
#endif
#ifndef KOMMANDER
#include "projectsettingsimpl.h"
#endif
#ifndef KOMMANDER
#include "qwidgetfactory.h"
#else
#include <kommanderfactory.h>
#endif
#include "formfile.h"


#include <qvbox.h>
#include <qprocess.h>
#include <qsettings.h>
#ifndef KOMMANDER
#include "pixmapcollection.h"
#include "sourcefile.h"
#endif
#include "qcompletionedit.h"
#include <qaccel.h>
#include <qtooltip.h>
#include <stdlib.h>
#ifndef Q_OS_WIN32
#include "assistproc.h"
#endif

#include <kommanderwidget.h>
#include "assoctexteditorimpl.h"
#include <dialog.h>

#include <klocale.h>

static bool mblockNewForms = FALSE;
extern QMap<QWidget*, QString> *qwf_functions;
extern QMap<QWidget*, QString> *qwf_forms;
extern QString *qwf_language;
extern bool qwf_execute_code;
extern bool qwf_stays_on_top;
extern void set_splash_status( const QString &txt );
/*### static bool tbSettingsRead = FALSE; */

MainWindow *MainWindow::self = 0;

QString assistantPath()
{
#ifdef Q_OS_MACX
    return QDir::cleanDirPath(QString(getenv("QTDIR")) + QDir::separator() +
                              "bin" + QDir::separator() +
                              "assistant.app/Contents/MacOS/assistant");
#else
    return "assistant";
#endif
}


static QString textNoAccel( const QString& text)
{
    QString t = text;
    int i;
    while ( (i = t.find('&') )>= 0 ) {
        t.remove(i,1);
    }
    return t;
}


MainWindow::MainWindow( bool asClient )
#if defined(HAVE_KDE)
    : KMainWindow( 0, "mainwindow", WType_TopLevel | WDestructiveClose | WGroupLeader ),
#else
    : QMainWindow( 0, "mainwindow", WType_TopLevel | WDestructiveClose | WGroupLeader ),
#endif
      grd( 10, 10 ), sGrid( TRUE ), snGrid( TRUE ), restoreConfig( TRUE ), splashScreen( TRUE ),
#ifndef KOMMANDER
      docPath( "$QTDIR/doc/html" ), fileFilter( i18n("Qt User-Interface Files (*.ui)" ) ), client( asClient ),
#else
      docPath( "$QTDIR/doc/html" ), fileFilter( i18n("Kommander Files (*.kmdr)" ) ), client( asClient ),
#endif
      previewing( FALSE ), databaseAutoEdit( FALSE )
{
    init_colors();

#ifndef KOMMANDER
    desInterface = new DesignerInterfaceImpl( this );
    desInterface->addRef();
#endif
    inDebugMode = TRUE; //debugging kommander

#ifndef KOMMANDER
	// slots timer for checking for new slots in code.. not needed
    updateSlotsTimer = new QTimer( this );
    connect( updateSlotsTimer, SIGNAL( timeout() ),
	     this, SLOT( doSlotsChanged() ) );
#endif

    set_splash_status( i18n("Creating plugin factories...") );

#ifndef KOMMANDER
    setupPluginManagers();
#else
    setupPlugins();
#endif

    qApp->setMainWidget( this );
#ifndef KOMMANDER
    QWidgetFactory::addWidgetFactory( new CustomWidgetFactory );
#endif
    self = this;
#ifndef KOMMANDER
    setIcon( PixmapChooser::loadPixmap( "logo" ) );
#endif

    actionGroupTools = 0;
    prefDia = 0;
    windowMenu = 0;
    hierarchyView = 0;
    actionEditor = 0;
#ifndef KOMMANDER
    currentProject = 0;
    oWindow = 0;
#endif
    wspace = 0;

    statusBar()->clear();
#ifndef KOMMANDER
#if defined(QT_NON_COMMERCIAL)
    statusBar()->addWidget( new QLabel(i18n("Ready - This is the non-commercial version of Qt - "
        "For commercial evaluations, use the help menu to register with Trolltech."), statusBar()), 1 );
#else
    statusBar()->addWidget( new QLabel(i18n("Ready"), statusBar()), 1 );
#endif
#else
    statusBar()->addWidget(new QLabel(i18n("Welcome to the Kommander Editor"), statusBar()), 1);
#endif


    set_splash_status( i18n("Setting up GUI...") );
    setupMDI();
    setupMenuBar();

    setupFileActions();
    setupEditActions();
#ifndef KOMMANDER
    setupProjectActions();
#endif
#ifndef KOMMANDER
    setupSearchActions();
#endif
#if defined(HAVE_KDE)
    layoutToolBar = new KToolBar( this, "Layout" );
    ( (KToolBar*)layoutToolBar )->setFullSize( FALSE );
#else
    layoutToolBar = new QToolBar( this, "Layout" );
    layoutToolBar->setCloseMode( QDockWindow::Undocked );
#endif
    addToolBar( layoutToolBar, i18n("Layout" ) );
    setupToolActions();
    setupLayoutActions();
    setupPreviewActions();
#ifndef KOMMANDER
    setupOutputWindow();
#endif
#ifndef KOMMANDER
    setupActionManager();
#endif
    setupWindowActions();

    setupWorkspace();
    setupHierarchyView();
    setupPropertyEditor();
    setupActionEditor();

    setupHelpActions();

    setupRMBMenus();

#ifndef KOMMANDER
    connect( this, SIGNAL( projectChanged() ), this, SLOT( emitProjectSignals() ) );
#endif
#ifndef KOMMANDER
    connect( this, SIGNAL( hasActiveWindow(bool) ), this, SLOT( emitProjectSignals() ) );
#endif

    emit hasActiveForm( FALSE );
    emit hasActiveWindow( FALSE );

    lastPressWidget = 0;
    qApp->installEventFilter( this );

    QSize as( qApp->desktop()->size() );
    as -= QSize( 30, 30 );
    resize( QSize( 1200, 1000 ).boundedTo( as ) );

    connect( qApp->clipboard(), SIGNAL( dataChanged() ),
             this, SLOT( clipboardChanged() ) );
    clipboardChanged();
    layoutChilds = FALSE;
    layoutSelected = FALSE;
    breakLayout = FALSE;
    backPix = TRUE;

    set_splash_status( i18n("Loading User Settings...") );
    readConfig();

    // hack to make WidgetFactory happy (so it knows QWidget and QDialog for resetting properties)
    QWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QWidget" ), this, 0, FALSE );
    delete w;
#ifndef KOMMANDER
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QDialog" ), this, 0, FALSE );
#else
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "Dialog" ), this, 0, FALSE );
#endif

    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QLabel" ), this, 0, FALSE );
    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QTabWidget" ), this, 0, FALSE );
    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QFrame" ), this, 0, FALSE );
    delete w;

    setAppropriate( (QDockWindow*)actionEditor->parentWidget(), FALSE );
    actionEditor->parentWidget()->hide();

#ifndef Q_OS_WIN32
    assistant = new AssistProc( this, "Internal Assistant", assistantPath() );
#endif

    statusBar()->setSizeGripEnabled( TRUE );
    set_splash_status( i18n("Initialization Done.") );
}

MainWindow::~MainWindow()
{
#ifndef KOMMANDER
return;
    QValueList<Tab>::Iterator tit;
    for ( tit = preferenceTabs.begin(); tit != preferenceTabs.end(); ++tit ) {
        Tab t = *tit;
        delete t.w;
    }
    for ( tit = projectTabs.begin(); tit != projectTabs.end(); ++tit ) {
        Tab t = *tit;
        delete t.w;
    }

    QMap< QAction*, Project* >::Iterator it = projects.begin();
    while ( it != projects.end() ) {
        Project *p = *it;
        ++it;
        delete p;
    }
    delete projects.getFirst();
    projects.clear();

    delete oWindow;
    oWindow = 0;

#endif
#ifndef KOMMANDER
    desInterface->release();
    desInterface = 0;

    delete actionPluginManager;
    delete preferencePluginManager;
    delete templateWizardPluginManager;

    delete interpreterPluginManager;
    delete programPluginManager;
    delete projectSettingsPluginManager;
  //  delete templateWizardPluginManager;
    delete editorPluginManager;
    delete sourceTemplatePluginManager;
#endif

    MetaDataBase::clearDataBase();
}

void MainWindow::setupMDI()
{
    QVBox *vbox = new QVBox( this );
    setCentralWidget( vbox );
    vbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    vbox->setMargin( 1 );
    vbox->setLineWidth( 1 );
    qworkspace = new QWorkspace( vbox );
    qworkspace->setBackgroundMode( PaletteDark );
    qworkspace->setBackgroundPixmap( PixmapChooser::loadPixmap( "background.png", PixmapChooser::NoSize ) );
    qworkspace->setPaletteBackgroundColor(QColor(238, 238, 238));
    qworkspace->setScrollBarsEnabled( TRUE );
    connect( qworkspace, SIGNAL( windowActivated( QWidget * ) ),
             this, SLOT( activeWindowChanged( QWidget * ) ) );
    lastActiveFormWindow = 0;
    qworkspace->setAcceptDrops( TRUE );
}

void MainWindow::setupMenuBar()
{
    menubar = menuBar();
}

void MainWindow::setupPropertyEditor()
{
    QDockWindow *dw = new QDockWindow;
    dw->setResizeEnabled( TRUE );
    dw->setCloseMode( QDockWindow::Always );
    propertyEditor = new PropertyEditor( dw );
    addToolBar( dw, Qt::DockLeft );
    dw->setWidget( propertyEditor );
    dw->setFixedExtentWidth( 300 );
    dw->setCaption( i18n("Property Editor/Signal Handlers" ) );
    QWhatsThis::add( propertyEditor,
                     i18n("<b>The Property Editor</b>"
                        "<p>You can change the appearance and behavior of the selected widget in the "
                        "property editor.</p>"
                        "<p>You can set properties for components and forms at design time and see the "
                        "immediately see the effects of the changes. "
                        "Each property has its own editor which (depending on the property) can be used "
                        "to enter "
                        "new values, open a special dialog, or to select values from a predefined list. "
                        "Click <b>F1</b> to get detailed help for the selected property.</p>"
                        "<p>You can resize the columns of the editor by dragging the separators in the "
                        "list's header.</p>"
                        "<p><b>Signal Handlers</b></p>"
                        "<p>In the Signal Handlers tab you can define connections between "
                        "the signals emitted by widgets and the slots in the form. "
                        "(These connections can also be made using the connection tool.)" ) );
    dw->show();
}

#ifndef KOMMANDER
void MainWindow::setupOutputWindow()
{
    QDockWindow *dw = new QDockWindow;
    dw->setResizeEnabled( TRUE );
    dw->setCloseMode( QDockWindow::Always );
    addToolBar( dw, Qt::DockBottom );
    oWindow = new OutputWindow( dw );
    dw->setWidget( oWindow );
    dw->setFixedExtentHeight( 200 );
    dw->setCaption( i18n("Output Window" ) );
    dw->hide();
}
#endif

void MainWindow::setupHierarchyView()
{
    if ( hierarchyView )
        return;
    QDockWindow *dw = new QDockWindow;
    dw->setResizeEnabled( TRUE );
    dw->setCloseMode( QDockWindow::Always );
    hierarchyView = new HierarchyView( dw );
    addToolBar( dw, Qt::DockLeft );
    dw->setWidget( hierarchyView );

    dw->setCaption( i18n("Object Explorer" ) );
    dw->setFixedExtentWidth( 300 );
    QWhatsThis::add( hierarchyView,
                     i18n("<b>The Object Explorer</b>"
                        "<p>The Object Explorer provides an overview of the relationships "
                        "between the widgets in a form. You can use the clipboard functions using "
                        "a context menu for each item in the view. It is also useful for selecting widgets "
                        "in forms that have complex layouts.</p>"
                        "<p>The columns can be resized by dragging the separator in the list's header.</p>"
                        "<p>The second tab shows all the form's slots, class variables, includes, etc.</p>") );
    dw->show();
}

void MainWindow::setupWorkspace()
{
    QDockWindow *dw = new QDockWindow;
    dw->setResizeEnabled( TRUE );
    dw->setCloseMode( QDockWindow::Always );
    QVBox *vbox = new QVBox( dw );
    QCompletionEdit *edit = new QCompletionEdit( vbox );
    QToolTip::add( edit, i18n("Start typing the buffer you want to switch to here (ALT+B)" ) );
    QAccel *a = new QAccel( this );
    a->connectItem( a->insertItem( ALT + Key_B ), edit, SLOT( setFocus() ) );

    wspace = new Workspace( vbox, this );
    wspace->setBufferEdit( edit );

#ifndef KOMMANDER
    currentProject = projects.getFirst();
    wspace->setCurrentProject( currentProject );
#endif
    addToolBar( dw, Qt::DockLeft );
    dw->setWidget( vbox );

    dw->setCaption( i18n("File Overview" ) );
    QWhatsThis::add( wspace, i18n("<b>The File Overview Window</b>"
                                "<p>The File Overview Window displays all the files, "
                                "including forms and source files.</p>"
                                "<p>Use the search field to rapidly switch between files.</p>"));
    dw->setFixedExtentHeight( 100 );
    dw->show();
}

void MainWindow::setupActionEditor()
{
    QDockWindow *dw = new QDockWindow( QDockWindow::OutsideDock, this, 0 );
    addDockWindow( dw, Qt::DockTornOff );
    dw->setResizeEnabled( TRUE );
    dw->setCloseMode( QDockWindow::Always );
    actionEditor = new ActionEditor( dw );
    dw->setWidget( actionEditor );
    actionEditor->show();
    dw->setCaption( i18n("Action Editor" ) );
    QWhatsThis::add( actionEditor, i18n("<b>The Action Editor</b>"
                                      "<p>The Action Editor is used to add actions and action groups to "
                                      "a form, and to connect actions to slots. Actions and action "
                                      "groups can be dragged into menus and into toolbars, and may "
                                      "feature keyboard shortcuts and tooltips. If actions have pixmaps "
                                      "these are displayed on toolbar buttons and beside their names in "
                                      "menus.</p>" ) );
    dw->hide();
    setAppropriate( dw, FALSE );
}

void MainWindow::setupRMBMenus()
{
    rmbWidgets = new QPopupMenu( this );
    actionEditCut->addTo( rmbWidgets );
    actionEditCopy->addTo( rmbWidgets );
    actionEditPaste->addTo( rmbWidgets );
    actionEditDelete->addTo( rmbWidgets );
#if 0
    rmbWidgets->insertSeparator();
    actionEditLower->addTo( rmbWidgets );
    actionEditRaise->addTo( rmbWidgets );
#endif
    rmbWidgets->insertSeparator();
    actionEditAdjustSize->addTo( rmbWidgets );
    actionEditHLayout->addTo( rmbWidgets );
    actionEditVLayout->addTo( rmbWidgets );
    actionEditGridLayout->addTo( rmbWidgets );
    actionEditSplitHorizontal->addTo( rmbWidgets );
    actionEditSplitVertical->addTo( rmbWidgets );
    actionEditBreakLayout->addTo( rmbWidgets );
    rmbWidgets->insertSeparator();
    actionEditConnections->addTo( rmbWidgets );
#ifndef KOMMANDER
    actionEditSource->addTo( rmbWidgets );
#endif

    rmbFormWindow = new QPopupMenu( this );
    actionEditPaste->addTo( rmbFormWindow );
    actionEditSelectAll->addTo( rmbFormWindow );
    actionEditAccels->addTo( rmbFormWindow );
    rmbFormWindow->insertSeparator();
    actionEditAdjustSize->addTo( rmbFormWindow );
    actionEditHLayout->addTo( rmbFormWindow );
    actionEditVLayout->addTo( rmbFormWindow );
    actionEditGridLayout->addTo( rmbFormWindow );
    actionEditBreakLayout->addTo( rmbFormWindow );
    rmbFormWindow->insertSeparator();
    actionEditScriptObjects->addTo(rmbFormWindow);
    actionEditSlots->addTo( rmbFormWindow );
    actionEditConnections->addTo( rmbFormWindow );
#ifndef KOMMANDER
    actionEditSource->addTo( rmbFormWindow );
#endif
    rmbFormWindow->insertSeparator();
    actionEditFormSettings->addTo( rmbFormWindow );
}

void MainWindow::toolSelected( QAction* action )
{
    actionCurrentTool = action;
    emit currentToolChanged();
    if ( formWindow() )
        formWindow()->commandHistory()->emitUndoRedo();
}

int MainWindow::currentTool() const
{
    if ( !actionCurrentTool )
        return POINTER_TOOL;
    return QString::fromLatin1(actionCurrentTool->name()).toInt();
}

#ifndef KOMMANDER
QObjectList *MainWindow::runProject()
{
    static QWidget *invisibleGroupLeader = 0;
    if ( !invisibleGroupLeader ) {
        invisibleGroupLeader = new QWidget( 0, "designer_invisible_group_leader", WGroupLeader );
        invisibleGroupLeader->hide();
    }

    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
        e->save();
        e->saveBreakPoints();
    }
    fileSaveProject();

    if ( currentTool() == ORDER_TOOL )
        resetTool();
    if ( !currentProject )
        return 0;
    //oWindow->parentWidget()->show();
    oWindow->clearErrorMessages();
    oWindow->clearDebug();
    oWindow->showDebugTab();
    QApplication::setOverrideCursor( WaitCursor );

    delete qwf_functions;
    qwf_functions = 0;
    delete qwf_forms;
    qwf_forms = 0;
    delete qwf_language;
    qwf_language = new QString( currentProject->language() );
    qwf_execute_code = FALSE;

    previewing = TRUE;


    for ( QPtrListIterator<FormFile> it = currentProject->formFiles(); it.current(); ++it ) {
#ifndef KOMMANDER
            QWidget *w = QWidgetFactory::create( currentProject->makeAbsolute( (*it)->fileName() ), 0, invisibleGroupLeader );
#else
            QWidget *w = KommanderFactory::create( currentProject->makeAbsolute( (*it)->fileName() ), 0, invisibleGroupLeader );
#endif

        if ( w ) {
            w->hide();
            if ( programPluginManager ) {
                QString lang = currentProject->language();
                ProgramInterface *piface = 0;
                programPluginManager->queryInterface( lang, &piface);
                if ( piface ) {
                    QStringList error;
                    QValueList<int> line;
                    if ( qwf_functions ) {
                        QMap<QWidget*, QString>::Iterator it = qwf_functions->find( w );
                        if ( it == qwf_functions->end() )
                            continue;
                        if ( !piface->check( *it, error, line ) && !error.isEmpty() && !error[ 0 ].isEmpty() ) {
                            showSourceLine( it.key(), line[ 0 ] - 1, Error );
                            oWindow->setErrorMessages( error, line );
                            piface->release();
                            QApplication::restoreOverrideCursor();
                            return 0;
                        }
                    }
                    for ( QPtrListIterator<SourceFile> sources = currentProject->sourceFiles();
                          sources.current(); ++sources ) {
                        SourceFile* f = sources.current();
                        QStringList error;
                        QValueList<int> line;
                        if ( !piface->check( f->text(), error, line ) && !error.isEmpty() && !error[ 0 ].isEmpty() ) {
                            showSourceLine( f, line[ 0 ] - 1, Error );
                            oWindow->setErrorMessages( error, line );
                            piface->release();
                            QApplication::restoreOverrideCursor();
                            return 0;
                        }
                    }
                    piface->release();
                }
            }
        }
    }

    delete qwf_functions;
    qwf_functions = 0;
    delete qwf_forms;
    qwf_forms = 0;
    delete qwf_language;
    qwf_language = new QString( currentProject->language() );
    qwf_execute_code = TRUE;
    qwf_stays_on_top = TRUE;

    InterpreterInterface *iiface = 0;
    if ( interpreterPluginManager ) {
        QString lang = currentProject->language();
        iiface = 0;
        interpreterPluginManager->queryInterface( lang, &iiface );
        if ( iiface ) {
            iiface->onShowDebugStep( this, SLOT( showDebugStep( QObject *, int ) ) );
            iiface->onShowStackFrame( this, SLOT( showStackFrame( QObject *, int ) ) );
            iiface->onShowError( this, SLOT( showErrorMessage( QObject *, int, const QString & ) ) );
            iiface->onFinish( this, SLOT( finishedRun() ) );
        }

        LanguageInterface *liface = MetaDataBase::languageInterface( lang );
        if ( iiface )
            iiface->init();
        if ( liface && liface->supports( LanguageInterface::AdditionalFiles ) ) {
            for ( QPtrListIterator<SourceFile> sources = currentProject->sourceFiles();
                  sources.current(); ++sources ) {
                SourceFile* f = sources.current();
                iiface->exec( f, f->text() );
            }
        }
    }

    QObjectList *l = new QObjectList;
    if ( iiface ) {
        bool hasForms = FALSE;
        for ( QPtrListIterator<FormFile> forms = currentProject->formFiles();
              forms.current(); ++forms ) {
            FormFile* f = forms.current();
            hasForms = TRUE;
            if ( !f->formWindow() )
                continue;
            FormWindow* fw = f->formWindow();
            QValueList<int> bps = MetaDataBase::breakPoints( fw );
            if ( !bps.isEmpty() )
                iiface->setBreakPoints( fw, bps );
        }

        for ( QPtrListIterator<SourceFile> sources = currentProject->sourceFiles();
              sources.current(); ++sources ) {
            SourceFile* f = sources.current();
            QValueList<int> bps = MetaDataBase::breakPoints( f );
            if ( !bps.isEmpty() )
                iiface->setBreakPoints( f, bps );
        }

        if ( hasForms )
            iiface->exec( 0, "main" );

        for ( QPtrListIterator<FormFile> it2 = currentProject->formFiles(); it2.current(); ++it2 ) {
#ifndef KOMMANDER
                QWidget *w = QWidgetFactory::create( (*it2)->absFileName(), 0, invisibleGroupLeader );
#else
                QWidget *w = KommanderFactory::create( (*it2)->absFileName(), 0, invisibleGroupLeader );
#endif
            if ( w ) {
                l->append( w );
                w->hide();
            }
        }

        for ( QObject *o = l->first(); o; o = l->next() ) {
            QWidget *fw = findRealForm( (QWidget*)o );
            if ( !fw )
                continue;
            QValueList<int> bps = MetaDataBase::breakPoints( fw );
            if ( !bps.isEmpty() )
                iiface->setBreakPoints( o, bps );
        }

        iiface->release();
    }

    QApplication::restoreOverrideCursor();
    inDebugMode = TRUE;

    debuggingForms = *l;

    enableAll( FALSE );

    qwf_stays_on_top = FALSE;

    for ( SourceEditor *e2 = sourceEditors.first(); e2; e2 = sourceEditors.next() ) {
        if ( e2->project() == currentProject )
            e2->editorInterface()->setMode( EditorInterface::Debugging );
    }
    return l;
}
#endif

QWidget* MainWindow::previewFormInternal( QStyle* style, QPalette* palet )
{
    qwf_execute_code = FALSE;
#ifndef KOMMANDER
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() )
	e->save();
#endif
    if ( currentTool() == ORDER_TOOL )
        resetTool();

    FormWindow *fw = formWindow();
    if ( !fw )
        return 0;

#ifndef KOMMANDER
    QStringList databases;
    QPtrDictIterator<QWidget> wit( *fw->widgets() );
    while ( wit.current() ) {
        QStringList lst = MetaDataBase::fakeProperty( wit.current(), "database" ).toStringList();
        if ( !lst.isEmpty() )
            databases << lst [ 0 ];
        ++wit;
    }

    if ( fw->project() ) {
        QStringList::Iterator it;
        for ( it = databases.begin(); it != databases.end(); ++it )
            fw->project()->openDatabase( *it, FALSE );
    }
#endif
    QApplication::setOverrideCursor( WaitCursor );

    QCString s;
    QBuffer buffer( s );
    buffer.open( IO_WriteOnly );
    Resource resource( this );
    resource.setWidget( fw );
    QValueList<Resource::Image> images;
    resource.save( &buffer );

    buffer.close();
    buffer.open( IO_ReadOnly );

#ifndef KOMMANDER
    QWidget *w = QWidgetFactory::create( &buffer );
#else
#if 0
    QWidget *w = EWidgetFactory::create( &buffer );
#else
    QWidget *w = KommanderFactory::create( &buffer );
#endif
#endif
    if ( w ) {
        previewedForm = w;
        if ( palet ) {
            if ( style )
                style->polish( *palet );
            w->setPalette( *palet );
        }

        if ( style )
            w->setStyle( style );

        QObjectList *l = w->queryList( "QWidget" );
        for ( QObject *o = l->first(); o; o = l->next() ) {
            if ( style )
                ( (QWidget*)o )->setStyle( style );
        }
        delete l;

        w->move( fw->mapToGlobal( QPoint(0,0) ) );
        ((MainWindow*)w )->setWFlags( WDestructiveClose );
        previewing = TRUE;
        w->show();
        previewing = FALSE;
        QApplication::restoreOverrideCursor();
        return w;
    }
    QApplication::restoreOverrideCursor();
    return 0;
}

void MainWindow::previewForm()
{
    QWidget* w = previewFormInternal();
    if ( w )
        w->show();
}

void MainWindow::previewForm( const QString & style )
{
    QStyle* st = QStyleFactory::create( style );
    QWidget* w = 0;
    if ( style == "Motif" ) {
        QPalette p( QColor( 192, 192, 192 ) );
        w = previewFormInternal( st, &p );
    } else if ( style == "Windows" ) {
        QPalette p( QColor( 212, 208, 200 ) );
        w = previewFormInternal( st, &p );
    } else if ( style == "Platinum" ) {
        QPalette p( QColor( 220, 220, 220 ) );
        w = previewFormInternal( st, &p );
    } else if ( style == "CDE" ) {
        QPalette p( QColor( 75, 123, 130 ) );
        p.setColor( QPalette::Active, QColorGroup::Base, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Inactive, QColorGroup::Base, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Disabled, QColorGroup::Base, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Active, QColorGroup::Highlight, Qt::white );
        p.setColor( QPalette::Active, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Inactive, QColorGroup::Highlight, Qt::white );
        p.setColor( QPalette::Inactive, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Disabled, QColorGroup::Highlight, Qt::white );
        p.setColor( QPalette::Disabled, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Active, QColorGroup::Foreground, Qt::white );
        p.setColor( QPalette::Active, QColorGroup::Text, Qt::white );
        p.setColor( QPalette::Active, QColorGroup::ButtonText, Qt::white );
        p.setColor( QPalette::Inactive, QColorGroup::Foreground, Qt::white );
        p.setColor( QPalette::Inactive, QColorGroup::Text, Qt::white );
        p.setColor( QPalette::Inactive, QColorGroup::ButtonText, Qt::white );
        p.setColor( QPalette::Disabled, QColorGroup::Foreground, Qt::lightGray );
        p.setColor( QPalette::Disabled, QColorGroup::Text, Qt::lightGray );
        p.setColor( QPalette::Disabled, QColorGroup::ButtonText, Qt::lightGray );

        w = previewFormInternal( st, &p );
    } else if ( style == "SGI" ) {
        QPalette p( QColor( 220, 220, 220 ) );
        w = previewFormInternal( st, &p );
    } else if ( style == "MotifPlus" ) {
        QColor gtkfg(0x00, 0x00, 0x00);
        QColor gtkdf(0x75, 0x75, 0x75);
        QColor gtksf(0xff, 0xff, 0xff);
        QColor gtkbs(0xff, 0xff, 0xff);
        QColor gtkbg(0xd6, 0xd6, 0xd6);
        QColor gtksl(0x00, 0x00, 0x9c);
        QColorGroup active(gtkfg,            // foreground
                           gtkbg,            // button
                           gtkbg.light(),    // light
                           gtkbg.dark(142),  // dark
                           gtkbg.dark(110),  // mid
                           gtkfg,            // text
                           gtkfg,            // bright text
                           gtkbs,            // base
                           gtkbg),           // background
            disabled(gtkdf,            // foreground
                     gtkbg,            // button
                     gtkbg.light(), // light
                     gtkbg.dark(156),  // dark
                     gtkbg.dark(110),  // mid
                     gtkdf,            // text
                     gtkdf,            // bright text
                     gtkbs,            // base
                     gtkbg);           // background

        QPalette pal(active, disabled, active);

        pal.setColor(QPalette::Active, QColorGroup::Highlight,
                     gtksl);
        pal.setColor(QPalette::Active, QColorGroup::HighlightedText,
                     gtksf);
        pal.setColor(QPalette::Inactive, QColorGroup::Highlight,
                     gtksl);
        pal.setColor(QPalette::Inactive, QColorGroup::HighlightedText,
                     gtksf);
        pal.setColor(QPalette::Disabled, QColorGroup::Highlight,
                     gtksl);
        pal.setColor(QPalette::Disabled, QColorGroup::HighlightedText,
                     gtkdf);
        w = previewFormInternal( st, &pal );
    } else {
        w = previewFormInternal( st );
    }

    if ( !w )
        return;
    w->insertChild( st );
    w->show();
}

void MainWindow::helpContents()
{
    QWidget *focusWidget = qApp->focusWidget();
    bool showClassDocu = TRUE;
    while ( focusWidget ) {
        if ( focusWidget->isA( "PropertyList" ) ) {
            showClassDocu = FALSE;
            break;
        }
        focusWidget = focusWidget->parentWidget();
    }

    QString source = "designer-manual.html";
    if ( propertyDocumentation.isEmpty() ) {
        QString indexFile = documentationPath() + "/propertyindex";
        QFile f( indexFile );
        if ( f.open( IO_ReadOnly ) ) {
            QTextStream ts( &f );
            while ( !ts.eof() ) {
                QString s = ts.readLine();
                int from = s.find( "\"" );
                if ( from == -1 )
                    continue;
                int to = s.findRev( "\"" );
                if ( to == -1 )
                    continue;
                propertyDocumentation[ s.mid( from + 1, to - from - 1 ) ] = s.mid( to + 2 ) + "-prop";
            }
            f.close();
        } else {
#if 0
            QMessageBox::critical( this, i18n("Error" ), i18n("Could not find the Qt documentation index file.\n"
                                            "Please set the correct documentation path in the preferences dialog." ) );
#endif
        }
    }

    if ( propertyEditor->widget() && !showClassDocu ) {
        if ( !propertyEditor->currentProperty().isEmpty() ) {
            QMetaObject* mo = propertyEditor->metaObjectOfCurrentProperty();
            QString s;
            QString cp = propertyEditor->currentProperty();
            if ( cp == "layoutMargin" ) {
                source = propertyDocumentation[ "QLayout/margin" ];
            } else if ( cp == "layoutSpacing" ) {
                source = propertyDocumentation[ "QLayout/spacing" ];
            } else if ( cp == "toolTip" ) {
                source = "qtooltip.html#details";
            } else if ( mo && qstrcmp( mo->className(), "Spacer" ) == 0 ) {
                if ( cp != "name" )
                    source = "qsizepolicy.html#SizeType";
                else
                    source = propertyDocumentation[ "QObject/name" ];
            } else {
                while ( mo && !propertyDocumentation.contains( ( s = QString( mo->className() ) + "/" + cp ) ) )
                    mo = mo->superClass();
                if ( mo )
                    source = "p:" + propertyDocumentation[s];
            }
        }

        QString classname =  WidgetFactory::classNameOf( propertyEditor->widget() );
        if ( source.isEmpty() || source == "designer-manual.html" ) {
            if ( classname.lower() == "spacer" )
                source = "qspaceritem.html#details";
            else if ( classname == "QLayoutWidget" )
                source = "layout.html";
            else
                source = QString( WidgetFactory::classNameOf( propertyEditor->widget() ) ).lower() + ".html#details";
        }
    } else if ( propertyEditor->widget() ) {
        source = QString( WidgetFactory::classNameOf( propertyEditor->widget() ) ).lower() + ".html#details";
    }

#ifdef Q_OS_WIN32
    if ( !source.isEmpty() ) {
        QStringList lst;
        lst << assistantPath() << QString( "d:" + source );
        QProcess proc( lst );
        proc.start();
    }
#else
    if ( !source.isEmpty() )
        if ( assistant ) assistant->sendRequest( source+'\n' );
#endif
}

void MainWindow::helpManual()
{
#ifdef Q_OS_WIN32
    QStringList lst;
    lst << assistantPath() << "d:designer-manual.html";
    QProcess proc( lst );
    proc.start();
#else
    if ( assistant )
        assistant->sendRequest( "designer-manual.html\n" );
#endif
}

void MainWindow::helpAbout()
{
    AboutDialog dlg( this, 0, TRUE );
    dlg.exec();
}

void MainWindow::helpAboutQt()
{
    QMessageBox::aboutQt( this, "Qt Designer" );
}

#if defined(_WS_WIN_)
#include <qt_windows.h>
#include <qprocess.h>
#endif

void MainWindow::helpRegister()
{
#if defined(_WS_WIN_)
    HKEY key;
    HKEY subkey;
    long res;
    DWORD type;
    DWORD size = 255;
    QString command;
    QString sub( "htmlfile\\shell" );
#if defined(UNICODE)
    if ( QApplication::winVersion() & Qt::WV_NT_based ) {
        unsigned char data[256];
        res = RegOpenKeyExW( HKEY_CLASSES_ROOT, NULL, 0, KEY_READ, &key );
        res = RegOpenKeyExW( key, (TCHAR*)qt_winTchar( sub, TRUE ), 0, KEY_READ, &subkey );
        res = RegQueryValueExW( subkey, NULL, NULL, &type, data, &size );
        command = qt_winQString( data ) + "\\command";
        size = 255;
        res = RegOpenKeyExW( subkey, (TCHAR*)qt_winTchar( command, TRUE ), 0, KEY_READ, &subkey );
        res = RegQueryValueExW( subkey, NULL, NULL, &type, data, &size );
        command = qt_winQString( data );
    } else
#endif
    {
        unsigned char data[256];
        res = RegOpenKeyExA( HKEY_CLASSES_ROOT, NULL, 0, KEY_READ, &key );
        res = RegOpenKeyExA( key, sub.local8Bit(), 0, KEY_READ, &subkey );
        res = RegQueryValueExA( subkey, NULL, NULL, &type, data, &size );
        command = QString::fromLocal8Bit( (const char*) data ) + "\\command";
        size = 255;
        res = RegOpenKeyExA( subkey, command.local8Bit(), 0, KEY_READ, &subkey );
        res = RegQueryValueExA( subkey, NULL, NULL, &type, data, &size );
        command = QString::fromLocal8Bit( (const char*) data );
    }

    res = RegCloseKey( subkey );
    res = RegCloseKey( key );

    QProcess process( command + " www.trolltech.com/products/download/eval/evaluation.html" );
    if ( !process.start() )
        QMessageBox::information( this, "Register Qt", "Launching your web browser failed.\n"
        "To register Qt, point your browser to www.trolltech.com/products/download/eval/evaluation.html" );
#endif
}

void MainWindow::showProperties( QObject *o )
{
    if ( !o->isWidgetType() ) {
        propertyEditor->setWidget( o, lastActiveFormWindow );
        if ( lastActiveFormWindow )
            hierarchyView->setFormWindow( lastActiveFormWindow, lastActiveFormWindow->mainContainer() );
        else
            hierarchyView->setFormWindow( 0, 0 );
        return;
    }
    QWidget *w = (QWidget*)o;
    setupHierarchyView();
    FormWindow *fw = (FormWindow*)isAFormWindowChild( w );
    if ( fw ) {
        propertyEditor->setWidget( w, fw );
        hierarchyView->setFormWindow( fw, w );
    } else {
        propertyEditor->setWidget( 0, 0 );
        hierarchyView->setFormWindow( 0, 0 );
    }

    if ( currentTool() == POINTER_TOOL && fw &&
         ( !qworkspace->activeWindow() || !qworkspace->activeWindow()->inherits( "SourceEditor" ) ) )
        fw->setFocus();
}

void MainWindow::resetTool()
{
    actionPointerTool->setOn( TRUE );
}

void MainWindow::updateProperties( QObject * )
{
    if ( propertyEditor )
        propertyEditor->refetchData();
}

bool MainWindow::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e || !o->isWidgetType() )
        return QMainWindow::eventFilter( o, e );

    QWidget *w = 0;
    bool passiveInteractor = WidgetFactory::isPassiveInteractor( o );
    switch ( e->type() ) {
    case QEvent::AccelOverride:
        if ( ( (QKeyEvent*)e )->key() == Key_F1 &&
             ( ( (QKeyEvent*)e )->state() & ShiftButton ) != ShiftButton ) {
            w = (QWidget*)o;
            while ( w ) {
                if ( w->inherits( "PropertyList" ) )
                    break;
                w = w->parentWidget( TRUE );
            }
            if ( w ) {
                propertyEditor->propertyList()->showCurrentWhatsThis();
                ( (QKeyEvent*)e )->accept();
                return TRUE;
            }
        }
        break;
    case QEvent::Accel:
        if ( ( ( (QKeyEvent*)e )->key() == Key_A ||
               ( (QKeyEvent*)e )->key() == Key_E ) &&
             ( (QKeyEvent*)e )->state() & ControlButton ) {
            if ( qWorkspace()->activeWindow() &&
                 qWorkspace()->activeWindow()->inherits( "SourceEditor" ) ) {
                ( (QKeyEvent*)e )->ignore();
                return TRUE;
            }
        }
        break;
    case QEvent::ContextMenu:
    case QEvent::MouseButtonPress:
        if ( o->inherits( "QDesignerPopupMenu" ) )
            break;
        if ( o && currentTool() == POINTER_TOOL && ( o->inherits( "QDesignerMenuBar" ) ||
                    o->inherits( "QDesignerToolBar" ) ||
                    ( o->inherits( "QComboBox") || o->inherits( "QToolButton" ) || o->inherits( "QDesignerToolBarSeparator" ) ) &&
                    o->parent() && o->parent()->inherits( "QDesignerToolBar" ) ) ) {
            QWidget *w = (QWidget*)o;
            if ( w->inherits( "QToolButton" ) || w->inherits( "QComboBox" ) || w->inherits( "QDesignerToolBarSeparator" ) )
                w = w->parentWidget();
            QWidget *pw = w->parentWidget();
            while ( pw ) {
                if ( pw->inherits( "FormWindow" ) ) {
                    ( (FormWindow*)pw )->emitShowProperties( w );
                    if ( !o->inherits( "QDesignerToolBar" ) )
                        return !o->inherits( "QToolButton" ) && !o->inherits( "QMenuBar" ) &&
                            !o->inherits( "QComboBox" ) && !o->inherits( "QDesignerToolBarSeparator" );
                }
                pw = pw->parentWidget();
            }
        }
        if ( o && ( o->inherits( "QDesignerToolBar" ) || o->inherits( "QDockWindowHandle" ) )
             && e->type() == QEvent::ContextMenu )
            break;
        if ( isAToolBarChild( o ) && currentTool() != CONNECT_TOOL )
            break;
        if ( o && o->inherits( "QSizeGrip" ) )
            break;
        if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
            break;
        if ( !w->hasFocus() )
            w->setFocus();
        if ( !passiveInteractor || currentTool() != ORDER_TOOL ) {
            if( e->type() == QEvent::ContextMenu ) {
                ( (FormWindow*)w )->handleContextMenu( (QContextMenuEvent*)e,
                                                       ( (FormWindow*)w )->designerWidget( o ) );
                return TRUE;
            } else {
                ( (FormWindow*)w )->handleMousePress( (QMouseEvent*)e,
                                                      ( (FormWindow*)w )->designerWidget( o ) );
            }
        }
        lastPressWidget = (QWidget*)o;
        if ( passiveInteractor )
            QTimer::singleShot( 0, formWindow(), SLOT( visibilityChanged() ) );
        if ( currentTool() == CONNECT_TOOL )
            return TRUE;
        return !passiveInteractor;
    case QEvent::MouseButtonRelease:
        lastPressWidget = 0;
        if ( isAToolBarChild( o )  && currentTool() != CONNECT_TOOL )
            break;
        if ( o && o->inherits( "QSizeGrip" ) )
            break;
        if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
            break;
        if ( !passiveInteractor )
            ( (FormWindow*)w )->handleMouseRelease( (QMouseEvent*)e, ( (FormWindow*)w )->designerWidget( o ) );
        if ( passiveInteractor ) {
            selectionChanged();
            QTimer::singleShot( 0, formWindow(), SLOT( visibilityChanged() ) );
        }
        return !passiveInteractor;
    case QEvent::MouseMove:
        if ( isAToolBarChild( o ) && currentTool() != CONNECT_TOOL )
            break;
        w = isAFormWindowChild( o );
        if ( lastPressWidget != (QWidget*)o && w &&
             !o->inherits( "SizeHandle" ) && !o->inherits( "OrderIndicator" ) &&
             !o->inherits( "QPopupMenu" ) && !o->inherits( "QMenuBar" ) &&
             !o->inherits( "QSizeGrip" ) )
            return TRUE;
        if ( o && o->inherits( "QSizeGrip" ) )
            break;
        if ( lastPressWidget != (QWidget*)o ||
             ( !w || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) ) )
            break;
        if ( !passiveInteractor )
            ( (FormWindow*)w )->handleMouseMove( (QMouseEvent*)e, ( (FormWindow*)w )->designerWidget( o ) );
        return !passiveInteractor;
    case QEvent::KeyPress:
        if ( ( (QKeyEvent*)e )->key() == Key_Escape && currentTool() != POINTER_TOOL ) {
            resetTool();
            return FALSE;
        }
#ifndef KOMMANDER
        if ( ( (QKeyEvent*)e )->key() == Key_Escape && incrementalSearch->hasFocus() ) {
            if ( qWorkspace()->activeWindow() && qWorkspace()->activeWindow()->inherits( "SourceEditor" ) ) {
                qWorkspace()->activeWindow()->setFocus();
                return TRUE;
            }
        }
#endif
        if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
            break;
        ( (FormWindow*)w )->handleKeyPress( (QKeyEvent*)e, ( (FormWindow*)w )->designerWidget( o ) );
        if ( ((QKeyEvent*)e)->isAccepted() )
            return TRUE;
        break;
    case QEvent::MouseButtonDblClick:
        if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) ) {
            if ( o && o->inherits( "QToolButton" ) && ( ( QToolButton*)o )->isOn() &&
                 o->parent() && o->parent()->inherits( "QToolBar" ) && formWindow() )
                formWindow()->setToolFixed();
            break;
        }
        if ( currentTool() == ORDER_TOOL ) {
            ( (FormWindow*)w )->handleMouseDblClick( (QMouseEvent*)e, ( (FormWindow*)w )->designerWidget( o ) );
            return TRUE;
        }
        if ( !WidgetFactory::isPassiveInteractor( o ) )
            return openEditor( ( (FormWindow*)w )->designerWidget( o ), (FormWindow*)w );
        return TRUE;
    case QEvent::KeyRelease:
        if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
            break;
        ( (FormWindow*)w )->handleKeyRelease( (QKeyEvent*)e, ( (FormWindow*)w )->designerWidget( o ) );
        if ( ((QKeyEvent*)e)->isAccepted() )
            return TRUE;
        break;
    case QEvent::Hide:
        if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
            break;
        if ( ( (FormWindow*)w )->isWidgetSelected( (QWidget*)o ) )
            ( (FormWindow*)w )->selectWidget( (QWidget*)o, FALSE );
        break;
    case QEvent::Enter:
    case QEvent::Leave:
        if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) || o->inherits( "QDesignerMenuBar" ) )
            break;
        return TRUE;
    case QEvent::Resize:
    case QEvent::Move:
        if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
            break;
        if ( WidgetFactory::layoutType( (QWidget*)o->parent() ) != WidgetFactory::NoLayout ) {
            ( (FormWindow*)w )->updateSelection( (QWidget*)o );
            if ( e->type() != QEvent::Resize )
                ( (FormWindow*)w )->updateChildSelections( (QWidget*)o );
        }
        break;
    case QEvent::Close:
#ifndef KOMMANDER
        if ( o->isWidgetType() && (QWidget*)o == (QWidget*)previewedForm ) {
            if ( lastActiveFormWindow && lastActiveFormWindow->project() ) {
                QStringList lst =
                    MetaDataBase::fakeProperty( lastActiveFormWindow, "database" ).toStringList();
                lastActiveFormWindow->project()->closeDatabase( lst[ 0 ] );
            }
        }
#endif
        break;
    case QEvent::DragEnter:
        if ( o == qWorkspace() || o == workspace() || o == workspace()->viewport() ) {
            workspace()->contentsDragEnterEvent( (QDragEnterEvent*)e );
            return TRUE;
        }
        break;
    case QEvent::DragMove:
        if ( o == qWorkspace() || o == workspace() || o == workspace()->viewport() ) {
            workspace()->contentsDragMoveEvent( (QDragMoveEvent*)e );
            return TRUE;
        }
        break;
    case QEvent::Drop:
        if ( o == qWorkspace() || o == workspace() || o == workspace()->viewport() ) {
            workspace()->contentsDropEvent( (QDropEvent*)e );
            return TRUE;
        }
        break;
    case QEvent::Show:
        if ( o != this )
            break;
        if ( ((QShowEvent*)e)->spontaneous() )
            break;
        QApplication::sendPostedEvents( qworkspace, QEvent::ChildInserted );
        showEvent( (QShowEvent*)e );
        checkTempFiles();
        return TRUE;
    case QEvent::Wheel:
        if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
            break;
        return TRUE;
    case QEvent::FocusIn:
	if ( !o->inherits( "FormWindow" ) && isAFormWindowChild( o ) )
	    return TRUE;
#ifndef KOMMANDER
	if ( o->inherits( "Editor" ) || o->inherits( "FormWindow" ) ) {
	    // QCustomEvent( 9999 ) is used by QListView to end in-place
	    // editing. In the case that one edits e.g. a class variable in
	    // the form definition view and clicks on another form in the
	    // designer, the in-place editor receives a focus out event. This
	    // event posts a QCustomEvent( 9999 ) to QListView to end in-place
	    // editing, which triggers a FormDefinitionView::save(). To make
	    // sure that we save() before the new active formwindow is set,
	    // post queued custom events of that type now.
	    if ( hierarchyView->formDefinitionView()->isRenaming() )
		QApplication::sendPostedEvents( hierarchyView->formDefinitionView()->
						child( 0, "QLineEdit" ) , 9999 );
	}
	if ( o->inherits( "Editor" ) ) {
	    QWidget *w = (QWidget*)o;
	    while ( w ) {
		if ( w->inherits( "SourceEditor" ) )
		    break;
		w = w->parentWidget( TRUE );
	    }
	    if ( w && w->inherits( "SourceEditor" ) )
		( (SourceEditor*)w )->checkTimeStamp();
	} else if ( o->inherits( "FormWindow" ) ) {
	    FormWindow *fw = (FormWindow*)o;
	    if ( fw->formFile() && fw->formFile()->editor() )
		fw->formFile()->editor()->checkTimeStamp();
	}
#endif
        break;
    case QEvent::FocusOut:
        if ( !o->inherits( "FormWindow" ) && isAFormWindowChild( o ) )
            return TRUE;
        break;
    default:
        return QMainWindow::eventFilter( o, e );
    }

    return QMainWindow::eventFilter( o, e );
}

QWidget *MainWindow::isAFormWindowChild( QObject *o ) const
{
    if ( o->parent() && o->parent()->inherits( "QWizard" ) && !o->inherits( "QPushButton" ) )
        return 0;
    while ( o ) {
        if ( o->inherits( "FormWindow" ) )
            return (QWidget*)o;
        o = o->parent();
    }
    return 0;
}

QWidget *MainWindow::isAToolBarChild( QObject *o ) const
{
    while ( o ) {
        if ( o->inherits( "QDesignerToolBar" ) )
            return (QWidget*)o;
        if ( o->inherits( "FormWindow" ) )
            return 0;
        o = o->parent();
    }
    return 0;
}

FormWindow *MainWindow::formWindow()
{
    if ( qworkspace->activeWindow() ) {
        FormWindow *fw = 0;
        if ( qworkspace->activeWindow()->inherits( "FormWindow" ) )
            fw = (FormWindow*)qworkspace->activeWindow();
        else if ( lastActiveFormWindow &&
                    qworkspace->windowList().find( lastActiveFormWindow ) != -1)
            fw = lastActiveFormWindow;
        return fw;
    }
    return 0;
}

#ifndef KOMMANDER
void MainWindow::emitProjectSignals()
{
    emit hasNonDummyProject( !currentProject->isDummy() );
    emit hasActiveWindowOrProject( !!qworkspace->activeWindow() || !currentProject->isDummy() );
}
#endif

void MainWindow::insertFormWindow( FormWindow *fw )
{
    if ( fw )
        QWhatsThis::add( fw, i18n("<b>The Form Window</b>"
                               "<p>Use the various tools to add widgets or to change the layout "
                               "and behavior of the components in the form. Select one or multiple "
                               "widgets to move them or lay them out. If a single widget is chosen it can "
                               "be resized using the resize handles.</p>"
                               "<p>Changes in the <b>Property Editor</b> are visible at design time, "
                               "and you can preview the form in different styles.</p>"
                               "<p>You can change the grid resolution, or turn the grid off in the "
                               "<b>Preferences</b> dialog from the <b>Edit</b> menu."
                               "<p>You can have several forms open, and all open forms are listed "
                               "in the <b>Form List</b>.") );

    connect( fw, SIGNAL( showProperties( QObject * ) ),
             this, SLOT( showProperties( QObject * ) ) );
    connect( fw, SIGNAL( updateProperties( QObject * ) ),
             this, SLOT( updateProperties( QObject * ) ) );
    connect( this, SIGNAL( currentToolChanged() ),
             fw, SLOT( currentToolChanged() ) );
    connect( fw, SIGNAL( selectionChanged() ),
             this, SLOT( selectionChanged() ) );
    connect( fw, SIGNAL( undoRedoChanged( bool, bool, const QString &, const QString & ) ),
             this, SLOT( updateUndoRedo( bool, bool, const QString &, const QString & ) ) );

#ifndef KOMMANDER
    if ( !mblockNewForms ) {
    } else {
        fw->setProject( currentProject );
    }
#endif
#ifndef KOMMANDER
    fw->setProject(currentProject);
#endif
    fw->show();
    fw->currentToolChanged();
    if ( fw->caption().isEmpty() && qstrlen( fw->name() )  )
        fw->setCaption( fw->name() );
    fw->mainContainer()->setCaption( fw->caption() );
    WidgetFactory::saveDefaultProperties( fw->mainContainer(),
                                          WidgetDatabase::
                                          idFromClassName( WidgetFactory::classNameOf( fw->mainContainer() ) ) );
    activeWindowChanged( fw );
    emit formWindowsChanged();
#ifndef KOMMANDER
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
        if ( e->project() == fw->project() )
            e->resetContext();
    }
#endif
}

#ifndef KOMMANDER
void MainWindow::createNewProject( const QString &lang )
{
#ifndef KOMMANDER
    Project *pro = new Project( "", "", projectSettingsPluginManager );
    pro->setLanguage( lang );


    if ( !openProjectSettings( pro ) ) {
        delete pro;
        return;
    }

    if ( !pro->isValid() ) {
        QMessageBox::information( this, i18n("New Project"), i18n("Cannot create an invalid project." ) );
        delete pro;
        return;
    }
    QAction *a = new QAction( pro->makeRelative( pro->fileName() ),
                              pro->makeRelative( pro->fileName() ), 0, actionGroupProjects, 0, TRUE );
    projects.insert( a, pro );
    addRecentlyOpened( pro->makeAbsolute( pro->fileName() ), recentlyProjects );
    projectSelected( a );
#endif
}
#endif


bool MainWindow::unregisterClient( FormWindow *w )
{
    propertyEditor->closed( w );
    objectHierarchy()->closed( w );
    if ( w == lastActiveFormWindow )
        lastActiveFormWindow = 0;

#ifndef KOMMANDER
    QPtrList<SourceEditor> waitingForDelete;
    waitingForDelete.setAutoDelete( TRUE );
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
        if ( e->object() == w )
            waitingForDelete.append( e );
    }
#endif
    if ( actionEditor->form() == w ) {
        actionEditor->setFormWindow( 0 );
        actionEditor->parentWidget()->hide();
    }

    return TRUE;
}

void MainWindow::activeWindowChanged( QWidget *w )
{
    QWidget *old = formWindow();
    if ( w && w->inherits( "FormWindow" ) ) {
        FormWindow *fw = (FormWindow*)w;
        lastActiveFormWindow = fw;
        lastActiveFormWindow->updateUndoInfo();
        emit hasActiveForm( TRUE );
        if ( formWindow() ) {
            formWindow()->emitShowProperties();
            emit formModified( formWindow()->commandHistory()->isModified() );
            if ( currentTool() != POINTER_TOOL )
                formWindow()->clearSelection();
        }
        workspace()->activeFormChanged( fw );
        setAppropriate( (QDockWindow*)actionEditor->parentWidget(), lastActiveFormWindow->mainContainer()->inherits( "QMainWindow" ) );
        if ( appropriate( (QDockWindow*)actionEditor->parentWidget() ) )
            actionEditor->parentWidget()->show();
        else
            actionEditor->parentWidget()->hide();

        actionEditor->setFormWindow( lastActiveFormWindow );
#ifndef KOMMANDER
        if ( wspace && fw->project() && fw->project() != currentProject ) {
            for ( QMap<QAction*, Project *>::Iterator it = projects.begin(); it != projects.end(); ++it ) {
                if ( *it == fw->project() ) {
                    projectSelected( it.key() );
                    break;
                }
            }
        }
#endif
        emit formWindowChanged();

    } else if ( w == propertyEditor ) {
        propertyEditor->resetFocus();
    } else if ( !lastActiveFormWindow ) {
        emit formWindowChanged();
        emit hasActiveForm( FALSE );
        actionEditUndo->setEnabled( FALSE );
        actionEditRedo->setEnabled( FALSE );
    }

    if ( !w ) {
        emit formWindowChanged();
        emit hasActiveForm( FALSE );
        propertyEditor->clear();
        hierarchyView->clear();
        updateUndoRedo( FALSE, FALSE, QString::null, QString::null );
    }

    selectionChanged();

#ifndef KOMMANDER
    if ( w && w->inherits( "SourceEditor" ) ) {
        SourceEditor *se = (SourceEditor*)w;
        if ( se->formWindow() &&
             lastActiveFormWindow != se->formWindow() ) {
            activeWindowChanged( se->formWindow() );
        }
        actionSearchFind->setEnabled( TRUE );
        actionSearchIncremetal->setEnabled( TRUE );
        actionSearchReplace->setEnabled( TRUE );
        actionSearchGotoLine->setEnabled( TRUE );
        incrementalSearch->setEnabled( TRUE );

        actionEditUndo->setEnabled( TRUE );
        actionEditRedo->setEnabled( TRUE );
        actionEditCut->setEnabled( TRUE );
        actionEditCopy->setEnabled( TRUE );
        actionEditPaste->setEnabled( TRUE );
        actionEditSelectAll->setEnabled( TRUE );
        actionEditUndo->setMenuText( i18n("&Undo" ) );
        actionEditUndo->setToolTip( textNoAccel( actionEditUndo->menuText()) );
        actionEditRedo->setMenuText( i18n("&Redo" ) );
        actionEditRedo->setToolTip( textNoAccel( actionEditRedo->menuText()) );
        if ( hierarchyView->sourceEditor() != w )
            hierarchyView->showClasses( se );
        actionEditor->setFormWindow( 0 );
        if ( wspace && se->project() && se->project() != currentProject ) {
            for ( QMap<QAction*, Project *>::Iterator it = projects.begin(); it != projects.end(); ++it ) {
                if ( *it == se->project() ) {
                    projectSelected( it.key() );
                    break;
                }
            }
        }
        workspace()->activeEditorChanged( se );
    } else {
        actionSearchFind->setEnabled( FALSE );
        actionSearchIncremetal->setEnabled( FALSE );
        actionSearchReplace->setEnabled( FALSE );
        actionSearchGotoLine->setEnabled( FALSE );
        incrementalSearch->setEnabled( FALSE );
    }
#endif

    if ( currentTool() == ORDER_TOOL && w != old )
        emit currentToolChanged();

    emit hasActiveWindow( !!qworkspace->activeWindow() );
}

void MainWindow::updateUndoRedo( bool undoAvailable, bool redoAvailable,
                                 const QString &undoCmd, const QString &redoCmd )
{
    actionEditUndo->setEnabled( undoAvailable );
    actionEditRedo->setEnabled( redoAvailable );
    if ( !undoCmd.isEmpty() )
        actionEditUndo->setMenuText( i18n("&Undo: %1" ).arg( undoCmd ) );
    else
        actionEditUndo->setMenuText( i18n("&Undo: Not Available" ) );
    if ( !redoCmd.isEmpty() )
        actionEditRedo->setMenuText( i18n("&Redo: %1" ).arg( redoCmd ) );
    else
        actionEditRedo->setMenuText( i18n("&Redo: Not Available" ) );

    actionEditUndo->setToolTip( textNoAccel( actionEditUndo->menuText()) );
    actionEditRedo->setToolTip( textNoAccel( actionEditRedo->menuText()) );

    if ( currentTool() == ORDER_TOOL ) {
        actionEditUndo->setEnabled( FALSE );
        actionEditRedo->setEnabled( FALSE );
    }
}

QWorkspace *MainWindow::qWorkspace() const
{
    return qworkspace;
}

void MainWindow::popupFormWindowMenu( const QPoint & gp, FormWindow *fw )
{
    QValueList<int> ids;
    QMap<QString, int> commands;

    setupRMBSpecialCommands( ids, commands, fw );
    setupRMBProperties( ids, commands, fw );

    qApp->processEvents();
    int r = rmbFormWindow->exec( gp );

    handleRMBProperties( r, commands, fw );
    handleRMBSpecialCommands( r, commands, fw );

    for ( QValueList<int>::Iterator i = ids.begin(); i != ids.end(); ++i )
        rmbFormWindow->removeItem( *i );
}

void MainWindow::popupWidgetMenu( const QPoint &gp, FormWindow * /*fw*/, QWidget * w)
{
    QValueList<int> ids;
    QMap<QString, int> commands;

    setupRMBSpecialCommands( ids, commands, w );
    setupRMBProperties( ids, commands, w );

    qApp->processEvents();
    int r = rmbWidgets->exec( gp );

    handleRMBProperties( r, commands, w );
    handleRMBSpecialCommands( r, commands, w );

    for ( QValueList<int>::Iterator i = ids.begin(); i != ids.end(); ++i )
        rmbWidgets->removeItem( *i );
}

void MainWindow::setupRMBProperties( QValueList<int> &ids, QMap<QString, int> &props, QWidget *w )
{
    const QMetaProperty* text = w->metaObject()->property( w->metaObject()->findProperty( "text", TRUE ), TRUE );
    if ( text && qstrcmp( text->type(), "QString") != 0 )
        text = 0;
    const QMetaProperty* title = w->metaObject()->property( w->metaObject()->findProperty( "title", TRUE ), TRUE );
    if ( title && qstrcmp( title->type(), "QString") != 0 )
        title = 0;
    const QMetaProperty* pagetitle =
        w->metaObject()->property( w->metaObject()->findProperty( "pageTitle", TRUE ), TRUE );
    if ( pagetitle && qstrcmp( pagetitle->type(), "QString") != 0 )
        pagetitle = 0;
    const QMetaProperty* pixmap =
        w->metaObject()->property( w->metaObject()->findProperty( "pixmap", TRUE ), TRUE );
    if ( pixmap && qstrcmp( pixmap->type(), "QPixmap") != 0 )
        pixmap = 0;

    if ( text && text->designable(w) ||
         title && title->designable(w) ||
         pagetitle && pagetitle->designable(w) ||
         pixmap && pixmap->designable(w) ) {
        int id = 0;
        if ( ids.isEmpty() )
            ids << rmbWidgets->insertSeparator(0);
        if ( pixmap && pixmap->designable(w) ) {
            ids << ( id = rmbWidgets->insertItem( i18n("Choose Pixmap..."), -1, 0) );
            props.insert( "pixmap", id );
        }
        if ( text && text->designable(w) && !w->inherits( "QTextEdit" ) ) {
            ids << ( id = rmbWidgets->insertItem( i18n("Edit Text..."), -1, 0) );
            props.insert( "text", id );
        }
        if ( title && title->designable(w) ) {
            ids << ( id = rmbWidgets->insertItem( i18n("Edit Title..."), -1, 0) );
            props.insert( "title", id );
        }
        if ( pagetitle && pagetitle->designable(w) ) {
            ids << ( id = rmbWidgets->insertItem( i18n("Edit Page Title..."), -1, 0) );
            props.insert( "pagetitle", id );
        }
    }
}

void MainWindow::setupRMBSpecialCommands( QValueList<int> &ids, QMap<QString, int> &commands, QWidget *w )
{
    int id;
    // KommanderWidget doesn't derive from QObject
    KommanderWidget *atw = dynamic_cast<KommanderWidget *>(w);
    if(atw)
    {
            if(ids.isEmpty())
                    ids << rmbWidgets->insertSeparator(0);

            ids << (id = rmbWidgets->insertItem( i18n("Edit Kommander Text..." ), -1, 0));

            commands.insert("assoc", id);
    }

    if ( w->inherits( "QTabWidget" ) ) {
        if ( ids.isEmpty() )
            ids << rmbWidgets->insertSeparator( 0 );
        if ( ( (QDesignerTabWidget*)w )->count() > 1) {
            ids << ( id = rmbWidgets->insertItem( i18n("Delete Page"), -1, 0 ) );
            commands.insert( "remove", id );
        }
        ids << ( id = rmbWidgets->insertItem( i18n("Add Page"), -1, 0 ) );
        commands.insert( "add", id );
    }
    if ( WidgetFactory::hasSpecialEditor( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) )
    {
        if ( ids.isEmpty() )
            ids << rmbWidgets->insertSeparator( 0 );
        ids << ( id = rmbWidgets->insertItem( i18n("Edit..."), -1, 0 ) );
        commands.insert( "edit", id );
    }
}

void MainWindow::setupRMBSpecialCommands( QValueList<int> &ids, QMap<QString, int> &commands, FormWindow *fw )
{
    int id;

    // KommanderWidget doesn't derive from QObject
    KommanderWidget *atw = dynamic_cast<KommanderWidget *>(fw->mainContainer());
    if(atw)
    {
            if(ids.isEmpty())
                    ids << rmbFormWindow->insertSeparator(0);

            ids << (id = rmbFormWindow->insertItem( i18n("Edit Kommander Text..." ), -1, 0));

            commands.insert("assoc", id);
    }

    if ( fw->mainContainer()->inherits( "QWizard" ) ) {
        if ( ids.isEmpty() )
            ids << rmbFormWindow->insertSeparator( 0 );

        if ( ( (QWizard*)fw->mainContainer() )->pageCount() > 1) {
            ids << ( id = rmbFormWindow->insertItem( i18n("Delete Page"), -1, 0 ) );
            commands.insert( "remove", id );
        }

        ids << ( id = rmbFormWindow->insertItem( i18n("Add Page"), -1, 0 ) );
        commands.insert( "add", id );

        ids << ( id = rmbFormWindow->insertItem( i18n("Edit Page Title..."), -1, 0 ) );
        commands.insert( "rename", id );

        ids << ( id = rmbFormWindow->insertItem( i18n("Edit Pages..."), -1, 0 ) );
        commands.insert( "edit", id );

    } else if ( fw->mainContainer()->inherits( "QMainWindow" ) ) {
        if ( ids.isEmpty() )
            ids << rmbFormWindow->insertSeparator( 0 );
        ids << ( id = rmbFormWindow->insertItem( i18n("Add Menu Item" ), -1, 0 ) );
        commands.insert( "add_menu_item", id );
        ids << ( id = rmbFormWindow->insertItem( i18n("Add Toolbar" ), -1, 0 ) );
        commands.insert( "add_toolbar", id );
    }
}

void MainWindow::handleRMBProperties( int id, QMap<QString, int> &props, QWidget *w )
{
    if ( id == props[ "text" ] ) {
        bool ok = FALSE;
        QString text;
        if ( w->inherits( "QTextView" ) || w->inherits( "QLabel" ) ) {
            text = TextEditor::getText( this, w->property("text").toString() );
            ok = !text.isEmpty();
        } else {
            text = KLineEditDlg::getText( i18n("Text"), i18n("New text:" ), w->property("text").toString(), &ok, this );
        }
        if ( ok ) {
            QString pn( i18n("Set the 'text' of '%1'" ).arg( w->name() ) );
            SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
                                                              "text", w->property( "text" ),
                                                              text, QString::null, QString::null );
            cmd->execute();
            formWindow()->commandHistory()->addCommand( cmd );
            MetaDataBase::setPropertyChanged( w, "text", TRUE );
        }
    } else if ( id == props[ "title" ] ) {
        bool ok = FALSE;
        QString title = KLineEditDlg::getText( i18n("Title"), i18n("New title:" ), w->property("title").toString(), &ok, this );
        if ( ok ) {
            QString pn( i18n("Set the 'title' of '%1'" ).arg( w->name() ) );
            SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
                                                              "title", w->property( "title" ),
                                                              title, QString::null, QString::null );
            cmd->execute();
            formWindow()->commandHistory()->addCommand( cmd );
            MetaDataBase::setPropertyChanged( w, "title", TRUE );
        }
    } else if ( id == props[ "pagetitle" ] ) {
        bool ok = FALSE;
        QString text = KLineEditDlg::getText( i18n("Page Title"), i18n("New page title:" ), w->property("pageTitle").toString(), &ok, this );
        if ( ok ) {
            QString pn( i18n("Set the 'pageTitle' of '%1'" ).arg( w->name() ) );
            SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
                                                              "pageTitle", w->property( "pageTitle" ),
                                                              text, QString::null, QString::null );
            cmd->execute();
            formWindow()->commandHistory()->addCommand( cmd );
            MetaDataBase::setPropertyChanged( w, "pageTitle", TRUE );
        }
    } else if ( id == props[ "pixmap" ] ) {
        QPixmap oldPix = w->property( "pixmap" ).toPixmap();
        QPixmap pix = qChoosePixmap( this, formWindow(), oldPix );
        if ( !pix.isNull() ) {
            QString pn( i18n("Set the 'pixmap' of '%1'" ).arg( w->name() ) );
            SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
                                                              "pixmap", w->property( "pixmap" ),
                                                              pix, QString::null, QString::null );
            cmd->execute();
            formWindow()->commandHistory()->addCommand( cmd );
            MetaDataBase::setPropertyChanged( w, "pixmap", TRUE );
        }
    }
}

void MainWindow::handleRMBSpecialCommands( int id, QMap<QString, int> &commands, QWidget *w )
{
    // associated text menu
    // we assume all widgets derive from KommanderWidget [MB02]
    if(id == commands["assoc"])
    {
         KommanderWidget *atw = dynamic_cast<KommanderWidget *>(w);
         if(!atw)
             return;

         AssocTextEditor *editor = new AssocTextEditor(w, atw, this, "AssocTextEditor", TRUE);
         QString caption = i18n("Edit Kommander Text for Widget \'%1\'").arg(w->name());
         editor->setCaption(caption);

         if(editor->exec())
         {
	     //set both the population text and associated text if changed
	     if( atw->associatedText() != editor->associatedText() )
	     {
		QString text = QString("Set the \'text association\' of \'%1\'").arg(w->name());

             SetPropertyCommand *cmd  = new SetPropertyCommand(text,
                                    formWindow(), w, propertyEditor,
                                    "associations", atw->associatedText(),
                                    editor->associatedText(), QString::null,
                                    QString::null, FALSE);

		 cmd->execute();
		 formWindow()->commandHistory()->addCommand(cmd);

		 MetaDataBase::setPropertyChanged( w, "associations", TRUE );
	     }
	     if( atw->populationText() != editor->populationText() )
	     {
		QString text = QString("Set the \'population text\' of \'%1\'").arg(w->name());

             SetPropertyCommand *cmd  = new SetPropertyCommand(text,
                                    formWindow(), w, propertyEditor,
                                    "populationText", atw->populationText(),
                                    editor->populationText(), QString::null,
                                    QString::null, FALSE);

		 cmd->execute();
		 formWindow()->commandHistory()->addCommand(cmd);

		 MetaDataBase::setPropertyChanged( w, "populationText", TRUE );
	     }
         }
         delete editor;
    }

    if ( w->inherits( "QTabWidget" ) ) {
        QTabWidget *tw = (QTabWidget*)w;
        if ( id == commands[ "add" ] ) {
            AddTabPageCommand *cmd = new AddTabPageCommand( i18n("Add Page to %1" ).arg( tw->name() ), formWindow(),
                                                            tw, "Tab" );
            formWindow()->commandHistory()->addCommand( cmd );
            cmd->execute();
        } else if ( id == commands[ "remove" ] ) {
            if ( tw->currentPage() ) {
                QDesignerTabWidget *dtw = (QDesignerTabWidget*)tw;
                DeleteTabPageCommand *cmd = new DeleteTabPageCommand( i18n("Delete Page %1 of %2" ).
                                                                      arg( dtw->pageTitle() ).arg( tw->name() ),
                                                                      formWindow(), tw, tw->currentPage() );
                formWindow()->commandHistory()->addCommand( cmd );
                cmd->execute();
            }
        }
    }
    if ( WidgetFactory::hasSpecialEditor( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) ) {
        if ( id == commands[ "edit" ] )
            WidgetFactory::editWidget( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ), this, w, formWindow() );
    }
}

void MainWindow::handleRMBSpecialCommands( int id, QMap<QString, int> &commands, FormWindow *fw )
{
    if(id == commands["assoc"])
    {
         KommanderWidget *atw = dynamic_cast<KommanderWidget *>(fw->mainContainer());
         if(!atw)
             return;

         AssocTextEditor *editor = new AssocTextEditor(fw->mainContainer(), atw, this, "AssocTextEditor", TRUE);
         QString caption = i18n("Edit Kommander Text for Widget \'%1\'").arg(fw->mainContainer()->name());
         editor->setCaption(caption);

         if(editor->exec())
         {
	     if( atw->associatedText() != editor->associatedText() )
	     {
		QString text = QString("Set the \'text association\' of \'%1\'")
						    .arg(fw->mainContainer()->name());

		 SetPropertyCommand *cmd  = new SetPropertyCommand(text,
					formWindow(), fw, propertyEditor,
					"associations", atw->associatedText(),
					editor->associatedText(), QString::null,
					QString::null, FALSE);

		 cmd->execute();
		 formWindow()->commandHistory()->addCommand(cmd);

		 MetaDataBase::setPropertyChanged( fw, "associations", TRUE );
	     }
	     if( atw->populationText() != editor->populationText() )
	     {
		QString text = QString("Set the \'population text\' of \'%1\'").arg(fw->name());

             SetPropertyCommand *cmd  = new SetPropertyCommand(text,
                                    formWindow(), fw, propertyEditor,
                                    "populationText", atw->populationText(),
                                    editor->populationText(), QString::null,
                                    QString::null, FALSE);

		 cmd->execute();
		 formWindow()->commandHistory()->addCommand(cmd);

		 MetaDataBase::setPropertyChanged( fw, "populationText", TRUE );
	     }
         }
         delete editor;
    }
    if ( fw->mainContainer()->inherits( "QWizard" ) ) {
        QWizard *wiz = (QWizard*)fw->mainContainer();
        if ( id == commands[ "add" ] ) {
            AddWizardPageCommand *cmd = new AddWizardPageCommand( i18n("Add Page to %1" ).arg( wiz->name() ), formWindow(),
                                                                  wiz, "Page" );
            formWindow()->commandHistory()->addCommand( cmd );
            cmd->execute();
        } else if ( id == commands[ "remove" ] ) {
            if ( wiz->currentPage() ) {
                QDesignerWizard *dw = (QDesignerWizard*)wiz;
                DeleteWizardPageCommand *cmd = new DeleteWizardPageCommand( i18n("Delete Page %1 of %2" ).
                                                                            arg( dw->pageTitle() ).arg( wiz->name() ),
                                                                            formWindow(), wiz, wiz->indexOf( wiz->currentPage() ) );
                formWindow()->commandHistory()->addCommand( cmd );
                cmd->execute();
            }
        } else if ( id == commands[ "edit" ] ) {
            WizardEditor *e = new WizardEditor( this, wiz, fw );
            e->exec();
            delete e;
        } else if ( id == commands[ "rename" ] ) {

            bool ok = FALSE;
            QDesignerWizard *dw = (QDesignerWizard*)wiz;
            QString text = KLineEditDlg::getText( i18n("Page Title"), i18n("New page title:" ), dw->pageTitle(), &ok, this );
            if ( ok ) {
                QString pn( i18n("Rename page %1 of %2" ).arg( dw->pageTitle() ).arg( wiz->name() ) );
                RenameWizardPageCommand *cmd = new RenameWizardPageCommand( pn, formWindow()
                                                                            , wiz, wiz->indexOf( wiz->currentPage() ), text );
                formWindow()->commandHistory()->addCommand( cmd );
                cmd->execute();
            }
        }
    } else if ( fw->mainContainer()->inherits( "QMainWindow" ) ) {
        QMainWindow *mw = (QMainWindow*)fw->mainContainer();
        if ( id == commands[ "add_toolbar" ] ) {
            AddToolBarCommand *cmd = new AddToolBarCommand( i18n("Add Toolbar to '%1'" ).arg( formWindow()->name() ), formWindow(), mw );
            formWindow()->commandHistory()->addCommand( cmd );
            cmd->execute();
        } else if ( id == commands[ "add_menu_item" ] ) {
            AddMenuCommand *cmd = new AddMenuCommand( i18n("Add Menu to '%1'" ).arg( formWindow()->name() ), formWindow(), mw );
            formWindow()->commandHistory()->addCommand( cmd );
            cmd->execute();
        }
    }
}

void MainWindow::clipboardChanged()
{
    QString text( qApp->clipboard()->text() );
    QString start( "<!DOCTYPE UI-SELECTION>" );
    actionEditPaste->setEnabled( text.left( start.length() ) == start );
}

void MainWindow::selectionChanged()
{
    layoutChilds = FALSE;
    layoutSelected = FALSE;
    breakLayout = FALSE;
    if ( !formWindow() ) {
        actionEditCut->setEnabled( FALSE );
        actionEditCopy->setEnabled( FALSE );
        actionEditDelete->setEnabled( FALSE );
        actionEditAdjustSize->setEnabled( FALSE );
        actionEditHLayout->setEnabled( FALSE );
        actionEditVLayout->setEnabled( FALSE );
        actionEditSplitHorizontal->setEnabled( FALSE );
        actionEditSplitVertical->setEnabled( FALSE );
        actionEditGridLayout->setEnabled( FALSE );
        actionEditBreakLayout->setEnabled( FALSE );
        actionEditLower->setEnabled( FALSE );
        actionEditRaise->setEnabled( FALSE );
        actionEditAdjustSize->setEnabled( FALSE );
        return;
    }

    int selectedWidgets = formWindow()->numSelectedWidgets();
    bool enable = selectedWidgets > 0;
    actionEditCut->setEnabled( enable );
    actionEditCopy->setEnabled( enable );
    actionEditDelete->setEnabled( enable );
    actionEditLower->setEnabled( enable );
    actionEditRaise->setEnabled( enable );

    actionEditAdjustSize->setEnabled( FALSE );
    actionEditSplitHorizontal->setEnabled( FALSE );
    actionEditSplitVertical->setEnabled( FALSE );

    enable = FALSE;
    QWidgetList widgets = formWindow()->selectedWidgets();
    if ( selectedWidgets > 1 ) {
        int unlaidout = 0;
        int laidout = 0;
        for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
            if ( !w->parentWidget() || WidgetFactory::layoutType( w->parentWidget() ) == WidgetFactory::NoLayout )
                unlaidout++;
            else
                laidout++;
        }
        actionEditHLayout->setEnabled( unlaidout > 1 );
        actionEditVLayout->setEnabled( unlaidout > 1 );
        actionEditSplitHorizontal->setEnabled( unlaidout > 1 );
        actionEditSplitVertical->setEnabled( unlaidout > 1 );
        actionEditGridLayout->setEnabled( unlaidout > 1 );
        actionEditBreakLayout->setEnabled( laidout > 0 );
        actionEditAdjustSize->setEnabled( laidout > 0 );
        layoutSelected = unlaidout > 1;
        breakLayout = laidout > 0;
    } else if ( selectedWidgets == 1 ) {
        QWidget *w = widgets.first();
        bool isContainer = WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) ||
                           w == formWindow()->mainContainer();
        actionEditAdjustSize->setEnabled( !w->parentWidget() ||
                                          WidgetFactory::layoutType( w->parentWidget() ) == WidgetFactory::NoLayout );

        if ( !isContainer ) {
            actionEditHLayout->setEnabled( FALSE );
            actionEditVLayout->setEnabled( FALSE );
            actionEditGridLayout->setEnabled( FALSE );
            if ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout ) {
                actionEditBreakLayout->setEnabled( !isAToolBarChild( w ) );
                breakLayout = TRUE;
            } else {
                actionEditBreakLayout->setEnabled( FALSE );
            }
        } else {
            if ( WidgetFactory::layoutType( w ) == WidgetFactory::NoLayout ) {
                if ( !formWindow()->hasInsertedChildren( w ) ) {
                    actionEditHLayout->setEnabled( FALSE );
                    actionEditVLayout->setEnabled( FALSE );
                    actionEditGridLayout->setEnabled( FALSE );
                    actionEditBreakLayout->setEnabled( FALSE );
                } else {
                    actionEditHLayout->setEnabled( TRUE );
                    actionEditVLayout->setEnabled( TRUE );
                    actionEditGridLayout->setEnabled( TRUE );
                    actionEditBreakLayout->setEnabled( FALSE );
                    layoutChilds = TRUE;
                }
                if ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout ) {
                    actionEditBreakLayout->setEnabled( !isAToolBarChild( w ) );
                    breakLayout = TRUE;
                }
            } else {
                actionEditHLayout->setEnabled( FALSE );
                actionEditVLayout->setEnabled( FALSE );
                actionEditGridLayout->setEnabled( FALSE );
                actionEditBreakLayout->setEnabled( !isAToolBarChild( w ) );
                breakLayout = TRUE;
            }
        }
    } else if ( selectedWidgets == 0 && formWindow() ) {
        actionEditAdjustSize->setEnabled( TRUE );
        QWidget *w = formWindow()->mainContainer();
        if ( WidgetFactory::layoutType( w ) == WidgetFactory::NoLayout ) {
            if ( !formWindow()->hasInsertedChildren( w ) ) {
                actionEditHLayout->setEnabled( FALSE );
                actionEditVLayout->setEnabled( FALSE );
                actionEditGridLayout->setEnabled( FALSE );
                actionEditBreakLayout->setEnabled( FALSE );
            } else {
                actionEditHLayout->setEnabled( TRUE );
                actionEditVLayout->setEnabled( TRUE );
                actionEditGridLayout->setEnabled( TRUE );
                actionEditBreakLayout->setEnabled( FALSE );
                layoutChilds = TRUE;
            }
        } else {
            actionEditHLayout->setEnabled( FALSE );
            actionEditVLayout->setEnabled( FALSE );
            actionEditGridLayout->setEnabled( FALSE );
            actionEditBreakLayout->setEnabled( TRUE );
            breakLayout = TRUE;
        }
    } else {
        actionEditHLayout->setEnabled( FALSE );
        actionEditVLayout->setEnabled( FALSE );
        actionEditGridLayout->setEnabled( FALSE );
        actionEditBreakLayout->setEnabled( FALSE );
    }
}

static QString fixArgs( const QString &s2 )
{
    QString s = s2;
    return s.replace( QRegExp( "," ), ";" );
}

void MainWindow::writeConfig()
{
    QSettings config;

    // No search path for unix, only needs application name
#ifndef KOMMANDER
    config.insertSearchPath( QSettings::Windows, "/Trolltech" );
#endif

    QString keybase = DesignerApplication::settingsKey();
    config.writeEntry( keybase + "RestoreWorkspace", restoreConfig );
    config.writeEntry( keybase + "SplashScreen", splashScreen );
    config.writeEntry( keybase + "DocPath", docPath );
    config.writeEntry( keybase + "FileFilter", fileFilter );
    config.writeEntry( keybase + "TemplatePath", templPath );
    config.writeEntry( keybase + "RecentlyOpenedFiles", recentlyFiles, ',' );
#ifndef KOMMANDER
    config.writeEntry( keybase + "RecentlyOpenedProjects", recentlyProjects, ',' );
#endif
#ifndef KOMMANDER
    config.writeEntry( keybase + "DatabaseAutoEdit", databaseAutoEdit );
#endif

    config.writeEntry( keybase + "Grid/Snap", snGrid );
    config.writeEntry( keybase + "Grid/Show", sGrid );
    config.writeEntry( keybase + "Grid/x", grid().x() );
    config.writeEntry( keybase + "Grid/y", grid().y() );

    config.writeEntry( keybase + "Background/UsePixmap", backPix );
    config.writeEntry( keybase + "Background/Color", (int)qworkspace->backgroundColor().rgb() );
    if ( qworkspace->backgroundPixmap() )
        qworkspace->backgroundPixmap()->save( QDir::home().absPath() + "/.designer/" + "background.xpm", "XPM" );

    config.writeEntry( keybase + "Geometries/MainwindowX", x() );
    config.writeEntry( keybase + "Geometries/MainwindowY", y() );
    config.writeEntry( keybase + "Geometries/MainwindowWidth", width() );
    config.writeEntry( keybase + "Geometries/MainwindowHeight", height() );
    config.writeEntry( keybase + "Geometries/MainwindowMaximized", isMaximized() );
    config.writeEntry( keybase + "Geometries/PropertyEditorX", propertyEditor->parentWidget()->x() );
    config.writeEntry( keybase + "Geometries/PropertyEditorY", propertyEditor->parentWidget()->y() );
    config.writeEntry( keybase + "Geometries/PropertyEditorWidth", propertyEditor->parentWidget()->width() );
    config.writeEntry( keybase + "Geometries/PropertyEditorHeight", propertyEditor->parentWidget()->height() );
    config.writeEntry( keybase + "Geometries/HierarchyViewX", hierarchyView->parentWidget()->x() );
    config.writeEntry( keybase + "Geometries/HierarchyViewY", hierarchyView->parentWidget()->y() );
    config.writeEntry( keybase + "Geometries/HierarchyViewWidth", hierarchyView->parentWidget()->width() );
    config.writeEntry( keybase + "Geometries/HierarchyViewHeight", hierarchyView->parentWidget()->height() );
    config.writeEntry( keybase + "Geometries/WorkspaceX", wspace->parentWidget()->x() );
    config.writeEntry( keybase + "Geometries/WorkspaceY", wspace->parentWidget()->y() );
    config.writeEntry( keybase + "Geometries/WorkspaceWidth", wspace->parentWidget()->width() );
    config.writeEntry( keybase + "Geometries/WorkspaceHeight", wspace->parentWidget()->height() );

    config.writeEntry( keybase + "View/TextLabels", usesTextLabel() );
    config.writeEntry( keybase + "View/BigIcons", usesBigPixmaps() );

    QString fn = QDir::homeDirPath() + "/.designerrctb2";
    QFile f( fn );
    f.open( IO_WriteOnly );
    QTextStream ts( &f );
    ts << *this;
    f.close();

    QPtrList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();
    config.writeEntry( keybase + "CustomWidgets/num", (int)lst->count() );
    int j = 0;
    QDir::home().mkdir( ".designer" );
    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
        QStringList l;
        l << w->className;
        l << w->includeFile;
        l << QString::number( (int)w->includePolicy );
        l << QString::number( w->sizeHint.width() );
        l << QString::number( w->sizeHint.height() );
        l << QString::number( w->lstSignals.count() );
        for ( QValueList<QCString>::Iterator it = w->lstSignals.begin(); it != w->lstSignals.end(); ++it )
            l << QString( fixArgs( *it ) );
        l << QString::number( w->lstSlots.count() );
        for ( QValueList<MetaDataBase::Slot>::Iterator it2 = w->lstSlots.begin(); it2 != w->lstSlots.end(); ++it2 ) {
            l << fixArgs( (*it2).slot );
            l << (*it2).access;
        }
        l << QString::number( w->lstProperties.count() );
        for ( QValueList<MetaDataBase::Property>::Iterator it3 = w->lstProperties.begin(); it3 != w->lstProperties.end(); ++it3 ) {
            l << (*it3).property;
            l << (*it3).type;
        }
        l << QString::number( size_type_to_int( w->sizePolicy.horData() ) );
        l << QString::number( size_type_to_int( w->sizePolicy.verData() ) );
        l << QString::number( (int)w->isContainer );
        config.writeEntry( keybase + "CustomWidgets/Widget" + QString::number( j++ ), l, ',' );
        w->pixmap->save( QDir::home().absPath() + "/.designer/" + w->className, "XPM" );
    }
}

static QString fixArgs2( const QString &s2 )
{
    QString s = s2;
    return s.replace( QRegExp( ";" ), "," );
}

void MainWindow::readConfig()
{
    QString keybase = DesignerApplication::settingsKey();
    QSettings config;
    config.insertSearchPath( QSettings::Windows, "/Trolltech" );

    bool ok;
    restoreConfig = config.readBoolEntry( keybase + "RestoreWorkspace", TRUE, &ok );
    if ( !ok ) {
        readOldConfig();
        return;
    }
    docPath = config.readEntry( keybase + "DocPath", docPath );
    fileFilter = config.readEntry( keybase + "FileFilter", fileFilter );
    templPath = config.readEntry( keybase + "TemplatePath", QString::null );
    databaseAutoEdit = config.readBoolEntry( keybase + "DatabaseAutoEdit", databaseAutoEdit );
    int num;

    if ( restoreConfig ) {
        splashScreen = config.readBoolEntry( keybase + "SplashScreen", TRUE );
        recentlyFiles = config.readListEntry( keybase + "RecentlyOpenedFiles", ',' );
#ifndef KOMMANDER
        recentlyProjects = config.readListEntry( keybase + "RecentlyOpenedProjects", ',' );
#endif

        backPix = config.readBoolEntry( keybase + "Background/UsePixmap", TRUE );
        if ( backPix ) {
            QPixmap pix;
            pix.load( QDir::home().absPath() + "/.designer/" + "background.xpm" );
            if ( !pix.isNull() )
                qworkspace->setBackgroundPixmap( pix );
        } else {
            qworkspace->setBackgroundColor( QColor( (QRgb)config.readNumEntry( keybase + "Background/Color" ) ) );
        }

        sGrid = config.readBoolEntry( keybase + "Grid/Show", TRUE );
        snGrid = config.readBoolEntry( keybase + "Grid/Snap", TRUE );
        grd.setX( config.readNumEntry( keybase + "Grid/x", 10 ) );
        grd.setY( config.readNumEntry( keybase + "Grid/y", 10 ) );

        if ( !config.readBoolEntry( DesignerApplication::settingsKey() + "Geometries/MainwindowMaximized", FALSE ) ) {
            QRect r( pos(), size() );
            r.setX( config.readNumEntry( keybase + "Geometries/MainwindowX", r.x() ) );
            r.setY( config.readNumEntry( keybase + "Geometries/MainwindowY", r.y() ) );
            r.setWidth( config.readNumEntry( keybase + "Geometries/MainwindowWidth", r.width() ) );
            r.setHeight( config.readNumEntry( keybase + "Geometries/MainwindowHeight", r.height() ) );

            QRect desk = QApplication::desktop()->geometry();
            QRect inter = desk.intersect( r );
            resize( r.size() );
            if ( inter.width() * inter.height() > ( r.width() * r.height() / 20 ) ) {
                move( r.topLeft() );
            }
        }

        setUsesTextLabel( config.readBoolEntry( keybase + "View/TextLabels", FALSE ) );
        setUsesBigPixmaps( FALSE /*config.readBoolEntry( "BigIcons", FALSE )*/ ); // ### disabled for now
    }

    num = config.readNumEntry( keybase + "CustomWidgets/num" );
    for ( int j = 0; j < num; ++j ) {
        MetaDataBase::CustomWidget *w = new MetaDataBase::CustomWidget;
        QStringList l = config.readListEntry( keybase + "CustomWidgets/Widget" + QString::number( j ), ',' );
        w->className = l[ 0 ];
        w->includeFile = l[ 1 ];
        w->includePolicy = (MetaDataBase::CustomWidget::IncludePolicy)l[ 2 ].toInt();
        w->sizeHint.setWidth( l[ 3 ].toInt() );
        w->sizeHint.setHeight( l[ 4 ].toInt() );
        uint c = 5;
        if ( l.count() > c ) {
            int numSignals = l[ c ].toInt();
            c++;
            for ( int i = 0; i < numSignals; ++i, c++ )
                w->lstSignals.append( fixArgs2( l[ c ] ).latin1() );
        }
        if ( l.count() > c ) {
            int numSlots = l[ c ].toInt();
            c++;
            for ( int i = 0; i < numSlots; ++i ) {
                MetaDataBase::Slot slot;
                slot.slot = fixArgs2( l[ c ] );
                c++;
                slot.access = l[ c ];
                c++;
                w->lstSlots.append( slot );
            }
        }
        if ( l.count() > c ) {
            int numProperties = l[ c ].toInt();
            c++;
            for ( int i = 0; i < numProperties; ++i ) {
                MetaDataBase::Property prop;
                prop.property = l[ c ];
                c++;
                prop.type = l[ c ];
                c++;
                w->lstProperties.append( prop );
            }
        } if ( l.count() > c ) {
            QSizePolicy::SizeType h, v;
             h = int_to_size_type( l[ c++ ].toInt() );
             v = int_to_size_type( l[ c++ ].toInt() );
             w->sizePolicy = QSizePolicy( h, v );
        }
        if ( l.count() > c ) {
            w->isContainer = (bool)l[ c++ ].toInt();
        }
        w->pixmap = new QPixmap( PixmapChooser::loadPixmap( QDir::home().absPath() + "/.designer/" + w->className ) );
        MetaDataBase::addCustomWidget( w );
    }
    if ( num > 0 )
        rebuildCustomWidgetGUI();

    if ( !restoreConfig )
        return;

#ifndef Q_WS_MAC
    /* I'm sorry to make this not happen on the Mac, but it seems to hang somewhere deep
       in QLayout, it gets into a very large loop - and seems it has to do with clever
       things the designer does ###Sam */
    QApplication::sendPostedEvents();
#endif
    QString fn = QDir::homeDirPath() + "/.designerrc" + "tb2";
    QFile f( fn );
    if ( f.open( IO_ReadOnly ) ) {
/*###        tbSettingsRead = TRUE;*/
        QTextStream ts( &f );
        ts >> *this;
        f.close();
    }

    rebuildCustomWidgetGUI();
}

void MainWindow::readOldConfig()
{
    QString fn = QDir::homeDirPath() + "/.designerrc";
    if ( !QFile::exists( fn ) ) {
        fn = "/etc/designerrc";
        if ( !QFile::exists( fn ) )
            return;
    }
    Config config( fn );
    config.setGroup( "General" );
    restoreConfig = config.readBoolEntry( "RestoreWorkspace", TRUE );
    docPath = config.readEntry( "DocPath", docPath );
    fileFilter = config.readEntry( "FileFilter", fileFilter );
    templPath = config.readEntry( "TemplatePath", QString::null );
    databaseAutoEdit = config.readBoolEntry( "DatabaseAutoEdit", databaseAutoEdit );
    int num;
    config.setGroup( "General" );
    if ( restoreConfig ) {
        splashScreen = config.readBoolEntry( "SplashScreen", TRUE );
        recentlyFiles = config.readListEntry( "RecentlyOpenedFiles", ',' );
#ifndef KOMMANDER
        recentlyProjects = config.readListEntry( "RecentlyOpenedProjects", ',' );
#endif
        config.setGroup( "Background" );
        backPix = config.readBoolEntry( "UsePixmap", TRUE );
        if ( backPix ) {
            QPixmap pix;
            pix.load( QDir::home().absPath() + "/.designer/" + "background.xpm" );
            if ( !pix.isNull() )
                qworkspace->setBackgroundPixmap( pix );
        } else {
            qworkspace->setBackgroundColor( QColor( (QRgb)config.readNumEntry( "Color" ) ) );
        }
        config.setGroup( "Grid" );
        sGrid = config.readBoolEntry( "Show", TRUE );
        snGrid = config.readBoolEntry( "Snap", TRUE );
        grd.setX( config.readNumEntry( "x", 10 ) );
        grd.setY( config.readNumEntry( "y", 10 ) );
        config.setGroup( "Geometries" );
        QRect r( pos(), size() );
        r.setX( config.readNumEntry( "MainwindowX", r.x() ) );
        r.setY( config.readNumEntry( "MainwindowY", r.y() ) );
        r.setWidth( config.readNumEntry( "MainwindowWidth", r.width() ) );
        r.setHeight( config.readNumEntry( "MainwindowHeight", r.height() ) );
        QRect desk = QApplication::desktop()->geometry();
        QRect inter = desk.intersect( r );
        resize( r.size() );
        if ( inter.width() * inter.height() > ( r.width() * r.height() / 20 ) ) {
            move( r.topLeft() );
        }

        config.setGroup( "View" );
        setUsesTextLabel( config.readBoolEntry( "TextLabels", FALSE ) );
        setUsesBigPixmaps( FALSE /*config.readBoolEntry( "BigIcons", FALSE )*/ ); // ### disabled for now
    }

    config.setGroup( "CustomWidgets" );
    num = config.readNumEntry( "num" );
    for ( int j = 0; j < num; ++j ) {
        MetaDataBase::CustomWidget *w = new MetaDataBase::CustomWidget;
        QStringList l = config.readListEntry( "Widget" + QString::number( j ), ',' );
        w->className = l[ 0 ];
        w->includeFile = l[ 1 ];
        w->includePolicy = (MetaDataBase::CustomWidget::IncludePolicy)l[ 2 ].toInt();
        w->sizeHint.setWidth( l[ 3 ].toInt() );
        w->sizeHint.setHeight( l[ 4 ].toInt() );
        uint c = 5;
        if ( l.count() > c ) {
            int numSignals = l[ c ].toInt();
            c++;
            for ( int i = 0; i < numSignals; ++i, c++ )
                w->lstSignals.append( fixArgs2( l[ c ] ).latin1() );
        }
        if ( l.count() > c ) {
            int numSlots = l[ c ].toInt();
            c++;
            for ( int i = 0; i < numSlots; ++i ) {
                MetaDataBase::Slot slot;
                slot.slot = fixArgs2( l[ c ] );
                c++;
                slot.access = l[ c ];
                c++;
                w->lstSlots.append( slot );
            }
        }
        if ( l.count() > c ) {
            int numProperties = l[ c ].toInt();
            c++;
            for ( int i = 0; i < numProperties; ++i ) {
                MetaDataBase::Property prop;
                prop.property = l[ c ];
                c++;
                prop.type = l[ c ];
                c++;
                w->lstProperties.append( prop );
            }
        } if ( l.count() > c ) {
            QSizePolicy::SizeType h, v;
             h = int_to_size_type( l[ c++ ].toInt() );
             v = int_to_size_type( l[ c++ ].toInt() );
             w->sizePolicy = QSizePolicy( h, v );
        }
        if ( l.count() > c ) {
            w->isContainer = (bool)l[ c++ ].toInt();
        }
        w->pixmap = new QPixmap( PixmapChooser::loadPixmap( QDir::home().absPath() + "/.designer/" + w->className ) );
        MetaDataBase::addCustomWidget( w );
    }
    if ( num > 0 )
        rebuildCustomWidgetGUI();

    if ( !restoreConfig )
        return;

    QApplication::sendPostedEvents();
    fn = QDir::homeDirPath() + "/.designerrc" + "tb2";
    QFile f( fn );
    if ( f.open( IO_ReadOnly ) ) {
        QTextStream ts( &f );
        ts >> *this;
        f.close();
    }

    rebuildCustomWidgetGUI();
}

HierarchyView *MainWindow::objectHierarchy() const
{
    if ( !hierarchyView )
        ( (MainWindow*)this )->setupHierarchyView();
    return hierarchyView;
}

QPopupMenu *MainWindow::setupNormalHierarchyMenu( QWidget *parent )
{
    QPopupMenu *menu = new QPopupMenu( parent );

    actionEditCut->addTo( menu );
    actionEditCopy->addTo( menu );
    actionEditPaste->addTo( menu );
    actionEditDelete->addTo( menu );

    return menu;
}

QPopupMenu *MainWindow::setupTabWidgetHierarchyMenu( QWidget *parent, const char *addSlot, const char *removeSlot )
{
    QPopupMenu *menu = new QPopupMenu( parent );

    menu->insertItem( i18n("Add Page" ), parent, addSlot );
    menu->insertItem( i18n("Delete Page" ), parent, removeSlot );
    menu->insertSeparator();
    actionEditCut->addTo( menu );
    actionEditCopy->addTo( menu );
    actionEditPaste->addTo( menu );
    actionEditDelete->addTo( menu );

    return menu;
}

void MainWindow::closeEvent( QCloseEvent *e )
{
    QWidgetList windows = qWorkspace()->windowList();
    QWidgetListIt wit( windows );
    while ( wit.current() ) {
	QWidget *w = wit.current();
	++wit;
	if ( w->inherits( "FormWindow" ) ) {
#ifndef KOMMANDER
	    if ( ( (FormWindow*)w )->formFile()->editor() )
		windows.removeRef( ( (FormWindow*)w )->formFile()->editor() );
#endif
	    if ( !( (FormWindow*)w )->formFile()->close() ) {
		e->ignore();
		return;
	    }
	}
#ifndef KOMMANDER
	else if ( w->inherits( "SourceEditor" ) ) {
	    if ( !( (SourceEditor*)w )->close() ) {
		e->ignore();
		return;
	    }
	}
#endif
    }

#ifndef KOMMANDER
    QMapConstIterator<QAction*, Project*> it = projects.begin();
    while( it != projects.end() ) {
        Project *pro = it.data();
        ++it;
        if ( pro->isModified() ) {
            switch ( QMessageBox::warning( this, i18n("Save Project Settings" ),
                                           i18n("Save changes to '%1'?" ).arg( pro->fileName() ),
                                           i18n("&Yes" ), i18n("&No" ), i18n("&Cancel" ), 0, 2 ) ) {
            case 0: // save
                pro->save();
                break;
            case 1: // don't save
                break;
            case 2: // cancel
                e->ignore();
                return;
            default:
                break;
            }
        }
    }
#endif

    writeConfig();
    hide();
    e->accept();

    if ( client ) {
        QDir home( QDir::homeDirPath() );
        home.remove( ".designerpid" );
    }
}

Workspace *MainWindow::workspace() const
{
    if ( !wspace )
        ( (MainWindow*)this )->setupWorkspace();
    return wspace;
}

PropertyEditor *MainWindow::propertyeditor() const
{
    if ( !propertyEditor )
        ( (MainWindow*)this )->setupPropertyEditor();
    return propertyEditor;
}

ActionEditor *MainWindow::actioneditor() const
{
    if ( !actionEditor )
        ( (MainWindow*)this )->setupActionEditor();
    return actionEditor;
}

bool MainWindow::openEditor( QWidget *w, FormWindow * )
{
    if ( WidgetFactory::hasSpecialEditor( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) ) {
        statusBar()->message( i18n("Edit %1..." ).arg( w->className() ) );
        WidgetFactory::editWidget( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ), this, w, formWindow() );
        statusBar()->clear();
        return TRUE;
    }

    const QMetaProperty* text = w->metaObject()->property( w->metaObject()->findProperty( "text", TRUE ), TRUE );
    const QMetaProperty* title = w->metaObject()->property( w->metaObject()->findProperty( "title", TRUE ), TRUE );
    if ( text && text->designable(w) ) {
        bool ok = FALSE;
        QString text;
        if ( w->inherits( "QTextView" ) || w->inherits( "QLabel" ) ) {
            text = TextEditor::getText( this, w->property("text").toString() );
            ok = !text.isEmpty();
        } else {
            text = KLineEditDlg::getText( i18n("Text"), i18n("New text:" ), w->property("text").toString(), &ok, this );
        }
        if ( ok ) {
            QString pn( i18n("Set the 'text' of '%2'" ).arg( w->name() ) );
            SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
                                                              "text", w->property( "text" ),
                                                              text, QString::null, QString::null );
            cmd->execute();
            formWindow()->commandHistory()->addCommand( cmd );
            MetaDataBase::setPropertyChanged( w, "text", TRUE );
        }
        return TRUE;
    }
    if ( title && title->designable(w) ) {
        bool ok = FALSE;
        QString text;
        text = KLineEditDlg::getText( i18n("Title"), i18n("New title:" ), w->property("title").toString(), &ok, this );
        if ( ok ) {
            QString pn( i18n("Set the 'title' of '%2'" ).arg( w->name() ) );
            SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
                                                              "title", w->property( "title" ),
                                                              text, QString::null, QString::null );
            cmd->execute();
            formWindow()->commandHistory()->addCommand( cmd );
            MetaDataBase::setPropertyChanged( w, "title", TRUE );
        }
        return TRUE;
    }

#ifndef KOMMANDER
    if ( !WidgetFactory::isPassiveInteractor( w ) )
        editSource();
#endif

    return TRUE;
}

void MainWindow::rebuildCustomWidgetGUI()
{
    customWidgetToolBar->clear();
    customWidgetMenu->clear();
    int count = 0;
    QPtrList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();

    actionToolsCustomWidget->addTo( customWidgetMenu );
    customWidgetMenu->insertSeparator();

    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
        QAction* a = new QAction( actionGroupTools, QString::number( w->id ).latin1() );
        a->setToggleAction( TRUE );
        a->setText( w->className );
        a->setIconSet( *w->pixmap );
        a->setStatusTip( i18n("Insert a %1 (custom widget)" ).arg(w->className) );
        a->setWhatsThis( i18n("<b>%1 (custom widget)</b>"
                            "<p>Click <b>Edit Custom Widgets...</b> in the <b>Tools|Custom</b> menu to "
                            "add and change custom widgets. You can add properties as well as "
                            "signals and slots to integrate them into Qt Designer, "
                            "and provide a pixmap which will be used to represent the widget on the form.</p>")
                         .arg(w->className) );

        a->addTo( customWidgetToolBar );
        a->addTo( customWidgetMenu);
        count++;
    }

    if ( count == 0 )
        customWidgetToolBar->hide();
    else
        customWidgetToolBar->show();
}

bool MainWindow::isCustomWidgetUsed( MetaDataBase::CustomWidget *wid )
{
    QWidgetList windows = qWorkspace()->windowList();
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
        if ( w->inherits( "FormWindow" ) ) {
            if ( ( (FormWindow*)w )->isCustomWidgetUsed( wid ) )
                return TRUE;
        }
    }
    return FALSE;
}

void MainWindow::setGrid( const QPoint &p )
{
    if ( p == grd )
        return;
    grd = p;
    QWidgetList windows = qWorkspace()->windowList();
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
        if ( !w->inherits( "FormWindow" ) )
            continue;
        ( (FormWindow*)w )->mainContainer()->update();
    }
}

void MainWindow::setShowGrid( bool b )
{
    if ( b == sGrid )
        return;
    sGrid = b;
    QWidgetList windows = qWorkspace()->windowList();
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
        if ( !w->inherits( "FormWindow" ) )
            continue;
        ( (FormWindow*)w )->mainContainer()->update();
    }
}

void MainWindow::setSnapGrid( bool b )
{
    if ( b == snGrid )
        return;
    snGrid = b;
}

QString MainWindow::documentationPath() const
{
    QString result = docPath;

    if ( docPath[0] == '$' ) {
        int fs = docPath.find('/');
        if ( fs == -1 )
            fs = docPath.find('\\');

        if ( fs > -1 ) {
            result = docPath.mid( 1, fs-1 );
        } else {
            fs=docPath.length();
            result = docPath.right(fs-1);
        }
        result = getenv(result.latin1()) + docPath.right( docPath.length()-fs );
    }

    return result;
}

void MainWindow::windowsMenuActivated( int id )
{
    QWidget* w = qworkspace->windowList().at( id );
    if ( w )
        w->setFocus();
}

#ifndef KOMMANDER
void MainWindow::projectSelected( QAction *a )
{
#ifndef KOMMANDER
    a->setOn( TRUE );
    if ( currentProject )
        currentProject->setActive( FALSE );
    Project *p = *projects.find( a );
    p->setActive( TRUE );
    if ( currentProject == p )
        return;
    currentProject = p;
    if ( wspace )
        wspace->setCurrentProject( currentProject );
#endif
    currentProject = projects.getFirst();
}
#endif

#ifndef KOMMANDER
void MainWindow::openProject( const QString &fn )
{
    for ( QMap<QAction*, Project*>::Iterator it = projects.begin(); it != projects.end(); ++it ) {
        if ( (*it)->fileName() == fn ) {
            projectSelected( it.key() );
            return;
        }
    }
    QApplication::setOverrideCursor( waitCursor );
    Project *pro = new Project( fn, "", projectSettingsPluginManager );
    pro->setModified( FALSE );
    QAction *a = new QAction( pro->projectName(), pro->projectName(), 0, actionGroupProjects, 0, TRUE );
    projects.insert( a, pro );
    projectSelected( a );
    QApplication::restoreOverrideCursor();
}
#endif

void MainWindow::checkTempFiles()
{
    QString s = QDir::homeDirPath() + "/.designer";
    QString baseName = s+ "/saved-form-";
#ifndef KOMMANDER
    if ( !QFile::exists( baseName + "1.ui" ) )
#else
    if ( !QFile::exists( baseName + "1.kmdr" ) )
#endif
        return;
    DesignerApplication::closeSplash();
    QDir d( s );
#ifndef KOMMANDER
    d.setNameFilter( "*.ui" );
#else
    d.setNameFilter( "*.kmdr" );
#endif
    QStringList lst = d.entryList();
    QApplication::restoreOverrideCursor();
    bool load = QMessageBox::information( this, i18n("Restoring the Last Session" ),
                                          i18n("Kommander found some temporary saved files, which were\n"
                                              "written when Kommander crashed last time. Do you want to\n"
                                              "load these files?" ), i18n("&Yes" ), i18n("&No" ) ) == 0;
    QApplication::setOverrideCursor( waitCursor );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        if ( load )
            openFormWindow( s + "/" + *it, FALSE );
        d.remove( *it );
    }
}

void MainWindow::showDialogHelp()
{
    QWidget *w = (QWidget*)sender();
    w = w->topLevelWidget();

    QString link = "designer-manual-12.html#";

    if ( w->inherits( "NewFormBase" ) )
        link += "dialog-file-new";
    else if ( w->inherits( "CreateTemplate" ) )
        link += "dialog-file-create-template";
    else if ( w->inherits( "EditSlotsBase" ) )
        link += "dialog-edit-slots";
    else if ( w->inherits( "ConnectionViewerBase" ) )
        link += "dialog-view-connections";
    else if ( w->inherits( "FormSettingsBase" ) )
        link += "dialog-edit-form-settings";
    else if ( w->inherits( "Preferences" ) )
        link += "dialog-edit-preferences";
    else if ( w->inherits( "PixmapCollectionEditor" ) )
        link += "dialog-image-collection";
    else if ( w->inherits( "DatabaseConnectionBase" ) )
        link += "dialog-edit-database-connections";
#ifndef KOMMANDER
    else if ( w->inherits( "ProjectSettingsBase" ) )
        link += "dialog-project-settings";
#endif
    else if ( w->inherits( "FindDialog" ) )
        link += "dialog-find-text";
    else if ( w->inherits( "ReplaceDialog" ) )
        link += "dialog-replace-text";
    else if ( w->inherits( "GotoLineDialog" ) )
        link += "dialog-go-to-line";
    else if ( w->inherits( "ConnectionEditorBase" ) )
        link += "dialog-edit-connections";
    else if ( w->inherits( "CustomWidgetEditorBase" ) )
        link += "dialog-edit-custom-widgets";
    else if ( w->inherits( "PaletteEditorBase" ) )
        link += "dialog-edit-palette";
    else if ( w->inherits( "ListBoxEditorBase" ) )
        link += "dialog-edit-listbox";
    else if ( w->inherits( "ListViewEditorBase" ) )
        link += "dialog-edit-listview";
    else if ( w->inherits( "IconViewEditorBase" ) )
        link += "dialog-edit-iconview";
    else if ( w->inherits( "TableEditorBase" ) )
        link += "dialog-edit-table";

    else {
        QMessageBox::information( this, i18n("Help" ),
                                  i18n("There is no help available for this dialog at the moment." ) );
        return;
    }

#ifdef Q_OS_WIN32
    QStringList lst;
    lst << assistantPath() << (QString( "d:" ) + link);
    QProcess proc( lst );
    proc.start();
#else
    if ( assistant )
        assistant->sendRequest( link+'\n');
#endif
}

#ifndef KOMMANDER
void MainWindow::setupActionManager()
{
#ifndef KOMMANDER
    actionPluginManager = new QPluginManager<ActionInterface>( IID_Action, QApplication::libraryPaths(), "/designer" );
#else
    actionPluginManager = new QPluginManager<ActionInterface>( IID_Action, QApplication::libraryPaths(), "" );
#endif


    QStringList lst = actionPluginManager->featureList();
    for ( QStringList::Iterator ait = lst.begin(); ait != lst.end(); ++ait ) {
        ActionInterface *iface = 0;
        actionPluginManager->queryInterface( *ait, &iface );
        if ( !iface )
            continue;

        iface->connectTo( desInterface );
        QAction *a = iface->create( *ait, this );
        if ( !a )
            continue;

        QString grp = iface->group( *ait );
        if ( grp.isEmpty() )
            grp = "3rd party actions";
        QPopupMenu *menu = 0;
        QToolBar *tb = 0;

        if ( !( menu = (QPopupMenu*)child( grp.latin1(), "QPopupMenu" ) ) ) {
            menu = new QPopupMenu( this, grp.latin1() );
	    QString i18ngrp = grp;
	    if (!grp.isEmpty())
	      i18ngrp = i18n(grp.utf8());
            menuBar()->insertItem( i18ngrp, menu );
        }
        if ( !( tb = (QToolBar*)child( grp.latin1(), "QToolBar" ) ) ) {
#if defined(HAVE_KDE)
            KToolBar *tb = new KToolBar( this );
            tb->setFullSize( FALSE );
#else
            tb = new QToolBar( this, grp.latin1() );
            tb->setCloseMode( QDockWindow::Undocked );
#endif
            addToolBar( tb, grp );
        }

        if ( iface->location( *ait, ActionInterface::Menu ) )
            a->addTo( menu );
        if ( iface->location( *ait, ActionInterface::Toolbar ) )
            a->addTo( tb );

        iface->release();
    }
}
#endif

#ifndef KOMMANDER
void MainWindow::editFunction( const QString &func, const QString &l, bool rereadSource )
{
    if ( !formWindow() )
        return;
    if ( !formWindow()->formFile()->hasFormCode() )
        formWindow()->formFile()->createFormCode();
    SourceEditor *editor = 0;
    QString lang = l;
    if ( lang.isEmpty() )
        lang = MetaDataBase::languageOfSlot( formWindow(), func.latin1() );
    if ( !MetaDataBase::hasEditor( lang ) )
        return;
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
        if ( e->language() == lang && e->object() == formWindow() ) {
            editor = e;
            break;
        }
    }
    if ( !editor ) {
        EditorInterface *eIface = 0;
        editorPluginManager->queryInterface( lang, &eIface );
        if ( !eIface )
            return;
        LanguageInterface *lIface = MetaDataBase::languageInterface( lang );
        if ( !lIface )
            return;
        editor = new SourceEditor( qWorkspace(), eIface, lIface );
        eIface->release();
        lIface->release();

        editor->setLanguage( lang );
        sourceEditors.append( editor );
    }
    if ( editor->object() != formWindow() )
	editor->setObject( formWindow(), formWindow()->project() );
#ifndef KOMMANDER
        editor->setObject( formWindow(), formWindow()->project() );
#else
//        editor->setObject( formWindow(), formWindow()->project() );
    ;
#endif // FIXME

    else if ( rereadSource )
        editor->refresh( FALSE );
    editor->show();
    editor->setFocus();
    editor->setFunction( func );
    emit editorChanged();
}
#endif

void MainWindow::setupRecentlyFilesMenu()
{
    recentlyFilesMenu->clear();
    int id = 0;
    for ( QStringList::Iterator it = recentlyFiles.begin(); it != recentlyFiles.end(); ++it ) {
        recentlyFilesMenu->insertItem( *it, id );
        id++;
    }
}

#ifndef KOMMANDER
void MainWindow::setupRecentlyProjectsMenu()
{
    recentlyProjectsMenu->clear();
    int id = 0;
    for ( QStringList::Iterator it = recentlyProjects.begin(); it != recentlyProjects.end(); ++it ) {
        recentlyProjectsMenu->insertItem( *it, id );
        id++;
    }
}
#endif

#ifndef KOMMANDER
QPtrList<DesignerProject> MainWindow::projectList() const
{
    QPtrList<DesignerProject> list;
    QMapConstIterator<QAction*, Project*> it = projects.begin();

    while( it != projects.end() ) {
        Project *p = it.data();
        ++it;
        list.append( p->iFace() );
    }

    list.append(projects.getFirst()->iFace());
    return list;
}
#endif

#ifndef KOMMANDER
QStringList MainWindow::projectNames() const
{
    QStringList res;
    for ( QMap<QAction*, Project* >::ConstIterator it = projects.begin(); it != projects.end(); ++it )
	res << (*it)->projectName();
        res << (*it)->projectName();
    res.append(projects.getFirst()->projectName());
    return res;
}
#endif

#ifndef KOMMANDER
QStringList MainWindow::projectFileNames() const
{
    QStringList res;
    for ( QMap<QAction*, Project* >::ConstIterator it = projects.begin(); it != projects.end(); ++it )
	res << (*it)->makeRelative( (*it)->fileName() );
        res << (*it)->makeRelative( (*it)->fileName() );
    res << projects.getFirst()->fileName();
    return res;
}
#endif

#ifndef KOMMANDER
Project *MainWindow::findProject( const QString &projectName ) const
{
    for ( QMap<QAction*, Project* >::ConstIterator it = projects.begin(); it != projects.end(); ++it ) {
        if ( (*it)->projectName() == projectName )
            return *it;
    }
    const QString projName = projects.getFirst()->projectName();

    if(projName == projectName)
            return projects.getFirst();
    return 0;
}
#endif

#ifndef KOMMANDER
void MainWindow::setCurrentProject( Project *pro )
{
    for ( QMap<QAction*, Project* >::ConstIterator it = projects.begin(); it != projects.end(); ++it ) {
        if ( *it == pro ) {
            projectSelected( it.key() );
            return;
        }
    }
    projectSelected(0);
}
#endif

#ifndef KOMMANDER
void MainWindow::setCurrentProjectByFilename( const QString& proFilename )
{
#ifndef KOMMANDER
    for ( QMap<QAction*, Project* >::ConstIterator it = projects.begin(); it != projects.end(); ++it ) {
        if ( (*it)->makeRelative( (*it)->fileName() ) == proFilename ) {
            projectSelected( it.key() );
            return;
        }
    }
#endif
    projectSelected(0);
}
#endif

void MainWindow::recentlyFilesMenuActivated( int id )
{
    if ( id != -1 ) {
        if ( !QFile::exists( *recentlyFiles.at( id ) ) ) {
            QMessageBox::warning( this, i18n("Open File" ),
                                  i18n("Could not open '%1'. File does not exist." ).
                                  arg( *recentlyFiles.at( id ) ) );
            recentlyFiles.remove( recentlyFiles.at( id ) );
            return;
        }
#ifndef KOMMANDER
        fileOpen( "", "", *recentlyFiles.at( id ) );
#else
        fileOpen("", *recentlyFiles.at(id));
#endif
        QString fn( *recentlyFiles.at( id ) );
        addRecentlyOpened( fn, recentlyFiles );
    }
}

#ifndef KOMMANDER
void MainWindow::recentlyProjectsMenuActivated( int id )
{
    if ( id != -1 ) {
        if ( !QFile::exists( *recentlyProjects.at( id ) ) ) {
            QMessageBox::warning( this, i18n("Open Project" ),
                                  i18n("Could not open '%1'. File does not exist." ).
                                  arg( *recentlyProjects.at( id ) ) );
            recentlyProjects.remove( recentlyProjects.at( id ) );
            return;
        }
        openProject( *recentlyProjects.at( id ) );
        QString fn( *recentlyProjects.at( id ) );
        addRecentlyOpened( fn, recentlyProjects );
    }
}
#endif

void MainWindow::addRecentlyOpened( const QString &fn, QStringList &lst )
{
    QFileInfo fi( fn );
    fi.convertToAbs();
    QString f = fi.filePath();
    if ( lst.find( f ) != lst.end() )
        lst.remove( f );
    if ( lst.count() >= 10 )
        lst.remove( lst.begin() );
    lst << f;
}

#ifndef KOMMANDER
TemplateWizardInterface * MainWindow::templateWizardInterface( const QString& className )
{
    TemplateWizardInterface* iface = 0;
    templateWizardPluginManager->queryInterface( className, & iface );
    return iface;
}
#endif

void MainWindow::setupPlugins()
{
    KommanderFactory::loadPlugins();
}

#ifndef KOMMANDER
void MainWindow::addPreferencesTab( QWidget *tab, const QString &title, QObject *receiver, const char *init_slot, const char *accept_slot )
{
    Tab t;
    t.w = tab;
    t.title = title;
    t.receiver = receiver;
    t.init_slot = init_slot;
    t.accept_slot = accept_slot;
    preferenceTabs << t;
}
#endif

#ifndef KOMMANDER
void MainWindow::addProjectTab( QWidget *tab, const QString &title, QObject *receiver, const char *init_slot, const char *accept_slot )
{
    Tab t;
    t.w = tab;
    t.title = title;
    t.receiver = receiver;
    t.init_slot = init_slot;
    t.accept_slot = accept_slot;
    projectTabs << t;
}
#endif

void MainWindow::setModified( bool b, QWidget *window )
{
    QWidget *w = window;
    while ( w ) {
	if ( w->inherits( "FormWindow" ) ) {
	    ( (FormWindow*)w )->modificationChanged( b );
	    return;
	}
#ifndef KOMMANDER
	else if ( w->inherits( "SourceEditor" ) ) {
	    FormWindow *fw = ( (SourceEditor*)w )->formWindow();
	    if ( fw ) {
		//fw->commandHistory()->setModified( b );
		//fw->modificationChanged( b );
		fw->formFile()->setModified( b, FormFile::WFormCode );
		wspace->update( fw->formFile() );
	    } else {
		wspace->update();
	    }
	    return;
	}
#endif
	w = w->parentWidget( TRUE );
        if ( w->inherits( "FormWindow" ) ) {
            ( (FormWindow*)w )->modificationChanged( b );
            return;
        }
#ifndef KOMMANDER
	else if ( w->inherits( "SourceEditor" ) ) {
            FormWindow *fw = ( (SourceEditor*)w )->formWindow();
            if ( fw ) {
                //fw->commandHistory()->setModified( b );
                //fw->modificationChanged( b );
                fw->formFile()->setModified( b, FormFile::WFormCode );
                wspace->update( fw->formFile() );
            } else {
                wspace->update();
            }
            return;
        }
#endif
        w = w->parentWidget( TRUE );
    }
}

#ifndef KOMMANDER
void MainWindow::editorClosed( SourceEditor *e )
{
    sourceEditors.take( sourceEditors.findRef( e ) );
}
#endif

#ifndef KOMMANDER
void MainWindow::slotsChanged()
{
    updateSlotsTimer->start( 0, TRUE );
}
#endif

#ifndef KOMMANDER
void MainWindow::doSlotsChanged()
{
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() )
	e->refresh( FALSE );
	hierarchyView->formDefinitionView()->refresh();
}
#endif

#ifndef KOMMANDER
void MainWindow::updateFunctionList()
{
    if ( !qWorkspace()->activeWindow() || !qWorkspace()->activeWindow()->inherits( "SourceEditor" ) )
        return;
    ( (SourceEditor*)qWorkspace()->activeWindow() )->save();
	hierarchyView->formDefinitionView()->refresh();
}
#endif

void MainWindow::updateWorkspace()
{
#ifndef KOMMANDER
    wspace->setCurrentProject( currentProject );
#endif
}


#ifndef KOMMANDER
void MainWindow::showDebugStep( QObject *o, int line )
{
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() )
        e->clearStep();
    if ( !o || line == -1 )
        return;
    showSourceLine( o, line, Step );
}

void MainWindow::showStackFrame( QObject *o, int line )
{
    if ( !o || line == -1 )
        return;
    showSourceLine( o, line, StackFrame );
}

void MainWindow::showErrorMessage( QObject *o, int errorLine, const QString &errorMessage )
{
    errorLine--; // ######
    QValueList<int> l;
    l << errorLine;
    QStringList l2;
    l2 << errorMessage;
    oWindow->setErrorMessages( l2, l, TRUE );
    showSourceLine( o, errorLine, Error );
}

void MainWindow::finishedRun()
{
    inDebugMode = FALSE;
    previewing = FALSE;
    debuggingForms.clear();
    enableAll( TRUE );
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
        if ( e->project() == currentProject )
            e->editorInterface()->setMode( EditorInterface::Editing );
        e->clearStackFrame();
    }
}

void MainWindow::enableAll( bool enable )
{
    menuBar()->setEnabled( enable );
    QObjectList *l = queryList( "QDockWindow" );
    for ( QObject *o = l->first(); o; o = l->next() ) {
        if ( o == wspace->parentWidget() || o == oWindow->parentWidget() || o == hierarchyView->parentWidget() )
            continue;
        ( (QWidget*)o )->setEnabled( enable );
    }
    delete l;
}

void MainWindow::showSourceLine( QObject *o, int line, LineMode lm )
{
    QWidgetList windows = qworkspace->windowList();
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
        FormWindow *fw = 0;
        SourceEditor *se = 0;
        SourceFile *sf = 0;
        if ( w->inherits( "FormWindow" ) ) {
            fw = (FormWindow*)fw;
        } else if ( w->inherits( "SourceEditor" ) ) {
            se = (SourceEditor*)w;
            if ( !se->object() )
                continue;
            if ( se->formWindow() )
                fw = se->formWindow();
            else
                sf = se->sourceFile();
        }

        if ( fw ) {
            if ( fw->project() != currentProject )
                continue;
            if ( QString( fw->name() ) == QString( o->name() ) ) {
                if ( se ) {
                    switch ( lm ) {
                    case Error:
                        se->editorInterface()->setError( line );
                        break;
                    case Step:
                        se->editorInterface()->setStep( line );
                        break;
                    case StackFrame:
                        se->editorInterface()->setStackFrame( line );
                        break;
                    }
                    return;
                } else {
                    fw->setFocus();
                    lastActiveFormWindow = fw;
                    qApp->processEvents();
                    se = editSource();
                    if ( se ) {
                        switch ( lm ) {
                        case Error:
                            se->editorInterface()->setError( line );
                            break;
                        case Step:
                            se->editorInterface()->setStep( line );
                            break;
                        case StackFrame:
                            se->editorInterface()->setStackFrame( line );
                            break;
                        }
                        return;
                    }
                }
            }
        } else if ( se ) {
            if ( o != sf )
                continue;
            switch ( lm ) {
            case Error:
                se->editorInterface()->setError( line );
                break;
            case Step:
                se->editorInterface()->setStep( line );
                break;
            case StackFrame:
                se->editorInterface()->setStackFrame( line );
                break;
            }
            return;
        }
    }

    if ( o->inherits( "SourceFile" ) ) {
        for ( QPtrListIterator<SourceFile> sources = currentProject->sourceFiles();
              sources.current(); ++sources ) {
            SourceFile* f = sources.current();
            if ( f == o ) {
                SourceEditor *se = editSource( f );
                if ( se ) {
                    switch ( lm ) {
                    case Error:
                        se->editorInterface()->setError( line );
                        break;
                    case Step:
                        se->editorInterface()->setStep( line );
                        break;
                    case StackFrame:
                        se->editorInterface()->setStackFrame( line );
                        break;
                    }
                }
                return;
            }
        }
    }

    mblockNewForms = TRUE;
    openFormWindow( currentProject->makeAbsolute( *qwf_forms->find( (QWidget*)o ) ) );
    qApp->processEvents(); // give all views the chance to get the formwindow
    SourceEditor *se = editSource();
    if ( se ) {
        switch ( lm ) {
        case Error:
            se->editorInterface()->setError( line );
            break;
        case Step:
            se->editorInterface()->setStep( line );
            break;
        case StackFrame:
            se->editorInterface()->setStackFrame( line );
            break;
        }
    }
    mblockNewForms = FALSE;
}
#endif


QWidget *MainWindow::findRealForm( QWidget *wid )
{
    QWidgetList windows = qWorkspace()->windowList();
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
        if ( QString( w->name() ) == QString( wid->name() ) )
            return w;
    }
    return 0;
}

void MainWindow::formNameChanged( FormWindow *fw )
{
#ifndef KOMMANDER
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
	if ( e->object() == fw )
	    e->refresh( TRUE );
	if ( e->project() == fw->project() )
	    e->resetContext();
    }
#endif
}

#ifndef KOMMANDER
void MainWindow::breakPointsChanged()
{
    if ( !inDebugMode )
        return;
    if ( !qWorkspace()->activeWindow() || !qWorkspace()->activeWindow()->inherits( "SourceEditor" ) )
        return;
    SourceEditor *e = (SourceEditor*)qWorkspace()->activeWindow();
    if ( !e->object() || !e->project() )
        return;
    if ( e->project() != currentProject )
        return;

    InterpreterInterface *iiface = 0;
    if ( interpreterPluginManager ) {
        QString lang = currentProject->language();
        iiface = 0;
        interpreterPluginManager->queryInterface( lang, &iiface );
        if ( !iiface )
            return;
    }

    e->saveBreakPoints();

    for ( QObject *o = debuggingForms.first(); o; o = debuggingForms.next() ) {
        if ( qstrcmp( o->name(), e->object()->name() ) == 0 ) {
            iiface->setBreakPoints( o, MetaDataBase::breakPoints( e->object() ) );
            break;
        }
    }

    for ( e = sourceEditors.first(); e; e = sourceEditors.next() ) {
        if ( e->project() == currentProject && e->sourceFile() ) {
            QValueList<int> bps = MetaDataBase::breakPoints( e->sourceFile() );
            iiface->setBreakPoints( e->object(), bps );
        }
    }

    iiface->release();
}
#endif

int MainWindow::currentLayoutDefaultSpacing() const
{
    if ( ( (MainWindow*)this )->formWindow() )
        return ( (MainWindow*)this )->formWindow()->layoutDefaultSpacing();
    return BOXLAYOUT_DEFAULT_SPACING;
}

int MainWindow::currentLayoutDefaultMargin() const
{
    if ( ( (MainWindow*)this )->formWindow() )
        return ( (MainWindow*)this )->formWindow()->layoutDefaultMargin();
    return BOXLAYOUT_DEFAULT_MARGIN;
}

#ifndef KOMMANDER
void MainWindow::saveAllBreakPoints()
{
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
        e->save();
        e->saveBreakPoints();
    }
}

void MainWindow::resetBreakPoints()
{
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() )
        e->resetBreakPoints();
}

SourceFile *MainWindow::sourceFile()
{
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
        if ( qworkspace->activeWindow() == e ) {
            if ( e->sourceFile() )
                return e->sourceFile();
        }
    }
    return 0;
}
#endif

#ifndef KOMMANDER
bool MainWindow::openProjectSettings( Project *pro )
{
    ProjectSettings dia( pro, this, 0, TRUE );
    SenderObject *senderObject = new SenderObject( designerInterface() );
    QValueList<Tab>::Iterator it;
    for ( it = projectTabs.begin(); it != projectTabs.end(); ++it ) {
        Tab t = *it;
        if ( t.title != pro->language() )
            continue;
        dia.tabWidget->addTab( t.w, t.title );
        if ( t.receiver ) {
            connect( dia.buttonOk, SIGNAL( clicked() ), senderObject, SLOT( emitAcceptSignal() ) );
            connect( senderObject, SIGNAL( acceptSignal( QUnknownInterface * ) ), t.receiver, t.accept_slot );
            connect( senderObject, SIGNAL( initSignal( QUnknownInterface * ) ), t.receiver, t.init_slot );
            senderObject->emitInitSignal();
            disconnect( senderObject, SIGNAL( initSignal( QUnknownInterface * ) ), t.receiver, t.init_slot );
        }
    }

    int res = dia.exec();

    delete senderObject;

    for ( it = projectTabs.begin(); it != projectTabs.end(); ++it ) {
        Tab t = *it;
        dia.tabWidget->removePage( t.w );
        t.w->reparent( 0, QPoint(0,0), FALSE );
    }

    return res == QDialog::Accepted;
}
#endif

#ifndef KOMMANDER
void MainWindow::popupProjectMenu( const QPoint &pos )
{

    projectMenu->exec( pos );
}
#endif

#ifndef KOMMANDER
QStringList MainWindow::sourceTemplates() const
{
    return sourceTemplatePluginManager->featureList();
}
#endif

#ifndef KOMMANDER
SourceTemplateInterface* MainWindow::sourceTemplateInterface( const QString& templ )
{
    SourceTemplateInterface *iface = 0;
    sourceTemplatePluginManager->queryInterface( templ, &iface);
    return iface;
}
#endif

QString MainWindow::whatsThisFrom( const QString &key )
{
    if ( menuHelpFile.isEmpty() ) {
        QString fn = getenv( "QTDIR" );
        fn += "/doc/html/designer-manual-10.html";
        QFile f( fn );
        if ( f.open( IO_ReadOnly ) ) {
            QTextStream ts( &f );
            menuHelpFile = ts.read();
        }
    }

    int i = menuHelpFile.find( key );
    if ( i == -1 )
        return QString::null;
    int start = i;
    int end = i;
    start = menuHelpFile.findRev( "<li>", i ) + 4;
    end = menuHelpFile.find( '\n', i ) - 1;
    return menuHelpFile.mid( start, end - start + 1 );
}
#include "mainwindow.moc"
