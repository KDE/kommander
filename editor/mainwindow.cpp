/**********************************************************************
 This file is based on Qt Designer, Copyright (C) 2000 Trolltech AS.  All rights reserved.

 This file may be distributed and/or modified under the terms of the
 GNU General Public License version 2 as published by the Free Software
 Foundation and appearing in the file LICENSE.GPL included in the
 packaging of this file.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

 See http://www.trolltech.com/gpl/ for GPL licensing information.

 Modified for Kommander:
  (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
  (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>

**********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "formwindow.h"
#include "globaldefs.h"
#include "hierarchyview.h"
#include "mainwindow.h"
#include "metadatabase.h"
#include "multilineeditorimpl.h"
#include "newformimpl.h"
#include "pixmapchooser.h"
#include "propertyeditor.h"
#include "resource.h"
#include "widgetdatabase.h"
#include "widgetfactory.h"
#include "wizardeditorimpl.h"
#include "workspace.h"
#include "actioneditorimpl.h"
#include "actiondnd.h"
#include "kommanderfactory.h"
#include "formfile.h"
#include "specials.h"
#include "assoctexteditorimpl.h"
#include "dialog.h"

#include <qaccel.h>
#include <qbuffer.h>
#include <qclipboard.h>
#include <qdir.h>
#include <qfeatures.h>
#include <qfile.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qmetaobject.h>
#include <qpixmap.h>
#include <qregexp.h>
#include <qstylefactory.h>
#include <qstyle.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <qvbox.h>
#include <qwhatsthis.h>
#include <qwidget.h>
#include <qwizard.h>
#include <qworkspace.h>


#include "qcompletionedit.h"
#include <stdlib.h>
#include "assistproc.h"

#include <kaction.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kconfig.h>
#include <kinputdialog.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kommanderversion.h>
#include <kommanderwidget.h>
#include <kprocess.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kurl.h>

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
  return "assistant";
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
    : KDockMainWindow( 0, "mainwindow", WType_TopLevel | WDestructiveClose | WGroupLeader ),
      grd( 10, 10 ), sGrid( TRUE ), snGrid( TRUE ), restoreConfig( TRUE ), splashScreen( TRUE ),
      docPath( "$QTDIR/doc/html" ), fileFilter( i18n("Kommander Files (*.kmdr)" ) ), client( asClient ),
      previewing( FALSE ), databaseAutoEdit( FALSE )
{
    init_colors();

    inDebugMode = TRUE; //debugging kommander

    setupPlugins();

    kapp->setMainWidget( this );
    self = this;

    prefDia = 0;
    windowMenu = 0;
    hierarchyView = 0;
    actionEditor = 0;
    wspace = 0;

    statusBar()->clear();
    statusBar()->addWidget(new QLabel(i18n("Welcome to the Kommander Editor"), statusBar()), 1);

    setupMDI();
    setupFileActions();
    setupEditActions();
    layoutToolBar = new KToolBar( this, "Layout" );
    ( (KToolBar*)layoutToolBar )->setFullSize( FALSE );
    addToolBar( layoutToolBar, i18n("Layout" ) );
    setupToolActions();
    setupLayoutActions();
    setupWorkspace();
    setupHierarchyView();
    setupPropertyEditor();
    setupActionEditor();
    setupRunActions();
    setupWindowActions();
    setupSettingsActions();
    setupHelpActions();
    setupRMBMenus();

    emit hasActiveForm( FALSE );
    emit hasActiveWindow( FALSE );

    lastPressWidget = 0;
    kapp->installEventFilter( this );

    QSize as( kapp->desktop()->size() );
    as -= QSize( 30, 30 );
    resize( QSize( 1200, 1000 ).boundedTo( as ) );

    connect( kapp->clipboard(), SIGNAL( dataChanged() ),
             this, SLOT( clipboardChanged() ) );
    clipboardChanged();
    layoutChilds = FALSE;
    layoutSelected = FALSE;
    breakLayout = FALSE;
    backPix = TRUE;

    readConfig();

    // hack to make WidgetFactory happy (so it knows QWidget and QDialog for resetting properties)
    QWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QWidget" ), this, 0, FALSE );
    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "Dialog" ), this, 0, FALSE );

    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QLabel" ), this, 0, FALSE );
    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QTabWidget" ), this, 0, FALSE );
    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QFrame" ), this, 0, FALSE );
    delete w;

    //FIXME    setAppropriate( (KDockWidget*)actionEditor->parentWidget(), FALSE );
    actionEditor->parentWidget()->hide();

    assistant = new AssistProc( this, "Internal Assistant", assistantPath() );

    statusBar()->setSizeGripEnabled( TRUE );

    SpecialInformation::registerSpecials();

}

MainWindow::~MainWindow()
{
  MetaDataBase::clearDataBase();
}

void MainWindow::setupMDI()
{
  qDebug("Setup MDI");
  KDockWidget* toolDock = createDockWidget("Workspace", QPixmap(), 0L, "main_workspace");
  QVBox *vbox = new QVBox(toolDock);
  vbox->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
  vbox->setMargin(1);
  vbox->setLineWidth(1);

  toolDock->setWidget(vbox);
  toolDock->setDockSite(KDockWidget::DockCorner);
  toolDock->setEnableDocking(KDockWidget::DockNone);
  setView(toolDock);
  setMainDockWidget(toolDock);

  qworkspace = new QWorkspace(vbox);
  qworkspace->setBackgroundMode(PaletteDark);
  qworkspace->setBackgroundPixmap(PixmapChooser::loadPixmap("background.png",
          PixmapChooser::NoSize));
  qworkspace->setPaletteBackgroundColor(QColor(238, 238, 238));
  qworkspace->setScrollBarsEnabled(TRUE);
  connect(qworkspace, SIGNAL(windowActivated(QWidget *)),
      this, SLOT(activeWindowChanged(QWidget *)));
  lastActiveFormWindow = 0;
  qworkspace->setAcceptDrops(TRUE);
}

void MainWindow::setupPropertyEditor()
{
  qDebug("Setup properties editor");
  KDockWidget *dw = createDockWidget("Property Editor", QPixmap(), 0, i18n("Properties"));
  propertyEditor = new PropertyEditor(dw);
  //addToolBar(dw, Qt::DockLeft);
  dw->setWidget(propertyEditor);
  dw->manualDock(getMainDockWidget(), KDockWidget::DockLeft, 20);
  dw->setCaption(i18n("Property Editor" ));
  QWhatsThis::add( propertyEditor,
                     i18n("<h2>The Property Editor</h2>"
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

}


void MainWindow::setupHierarchyView()
{
  qDebug("Setup hierarchy view");
  if (hierarchyView)
    return;
  KDockWidget *dw = createDockWidget("Object Explorer", QPixmap(), 0, i18n("Widgets"));
  hierarchyView = new HierarchyView(dw);
  dw->setWidget(hierarchyView);
  dw->setCaption(i18n("Object Explorer"));
  QWhatsThis::add(hierarchyView,
      i18n("<h2>The Object Explorer</h2>"
          "<p>The Object Explorer provides an overview of the relationships "
          "between the widgets in a form. You can use the clipboard functions using "
          "a context menu for each item in the view. It is also useful for selecting widgets "
          "in forms that have complex layouts.</p>"
          "<p>The columns can be resized by dragging the separator in the list's header.</p>"
          "<p>The second tab shows all the form's slots, class variables, includes, etc.</p>"));
}

void MainWindow::setupWorkspace()
{
  qDebug("Setup file list");
  KDockWidget *dw = createDockWidget("Dialogs", QPixmap(), 0, i18n("Dialogs"));
  QVBox *vbox = new QVBox(dw);
  QCompletionEdit *edit = new QCompletionEdit(vbox);
  QToolTip::add(edit, i18n("Start typing the buffer you want to switch to here (ALT+B)"));
  QAccel *a = new QAccel(this);
  a->connectItem(a->insertItem(ALT + Key_B), edit, SLOT(setFocus()));
  wspace = new Workspace(vbox, this);
  wspace->setBufferEdit(edit);
  dw->setWidget(vbox);
  dw->setCaption(i18n("Dialogs"));
  QWhatsThis::add(wspace, i18n("<h2>The File Overview Window</h2>"
          "<p>The File Overview Window displays all open dialogs.</p>"));

}

void MainWindow::setupActionEditor()
{
  qDebug("Setup action editor");
  KDockWidget *dw = createDockWidget("Action Editor", QPixmap(), 0, i18n("Actions"));
  actionEditor = new ActionEditor(dw);
  //addToolBar(dw, Qt::DockLeft);
  dw->setWidget(actionEditor);
  dw->setCaption(i18n("Action Editor"));
  QWhatsThis::add(actionEditor, i18n("<b>The Action Editor</b>"
          "<p>The Action Editor is used to add actions and action groups to "
          "a form, and to connect actions to slots. Actions and action "
          "groups can be dragged into menus and into toolbars, and may "
          "feature keyboard shortcuts and tooltips. If actions have pixmaps "
          "these are displayed on toolbar buttons and beside their names in " "menus.</p>"));
}

void MainWindow::setupRMBMenus()
{
    rmbWidgets = new QPopupMenu( this );
    actionEditCut->plug( rmbWidgets );
    actionEditCopy->plug( rmbWidgets );
    actionEditPaste->plug( rmbWidgets );
    actionEditDelete->plug( rmbWidgets );

    rmbWidgets->insertSeparator();
    actionEditAdjustSize->plug( rmbWidgets );
    actionEditHLayout->plug( rmbWidgets );
    actionEditVLayout->plug( rmbWidgets );
    actionEditGridLayout->plug( rmbWidgets );
    actionEditSplitHorizontal->plug( rmbWidgets );
    actionEditSplitVertical->plug( rmbWidgets );
    actionEditBreakLayout->plug( rmbWidgets );
    rmbWidgets->insertSeparator();
    actionEditConnections->plug( rmbWidgets );
    rmbFormWindow = new QPopupMenu( this );
    actionEditPaste->plug( rmbFormWindow );
    actionEditSelectAll->plug( rmbFormWindow );
    actionEditAccels->plug( rmbFormWindow );
    rmbFormWindow->insertSeparator();
    actionEditAdjustSize->plug( rmbFormWindow );
    actionEditHLayout->plug( rmbFormWindow );
    actionEditVLayout->plug( rmbFormWindow );
    actionEditGridLayout->plug( rmbFormWindow );
    actionEditBreakLayout->plug( rmbFormWindow );
    rmbFormWindow->insertSeparator();
    actionEditConnections->plug( rmbFormWindow );
    rmbFormWindow->insertSeparator();
    actionEditFormSettings->plug( rmbFormWindow );
}

void MainWindow::toolSelected()
{
  if (!(sender())->inherits("KAction"))
    return;
  actionCurrentTool = (KAction*)sender();
  emit currentToolChanged();
  if (formWindow())
      formWindow()->commandHistory()->emitUndoRedo();
}

int MainWindow::currentTool() const
{
    if (!actionCurrentTool)
      return POINTER_TOOL;
    return QString::fromLatin1(actionCurrentTool->name()).toInt();
}


void MainWindow::runForm()
{
  FormWindow* form = activeForm();
  if (!form || !form->formFile())
    return;
  if (form->formFile()->save(false))
  {
    KProcess process;
    process << "kmdr-executor";
    process << form->formFile()->fileName();
    process.start(KProcess::DontCare);
  }
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
  actionPointerTool->setChecked(TRUE);
  actionCurrentTool = actionPointerTool;
  emit currentToolChanged();
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
}



bool MainWindow::unregisterClient( FormWindow *w )
{
    propertyEditor->closed( w );
    objectHierarchy()->closed( w );
    if ( w == lastActiveFormWindow )
        lastActiveFormWindow = 0;

    if ( actionEditor->form() == w ) {
        actionEditor->setFormWindow( 0 );
        actionEditor->parentWidget()->hide();
    }

    return TRUE;
}

void MainWindow::activeWindowChanged( QWidget *w )
{
  QWidget *old = formWindow();
  if (w && w->inherits("FormWindow"))
  {
    FormWindow *fw = (FormWindow *) w;
    lastActiveFormWindow = fw;
    lastActiveFormWindow->updateUndoInfo();
    emit hasActiveForm(TRUE);
    if (formWindow())
    {
      formWindow()->emitShowProperties();
      emit formModified(formWindow()->commandHistory()->isModified());
      if (currentTool() != POINTER_TOOL)
        formWindow()->clearSelection();
    }
    workspace()->activeFormChanged(fw);
/*FIXME
    setAppropriate((QDockWindow *) actionEditor->parentWidget(),
        lastActiveFormWindow->mainContainer()->inherits("QMainWindow"));
    if (appropriate((QDockWindow *) actionEditor->parentWidget()))
      actionEditor->parentWidget()->show();
    else
      actionEditor->parentWidget()->hide();
  */
    actionEditor->setFormWindow(lastActiveFormWindow);
    emit formWindowChanged();

  } else if (w == propertyEditor)
  {
    propertyEditor->resetFocus();
  } else if (!lastActiveFormWindow)
  {
    emit formWindowChanged();
    emit hasActiveForm(FALSE);
    actionEditUndo->setEnabled(FALSE);
    actionEditRedo->setEnabled(FALSE);
  }

  if (!w)
  {
    emit formWindowChanged();
    emit hasActiveForm(FALSE);
    propertyEditor->clear();
    hierarchyView->clear();
    updateUndoRedo(FALSE, FALSE, QString::null, QString::null);
  }

  selectionChanged();

  if (currentTool() == ORDER_TOOL && w != old)
    emit currentToolChanged();

  emit hasActiveWindow(!!qworkspace->activeWindow());
}


void MainWindow::updateUndoRedo( bool undoAvailable, bool redoAvailable,
                                 const QString &undoCmd, const QString &redoCmd )
{
    actionEditUndo->setEnabled( undoAvailable );
    actionEditRedo->setEnabled( redoAvailable );
    if ( !undoCmd.isEmpty() )
        actionEditUndo->setText( i18n("&Undo: %1" ).arg( undoCmd ) );
    else
        actionEditUndo->setText( i18n("&Undo: Not Available" ) );
    if ( !redoCmd.isEmpty() )
        actionEditRedo->setText( i18n("&Redo: %1" ).arg( redoCmd ) );
    else
        actionEditRedo->setText( i18n("&Redo: Not Available" ) );

    actionEditUndo->setToolTip( textNoAccel( actionEditUndo->text()) );
    actionEditRedo->setToolTip( textNoAccel( actionEditRedo->text()) );

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

    kapp->processEvents();
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

    kapp->processEvents();
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
            text = KInputDialog::getText( i18n("Text"), i18n("New text:" ), w->property("text").toString(), &ok, this );
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
        QString title = KInputDialog::getText( i18n("Title"), i18n("New title:" ), w->property("title").toString(), &ok, this );
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
        QString text = KInputDialog::getText( i18n("Page Title"), i18n("New page title:" ), w->property("pageTitle").toString(), &ok, this );
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
         AssocTextEditor *editor = new AssocTextEditor(w, formWindow(), propertyEditor,
             this, "AssocTextEditor", true);
         editor->exec();
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
         AssocTextEditor *editor = new AssocTextEditor(fw->mainContainer(), formWindow(), propertyEditor,
             this, "AssocTextEditor", true);
         editor->exec();
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
            QString text = KInputDialog::getText( i18n("Page Title"), i18n("New page title:" ), dw->pageTitle(), &ok, this );
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
    QString text( kapp->clipboard()->text() );
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

void MainWindow::writeConfig()
{
  KConfig* config = kapp->config();

  config->setGroup("General");
  config->writeEntry("RestoreWorkspace", restoreConfig);
  config->writeEntry("SplashScreen", splashScreen);
  config->writeEntry("DocPath", docPath);
  config->writeEntry("FileFilter", fileFilter);
  config->writeEntry("TemplatePath", templPath);

  config->setGroup("Grid");
  config->writeEntry("Snap", snGrid);
  config->writeEntry("Show", sGrid);
  config->writeEntry("x", grid().x());
  config->writeEntry("y", grid().y());

  config->setGroup("Background");
  config->writeEntry("UsePixmap", backPix);
  config->writeEntry("Color", qworkspace->backgroundColor().name());

  /*
  config->setGroup("Geometry");
  config->writeEntry("MainWindow", size());
  config->writeEntry("MainWindowMax", isMaximized());
  */
  writeDockConfig(config, "Docks");

  config->setGroup("View");
  config->writeEntry("TextLabels", usesTextLabel());
  config->writeEntry("BigIcons", usesBigPixmaps());

  actionCollection()->writeShortcutSettings("Keys", config);

  config->deleteGroup("Recent Files");
  actionRecent->saveEntries(config, "Recent Files");
}

void MainWindow::readConfig()
{
  KConfig *config = kapp->config();

  config->setGroup("General");
  restoreConfig = config->readBoolEntry("RestoreWorkspace", true);
  splashScreen = config->readBoolEntry("SplashScreen", true);
  docPath = config->readEntry("DocPath", docPath);
  fileFilter = config->readEntry("FileFilter", fileFilter);
  templPath = config->readEntry("TemplatePath", QString::null);

  config->setGroup("Grid");
  sGrid = config->readBoolEntry("Snap", true);
  snGrid = config->readBoolEntry("Show", true);
  grd.setX(config->readNumEntry("x", 10));
  grd.setY(config->readNumEntry("y", 10));

  config->setGroup("Background");
  if (config->readBoolEntry("UsePixmap", true))
    qworkspace->setBackgroundPixmap(PixmapChooser::loadPixmap("background.png", PixmapChooser::NoSize));
  else
    qworkspace->setBackgroundColor(QColor(config->readEntry("Color", "#f0f0f0")).rgb());

  /*
  config->setGroup("Geometry");
  QSize winSize = size();
  resize(config->readSizeEntry("MainWindow", &winSize));
  if (config->readBoolEntry("MainWindowMax", false))
    showMaximized();
  */
  readDockConfig(config, "Docks");


  config->setGroup("View");
  setUsesTextLabel(config->readBoolEntry("TextLabels", false));
  setUsesBigPixmaps(config->readBoolEntry("BigIcons", false));

  actionCollection()->readShortcutSettings("Keys", config);

  actionRecent->loadEntries(config, "Recent Files");

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  for(int i = 0; i < args->count(); i++)
  {
    QFileInfo fi(args->url(i).path());
    if (fi.exists() && openFormWindow(args->url(i).path()))
      actionRecent->addURL(args->url(i));
  }
  args->clear();
}


HierarchyView *MainWindow::objectHierarchy() const
{
  if (!hierarchyView)
    ((MainWindow *) this)->setupHierarchyView();
  return hierarchyView;
}

QPopupMenu *MainWindow::setupNormalHierarchyMenu( QWidget *parent )
{
  QPopupMenu *menu = new QPopupMenu(parent);

  actionEditCut->plug(menu);
  actionEditCopy->plug(menu);
  actionEditPaste->plug(menu);
  actionEditDelete->plug(menu);

  return menu;
}

QPopupMenu *MainWindow::setupTabWidgetHierarchyMenu( QWidget *parent, const char *addSlot, const char *removeSlot )
{
    QPopupMenu *menu = new QPopupMenu( parent );

    menu->insertItem( i18n("Add Page" ), parent, addSlot );
    menu->insertItem( i18n("Delete Page" ), parent, removeSlot );
    menu->insertSeparator();
    actionEditCut->plug( menu );
    actionEditCopy->plug( menu );
    actionEditPaste->plug( menu );
    actionEditDelete->plug( menu );

    return menu;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
  QWidgetList windows = qWorkspace()->windowList();
  QWidgetListIt wit(windows);
  while (wit.current())
  {
    QWidget *w = wit.current();
    ++wit;
    if (w->inherits("FormWindow"))
    {
      if (!((FormWindow *) w)->formFile()->close())
      {
        e->ignore();
        return;
      }
    }
  }

  writeConfig();
  hide();
  e->accept();

  if (client)
  {
    QDir home(QDir::homeDirPath());
    home.remove(".designerpid");
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

bool MainWindow::openEditor(QWidget* w, FormWindow*)
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
            text = KInputDialog::getText( i18n("Text"), i18n("New text:" ), w->property("text").toString(), &ok, this );
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
        text = KInputDialog::getText( i18n("Title"), i18n("New title:" ), w->property("title").toString(), &ok, this );
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

    return TRUE;
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



void MainWindow::checkTempFiles()
{
    QString s = QDir::homeDirPath() + "/.designer";
    QString baseName = s+ "/saved-form-";
    if ( !QFile::exists( baseName + "1.kmdr" ) )
        return;
    QDir d( s );
    d.setNameFilter( "*.kmdr" );
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
    else if ( w->inherits( "FindDialog" ) )
        link += "dialog-find-text";
    else if ( w->inherits( "ReplaceDialog" ) )
        link += "dialog-replace-text";
    else if ( w->inherits( "GotoLineDialog" ) )
        link += "dialog-go-to-line";
    else if ( w->inherits( "ConnectionEditorBase" ) )
        link += "dialog-edit-connections";
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

    if ( assistant )
        assistant->sendRequest( link+'\n');
}



void MainWindow::fileOpenRecent(const KURL& filename)
{
  if (!QFile::exists(filename.path()))
  {
    QMessageBox::warning(this, i18n("Open File"), i18n("<qt>Could not open file:<br><b>%1</b><br>File does not exist.</qt>").
        arg(filename.path()));
    actionRecent->removeURL(filename);
  }
  else
    fileOpen("", filename.path());
}

void MainWindow::setupPlugins()
{
  KommanderFactory::loadPlugins();
}



void MainWindow::setModified( bool b, QWidget *window )
{
  QWidget *w = window;
  while (w)
  {
    if (w->inherits("FormWindow"))
    {
      ((FormWindow *) w)->modificationChanged(b);
      return;
    }
    w = w->parentWidget(TRUE);
    if (w->inherits("FormWindow"))
    {
      ((FormWindow *) w)->modificationChanged(b);
      return;
    }
    w = w->parentWidget(TRUE);
  }
}


void MainWindow::updateWorkspace()
{
}

QWidget *MainWindow::findRealForm( QWidget *wid )
{
    QWidgetList windows = qWorkspace()->windowList();
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
        if ( QString( w->name() ) == QString( wid->name() ) )
            return w;
    }
    return 0;
}

void MainWindow::formNameChanged(FormWindow*)
{
}


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

QString MainWindow::whatsThisFrom(const QString&)
{
  return QString::null;
}
#include "mainwindow.moc"

