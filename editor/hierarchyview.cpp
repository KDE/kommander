/**********************************************************************
** Copyright (C) 2000-2001 Trolltech AS.  All rights reserved.
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

#include "globaldefs.h"
#include "hierarchyview.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "command.h"
#include "widgetfactory.h"
#include "widgetdatabase.h"
#include "pixmapchooser.h"
#include "propertyeditor.h"
#include "listeditor.h"

#include <qpalette.h>
#include <qobjectlist.h>
#include <qheader.h>
#include <qpopupmenu.h>
#include <qtabwidget.h>
#include <qwizard.h>
#include <qwidgetstack.h>
#include <qtabbar.h>
#include <qfeatures.h>
#include <qapplication.h>
#include <qtimer.h>
#include <qworkspace.h>
#include <qaccel.h>

#include <klocale.h>

#include <stdlib.h>

static const char * const folder_xpm[]={
    "16 16 6 1",
    ". c None",
    "b c #ffff00",
    "d c #000000",
    "* c #999999",
    "a c #cccccc",
    "c c #ffffff",
    "................",
    "................",
    "..*****.........",
    ".*ababa*........",
    "*abababa******..",
    "*cccccccccccc*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "**************d.",
    ".dddddddddddddd.",
    "................"};

QListViewItem *newItem = 0;

HierarchyItem::HierarchyItem( Type type, QListViewItem *parent,
            const QString &txt1, const QString &txt2, const QString &txt3 )
    : QListViewItem( parent, txt1, txt2, txt3 ), typ( type )
{
}

HierarchyItem::HierarchyItem( Type type, QListView *parent,
            const QString &txt1, const QString &txt2, const QString &txt3 )
    : QListViewItem( parent, txt1, txt2, txt3 ), typ( type )
{
}

void HierarchyItem::paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align )
{
    QColorGroup g( cg );
    g.setColor( QColorGroup::Base, backgroundColor() );
    g.setColor( QColorGroup::Foreground, Qt::black );
    g.setColor( QColorGroup::Text, Qt::black );
    QString txt = text( 0 );
    if ( rtti() == Slot &&
   ( txt == "init()" || txt == "destroy()" ) ) {
  listView()->setUpdatesEnabled( FALSE );
  if ( txt == "init()" )
      setText( 0, txt + " " + i18n( "(Constructor)" ) );
  else
      setText( 0, txt + " " + i18n( "(Destructor)" ) );
  QListViewItem::paintCell( p, g, column, width, align );
  setText( 0, txt );
  listView()->setUpdatesEnabled( TRUE );
    } else {
  QListViewItem::paintCell( p, g, column, width, align );
    }
    p->save();
    p->setPen( QPen( cg.dark(), 1 ) );
    if ( column == 0 )
  p->drawLine( 0, 0, 0, height() - 1 );
    if ( listView()->firstChild() != this ) {
  if ( nextSibling() != itemBelow() && itemBelow()->depth() < depth() ) {
      int d = depth() - itemBelow()->depth();
      p->drawLine( -listView()->treeStepSize() * d, height() - 1, 0, height() - 1 );
  }
    }
    p->drawLine( 0, height() - 1, width, height() - 1 );
    p->drawLine( width - 1, 0, width - 1, height() );
    p->restore();
}

QColor HierarchyItem::backgroundColor()
{
    updateBackColor();
    return backColor;
}

void HierarchyItem::updateBackColor()
{
    if ( listView()->firstChild() == this ) {
  backColor = *backColor1;
  return;
    }

    QListViewItemIterator it( this );
    --it;
    if ( it.current() ) {
  if ( ( ( HierarchyItem*)it.current() )->backColor == *backColor1 )
      backColor = *backColor2;
  else
      backColor = *backColor1;
    } else {
  backColor = *backColor1;
    }
}

void HierarchyItem::setWidget( QWidget *w )
{
    wid = w;
}

QWidget *HierarchyItem::widget() const
{
    return wid;
}

void HierarchyItem::okRename( int col )
{
    if ( newItem == this )
  newItem = 0;
    QListViewItem::okRename( col );
}

void HierarchyItem::cancelRename( int col )
{
    if ( newItem == this ) {
  newItem = 0;
  QListViewItem::cancelRename( col );
  delete this;
  return;
    }
    QListViewItem::cancelRename( col );
}




HierarchyList::HierarchyList( QWidget *parent, FormWindow *fw, bool doConnects )
    : QListView( parent ), formWindow( fw )
{
    init_colors();

    setDefaultRenameAction( Accept );
    header()->setMovingEnabled( FALSE );
    header()->setStretchEnabled( TRUE );
    normalMenu = 0;
    tabWidgetMenu = 0;
    addColumn( i18n("Name" ) );
    addColumn( i18n("Class" ) );
    QPalette p( palette() );
    p.setColor( QColorGroup::Base, QColor( *backColor2 ) );
    (void)*selectedBack; // hack
    setPalette( p );
    disconnect( header(), SIGNAL( sectionClicked( int ) ),
    this, SLOT( changeSortColumn( int ) ) );
    setSorting( -1 );
    setHScrollBarMode( AlwaysOff );
    setVScrollBarMode( AlwaysOn );
    if ( doConnects ) {
  connect( this, SIGNAL( clicked( QListViewItem * ) ),
     this, SLOT( objectClicked( QListViewItem * ) ) );
  connect( this, SIGNAL( returnPressed( QListViewItem * ) ),
     this, SLOT( objectClicked( QListViewItem * ) ) );
  connect( this, SIGNAL( contextMenuRequested( QListViewItem *, const QPoint&, int ) ),
     this, SLOT( showRMBMenu( QListViewItem *, const QPoint & ) ) );
    }
    deselect = TRUE;
    setColumnWidthMode( 1, Manual );
}

void HierarchyList::keyPressEvent( QKeyEvent *e )
{
    if ( e->key() == Key_Shift || e->key() == Key_Control )
  deselect = FALSE;
    else
  deselect = TRUE;
    QListView::keyPressEvent( e );
}

void HierarchyList::keyReleaseEvent( QKeyEvent *e )
{
    deselect = TRUE;
    QListView::keyReleaseEvent( e );
}

void HierarchyList::viewportMousePressEvent( QMouseEvent *e )
{
    if ( e->state() & ShiftButton || e->state() & ControlButton )
  deselect = FALSE;
    else
  deselect = TRUE;
    QListView::viewportMousePressEvent( e );
}

void HierarchyList::viewportMouseReleaseEvent( QMouseEvent *e )
{
    QListView::viewportMouseReleaseEvent( e );
}

void HierarchyList::objectClicked( QListViewItem *i )
{
    if ( !i )
  return;

    QWidget *w = findWidget( i );
    if ( !w )
  return;
    if ( formWindow == w ) {
  if ( deselect )
      formWindow->clearSelection( FALSE );
  formWindow->emitShowProperties( formWindow );
  return;
    }

    if ( !formWindow->widgets()->find( w ) ) {
  if ( w->parent() && w->parent()->inherits( "QWidgetStack" ) &&
       w->parent()->parent() &&
       ( w->parent()->parent()->inherits( "QTabWidget" ) ||
         w->parent()->parent()->inherits( "QWizard" ) ) ) {
      if ( w->parent()->parent()->inherits( "QTabWidget" ) )
    ( (QTabWidget*)w->parent()->parent() )->showPage( w );
      else
    ( (QDesignerWizard*)w->parent()->parent() )->setCurrentPage( ( (QDesignerWizard*)w->parent()->parent() )->pageNum( w ) );
      w = (QWidget*)w->parent()->parent();
      formWindow->emitUpdateProperties( formWindow->currentWidget() );
  } else {
      return;
  }
    }

    if ( deselect )
  formWindow->clearSelection( FALSE );
    if ( w->isVisibleTo( formWindow ) )
  formWindow->selectWidget( w, TRUE );
}

QWidget *HierarchyList::findWidget( QListViewItem *i )
{
    return ( (HierarchyItem*)i )->widget();
}

QListViewItem *HierarchyList::findItem( QWidget *w )
{
    QListViewItemIterator it( this );
    while ( it.current() ) {
  if ( ( (HierarchyItem*)it.current() )->widget() == w )
      return it.current();
  ++it;
    }
    return 0;
}

QWidget *HierarchyList::current() const
{
    if ( currentItem() )
  return ( (HierarchyItem*)currentItem() )->widget();
    return 0;
}

void HierarchyList::changeNameOf( QWidget *w, const QString &name )
{
    QListViewItem *item = findItem( w );
    if ( !item )
  return;
    item->setText( 0, name );
}


void HierarchyList::changeDatabaseOf( QWidget *w, const QString & info )
{
#ifndef QT_NO_SQL
    if ( !formWindow->isDatabaseAware() )
  return;
    QListViewItem *item = findItem( w );
    if ( !item )
  return;
    item->setText( 2, info );
#else
  Q_UNUSED(w);
  Q_UNUSED(info);
#endif
}

void HierarchyList::setup()
{
    if ( !formWindow )
  return;
    clear();
    QWidget *w = formWindow->mainContainer();
#ifndef QT_NO_SQL
    if ( formWindow->isDatabaseAware() ) {
  if ( columns() == 2 ) {
      addColumn( i18n("Database" ) );
      header()->resizeSection( 0, 1 );
      header()->resizeSection( 1, 1 );
      header()->resizeSection( 2, 1 );
      header()->adjustHeaderSize();
  }
    } else {
  if ( columns() == 3 ) {
      removeColumn( 2 );
  }
    }
#endif
    if ( w )
  insertObject( w, 0 );
}

void HierarchyList::setOpen( QListViewItem *i, bool b )
{
    QListView::setOpen( i, b );
}

void HierarchyList::insertObject( QObject *o, QListViewItem *parent )
{
    bool fakeMainWindow = FALSE;
    if ( o && o->inherits( "QMainWindow" ) ) {
  QObject *cw = ( (QMainWindow*)o )->centralWidget();
  if ( cw ) {
      o = cw;
      fakeMainWindow = TRUE;
  }
    }
    QListViewItem *item = 0;
    QString className = WidgetFactory::classNameOf( o );
    if ( o->inherits( "QLayoutWidget" ) ) {
  switch ( WidgetFactory::layoutType( (QWidget*)o ) ) {
  case WidgetFactory::HBox:
      className = "HBox";
      break;
  case WidgetFactory::VBox:
      className = "VBox";
      break;
  case WidgetFactory::Grid:
      className = "Grid";
      break;
  default:
      break;
  }
    }

    QString dbInfo;
#ifndef QT_NO_SQL
    dbInfo = MetaDataBase::fakeProperty( o, "database" ).toStringList().join(".");
#endif

    QString name = o->name();
    if ( o->parent() && o->parent()->inherits( "QWidgetStack" ) &&
   o->parent()->parent() ) {
  if ( o->parent()->parent()->inherits( "QTabWidget" ) )
      name = ( (QTabWidget*)o->parent()->parent() )->tabLabel( (QWidget*)o );
  else if ( o->parent()->parent()->inherits( "QWizard" ) )
      name = ( (QWizard*)o->parent()->parent() )->title( (QWidget*)o );
    }

    if ( fakeMainWindow ) {
  name = o->parent()->name();
  className = "QMainWindow";
    }

    if ( !parent )
  item = new HierarchyItem( HierarchyItem::Widget, this, name, className, dbInfo );
    else
  item = new HierarchyItem( HierarchyItem::Widget, parent, name, className, dbInfo );
    if ( !parent )
  item->setPixmap( 0, PixmapChooser::loadPixmap( "form.xpm", PixmapChooser::Mini ) );
    else if ( o->inherits( "QLayoutWidget") )
  item->setPixmap( 0, PixmapChooser::loadPixmap( "layout.xpm", PixmapChooser::Small ) );
    else
  item->setPixmap( 0, WidgetDatabase::iconSet( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( o ) ) ).
       pixmap( QIconSet::Small, QIconSet::Normal ) );
    ( (HierarchyItem*)item )->setWidget( (QWidget*)o );

    const QObjectList *l = o->children();
    if ( !l )
  return;
    QObjectListIt it( *l );
    it.toLast();
    for ( ; it.current(); --it ) {
  if ( !it.current()->isWidgetType() || ( (QWidget*)it.current() )->isHidden() )
      continue;
  if (  !formWindow->widgets()->find( (QWidget*)it.current() ) ) {
      if ( it.current()->parent() &&
     ( it.current()->parent()->inherits( "QTabWidget" ) ||
       it.current()->parent()->inherits( "QWizard" ) ) &&
     it.current()->inherits( "QWidgetStack" ) ) {
    QObject *obj = it.current();
    QObjectList *l2 = obj->queryList( "QWidget", 0, TRUE, FALSE );
    QDesignerTabWidget *tw = 0;
    QDesignerWizard *dw = 0;
    if ( it.current()->parent()->inherits( "QTabWidget" ) )
        tw = (QDesignerTabWidget*)it.current()->parent();
    if ( it.current()->parent()->inherits( "QWizard" ) )
        dw = (QDesignerWizard*)it.current()->parent();
    QWidgetStack *stack = (QWidgetStack*)obj;
    for ( obj = l2->last(); obj; obj = l2->prev() ) {
        if ( qstrcmp( obj->className(), "QWidgetStackPrivate::Invisible" ) == 0 ||
       ( tw && !tw->tabBar()->tab( stack->id( (QWidget*)obj ) ) ) ||
       ( dw && dw->isPageRemoved( (QWidget*)obj ) ) )
      continue;
        insertObject( obj, item );
    }
    delete l2;
      }
      continue;
  }
  insertObject( it.current(), item );
    }

    if ( item->firstChild() )
  item->setOpen( TRUE );
}

void HierarchyList::setCurrent( QWidget *w )
{
    QListViewItemIterator it( this );
    while ( it.current() ) {
  if ( ( (HierarchyItem*)it.current() )->widget() == w ) {
      blockSignals( TRUE );
      setCurrentItem( it.current() );
      ensureItemVisible( it.current() );
      blockSignals( FALSE );
      return;
  }
  ++it;
    }
}

void HierarchyList::showRMBMenu( QListViewItem *i, const QPoint & p )
{
    if ( !i )
  return;


    QWidget *w = findWidget( i );
    if ( !w )
  return;

    if ( w != formWindow &&
   !formWindow->widgets()->find( w ) )
  return;

    if ( w->isVisibleTo( formWindow ) ) {
  if ( !w->inherits( "QTabWidget" ) && !w->inherits( "QWizard" ) ) {
      if ( !normalMenu )
    normalMenu = formWindow->mainWindow()->setupNormalHierarchyMenu( this );
      normalMenu->popup( p );
  } else {
      if ( !tabWidgetMenu )
    tabWidgetMenu =
        formWindow->mainWindow()->setupTabWidgetHierarchyMenu( this, SLOT( addTabPage() ),
                        SLOT( removeTabPage() ) );
      tabWidgetMenu->popup( p );
  }
    }
}

void HierarchyList::addTabPage()
{
    QWidget *w = current();
    if ( !w )
  return;
    if ( w->inherits( "QTabWidget" ) ) {
  QTabWidget *tw = (QTabWidget*)w;
  AddTabPageCommand *cmd = new AddTabPageCommand( i18n("Add Page to %1" ).arg( tw->name() ), formWindow,
              tw, "Tab" );
  formWindow->commandHistory()->addCommand( cmd );
  cmd->execute();
    } else if ( w->inherits( "QWizard" ) ) {
  QWizard *wiz = (QWizard*)formWindow->mainContainer();
  AddWizardPageCommand *cmd = new AddWizardPageCommand( i18n("Add Page to %1" ).arg( wiz->name() ), formWindow,
                    wiz, "Page" );
  formWindow->commandHistory()->addCommand( cmd );
  cmd->execute();
    }
}

void HierarchyList::removeTabPage()
{
    QWidget *w = current();
    if ( !w )
  return;
    if ( w->inherits( "QTabWidget" ) ) {
  QTabWidget *tw = (QTabWidget*)w;
  if ( tw->currentPage() ) {
      QDesignerTabWidget *dtw = (QDesignerTabWidget*)tw;
      DeleteTabPageCommand *cmd = new DeleteTabPageCommand( i18n("Delete Page %1 of %2" ).
                  arg( dtw->pageTitle() ).arg( tw->name() ),
                  formWindow, tw, tw->currentPage() );
      formWindow->commandHistory()->addCommand( cmd );
      cmd->execute();
  }
    } else if ( w->inherits( "QWizard" ) ) {
  QWizard *wiz = (QWizard*)formWindow->mainContainer();
  if ( wiz->currentPage() ) {
      QDesignerWizard *dw = (QDesignerWizard*)wiz;
      DeleteWizardPageCommand *cmd = new DeleteWizardPageCommand( i18n("Delete Page %1 of %2" ).
                  arg( dw->pageTitle() ).arg( wiz->name() ),
                  formWindow, wiz,
                  wiz->indexOf( wiz->currentPage() ), TRUE );
      formWindow->commandHistory()->addCommand( cmd );
      cmd->execute();
  }
    }
}

// ------------------------------------------------------------


// ------------------------------------------------------------
static HierarchyItem::Type getChildType( int type )
{
    switch ( (HierarchyItem::Type)type ) {
    case HierarchyItem::Widget:
    case HierarchyItem::SlotParent:
  qWarning( "getChildType: Inserting childs dynamically to Widget or SlotParent is not allwowed!" );
  return (HierarchyItem::Type)type;
    case HierarchyItem::Public:
    case HierarchyItem::Protected:
    case HierarchyItem::Private:
    case HierarchyItem::Slot:
  return HierarchyItem::Slot;
    case HierarchyItem::DefinitionParent:
    case HierarchyItem::Definition:
  return HierarchyItem::Definition;
    case HierarchyItem::Event:
    case HierarchyItem::EventFunction:
  return HierarchyItem::Event;
    }
    return (HierarchyItem::Type)type;
}

void HierarchyList::insertEntry( QListViewItem *i, const QPixmap &pix, const QString &s )
{
    HierarchyItem *item = new HierarchyItem( getChildType( i->rtti() ), i, s,
               QString::null, QString::null );
    if ( !pix.isNull() )
  item->setPixmap( 0, pix );
    item->setRenameEnabled( 0, TRUE );
    setCurrentItem( item );
    ensureItemVisible( item );
    qApp->processEvents();
    newItem = item;
    item->startRename( 0 );
}

HierarchyView::HierarchyView( QWidget *parent )
    : QTabWidget( parent, 0, WStyle_Customize | WStyle_NormalBorder | WStyle_Title |
      WStyle_Tool |WStyle_MinMax | WStyle_SysMenu )
{
    formwindow = 0;
    setIcon( PixmapChooser::loadPixmap( "logo" ) );
    listview = new HierarchyList( this, formWindow() );
    addTab( listview, i18n("Widgets" ) );
}

HierarchyView::~HierarchyView()
{
}

void HierarchyView::clear()
{
    listview->clear();
}

void HierarchyView::setFormWindow( FormWindow *fw, QWidget *w )
{
  if (fw == 0 || w == 0)
  {
    listview->clear();
    listview->setFormWindow(fw);
    formwindow = 0;
  }

  if (fw == formwindow)
  {
    listview->setCurrent(w);
    if (MainWindow::self->qWorkspace()->activeWindow() == fw)
      showPage(listview);
    return;
  }

  formwindow = fw;
  listview->setFormWindow(fw);
  listview->setup();
  listview->setCurrent(w);

  if (MainWindow::self->qWorkspace()->activeWindow() == fw)
    showPage(listview);
}

FormWindow *HierarchyView::formWindow() const
{
    return formwindow;
}

void HierarchyView::closeEvent( QCloseEvent *e )
{
  emit hidden();
  e->accept();
}

void HierarchyView::widgetInserted( QWidget * )
{
    listview->setup();
}

void HierarchyView::widgetRemoved( QWidget * )
{
    listview->setup();
}

void HierarchyView::widgetsInserted( const QWidgetList & )
{
    listview->setup();
}

void HierarchyView::widgetsRemoved( const QWidgetList & )
{
    listview->setup();
}

void HierarchyView::namePropertyChanged( QWidget *w, const QVariant & )
{
    QWidget *w2 = w;
    if ( w->inherits( "QMainWindow" ) )
  w2 = ( (QMainWindow*)w )->centralWidget();
    listview->changeNameOf( w2, w->name() );
}


void HierarchyView::databasePropertyChanged( QWidget *w, const QStringList& info )
{
#ifndef QT_NO_SQL
    QString i = info.join( "." );
    listview->changeDatabaseOf( w, i );
#else
  Q_UNUSED(w);
  Q_UNUSED(info);
#endif
}


void HierarchyView::tabsChanged( QTabWidget * )
{
    listview->setup();
}

void HierarchyView::pagesChanged( QWizard * )
{
    listview->setup();
}

void HierarchyView::rebuild()
{
    listview->setup();
}

void HierarchyView::closed( FormWindow *fw )
{
  if ( fw == formwindow ) 
    listview->clear();
}

#include "hierarchyview.moc"
