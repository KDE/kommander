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
#ifndef KOMMANDER
#include "project.h"
#include "sourceeditor.h"
#endif
#include "propertyeditor.h"
#include "editslotsimpl.h"
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
#ifndef KOMMANDER
#include "languageinterface.h"
#endif
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

#ifndef KOMMANDER
static QPixmap *folderPixmap = 0;
#endif

QListViewItem *newItem = 0;

#ifndef KOMMANDER
    static QPluginManager<ClassBrowserInterface> *classBrowserInterfaceManager = 0;
#endif

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
#ifndef KOMMANDER
   MainWindow::self->currProject()->language() == "C++" &&
#endif
   ( txt == "init()" || txt == "destroy()" ) ) {
  listView()->setUpdatesEnabled( FALSE );
  if ( txt == "init()" )
      setText( 0, txt + " " + "(Constructor)" );
  else
      setText( 0, txt + " " + "(Destructor)" );
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
    addColumn( i18n( "Name" ) );
    addColumn( i18n( "Class" ) );
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
      addColumn( i18n( "Database" ) );
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
  AddTabPageCommand *cmd = new AddTabPageCommand( i18n( "Add Page to %1" ).arg( tw->name() ), formWindow,
              tw, "Tab" );
  formWindow->commandHistory()->addCommand( cmd );
  cmd->execute();
    } else if ( w->inherits( "QWizard" ) ) {
  QWizard *wiz = (QWizard*)formWindow->mainContainer();
  AddWizardPageCommand *cmd = new AddWizardPageCommand( i18n( "Add Page to %1" ).arg( wiz->name() ), formWindow,
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
      DeleteTabPageCommand *cmd = new DeleteTabPageCommand( i18n( "Delete Page %1 of %2" ).
                  arg( dtw->pageTitle() ).arg( tw->name() ),
                  formWindow, tw, tw->currentPage() );
      formWindow->commandHistory()->addCommand( cmd );
      cmd->execute();
  }
    } else if ( w->inherits( "QWizard" ) ) {
  QWizard *wiz = (QWizard*)formWindow->mainContainer();
  if ( wiz->currentPage() ) {
      QDesignerWizard *dw = (QDesignerWizard*)wiz;
      DeleteWizardPageCommand *cmd = new DeleteWizardPageCommand( i18n( "Delete Page %1 of %2" ).
                  arg( dw->pageTitle() ).arg( wiz->name() ),
                  formWindow, wiz,
                  wiz->indexOf( wiz->currentPage() ), TRUE );
      formWindow->commandHistory()->addCommand( cmd );
      cmd->execute();
  }
    }
}

// ------------------------------------------------------------

#ifndef KOMMANDER
FormDefinitionView::FormDefinitionView( QWidget *parent, FormWindow *fw )
    : HierarchyList( parent, fw, TRUE )
{
    header()->hide();
    removeColumn( 1 );
    connect( this, SIGNAL( itemRenamed( QListViewItem *, int, const QString & ) ),
       this, SLOT( renamed( QListViewItem * ) ) );
    popupOpen = FALSE;
    QAccel *a = new QAccel( MainWindow::self );
    a->connectItem( a->insertItem( ALT + Key_V ), this, SLOT( editVars() ) );
}

void FormDefinitionView::setup()
{
    if ( popupOpen || !formWindow )
  return;
    if ( !folderPixmap ) {
  folderPixmap = new QPixmap( folder_xpm );
    }

    clear();

#ifndef KOMMANDER
    LanguageInterface *lIface = MetaDataBase::languageInterface( formWindow->project()->language() );
#else
    LanguageInterface *lIface = MetaDataBase::languageInterface( "C++" );
#endif
    if ( lIface ) {
  QStringList defs = lIface->definitions();
  for ( QStringList::Iterator dit = defs.begin(); dit != defs.end(); ++dit ) {
      HierarchyItem *itemDef = new HierarchyItem( HierarchyItem::DefinitionParent,
              this, *dit, QString::null, QString::null );
      itemDef->setPixmap( 0, *folderPixmap );
      itemDef->setOpen( TRUE );
      QStringList entries = lIface->definitionEntries( *dit, formWindow->mainWindow()->designerInterface() );
      for ( QStringList::Iterator eit = entries.begin(); eit != entries.end(); ++eit ) {
    HierarchyItem *item = new HierarchyItem( HierarchyItem::Definition,
               itemDef, *eit, QString::null, QString::null );
    item->setRenameEnabled( 0, TRUE );
      }
  }
  lIface->release();
    }

    refresh( FALSE );
}

void FormDefinitionView::refresh( bool doDelete )
{
    if ( popupOpen )
  return;
    if ( !formWindow )
  return;
    QListViewItem *i = firstChild();
    while ( i ) {
  if ( doDelete && i->rtti() == HierarchyItem::SlotParent ) {
      QListViewItem* a = i;
      i = i->nextSibling();
      delete a;
      continue;
  }
  i = i->nextSibling();
    }

    QValueList<MetaDataBase::Slot> slotList = MetaDataBase::slotList( formWindow );
    HierarchyItem *itemSlots = new HierarchyItem( HierarchyItem::SlotParent,
              this, i18n( "Slots" ), QString::null, QString::null );
    itemSlots->moveItem( i );
    itemSlots->setPixmap( 0, *folderPixmap );
    HierarchyItem *itemPrivate = new HierarchyItem( HierarchyItem::Private, itemSlots, "private",
                QString::null, QString::null );
    HierarchyItem *itemProtected = new HierarchyItem( HierarchyItem::Protected, itemSlots, "protected",
                  QString::null, QString::null );
    HierarchyItem *itemPublic = new HierarchyItem( HierarchyItem::Public, itemSlots, "public",
               QString::null, QString::null );
    QValueList<MetaDataBase::Slot>::Iterator it = --( slotList.end() );
    if ( !slotList.isEmpty() ) {
  for (;;) {
      QListViewItem *item = 0;
      if ( (*it).access == "protected" )
    item = new HierarchyItem( HierarchyItem::Slot,
            itemProtected, (*it).slot, QString::null, QString::null );
      else if ( (*it).access == "private" )
    item = new HierarchyItem( HierarchyItem::Slot,
            itemPrivate, (*it).slot, QString::null, QString::null );
      else // default is public
    item = new HierarchyItem( HierarchyItem::Slot,
            itemPublic, (*it).slot, QString::null, QString::null );
      item->setPixmap( 0, PixmapChooser::loadPixmap( "editslots.xpm" ) );
      if ( it == slotList.begin() )
    break;
      --it;
  }
    }
    itemPrivate->setOpen( TRUE );
    itemProtected->setOpen( TRUE );
    itemPublic->setOpen( TRUE );
    itemSlots->setOpen( TRUE );
}


void FormDefinitionView::setCurrent( QWidget * )
{
}

void FormDefinitionView::objectClicked( QListViewItem *i )
{
    if ( !i )
  return;
    if ( i->rtti() == HierarchyItem::Slot )
  formWindow->mainWindow()->editFunction( i->text( 0 ) );
}


void FormDefinitionView::contentsMouseDoubleClickEvent( QMouseEvent *e )
{
    QListViewItem *i = itemAt( contentsToViewport( e->pos() ) );
    if ( !i )
  return;
    if ( i->rtti() == HierarchyItem::SlotParent )
  return;
    HierarchyItem::Type t = getChildType( i->rtti() );
    if ( (int)t == i->rtti() )
  i = i->parent();
#ifndef KOMMANDER
    if ( formWindow->project()->language() == "C++" &&
#else
    if (
#endif
   ( i->rtti() == HierarchyItem::Public ||
     i->rtti() == HierarchyItem::Protected ||
     i->rtti() == HierarchyItem::Private ) ) {
  EditSlots dlg( this, formWindow );
  QString access = "public";
  if ( i->rtti() == HierarchyItem::Protected )
      access = "protected";
  else if  ( i->rtti() == HierarchyItem::Private )
      access = "private";
  dlg.slotAdd( access );
  dlg.exec();
    } else {
  insertEntry( i );
    }
}


void FormDefinitionView::showRMBMenu( QListViewItem *i, const QPoint &pos )
{
    if ( !i )
  return;
    if ( i->rtti() == HierarchyItem::SlotParent ) {
  QPopupMenu menu;
  menu.insertItem( PixmapChooser::loadPixmap( "editslots" ), i18n( "Edit..." ) );
  if ( menu.exec( pos ) != -1 ) {
      EditSlots dlg( this, formWindow );
      dlg.exec();
  }
  return;
    }

    if ( i->rtti() == HierarchyItem::Slot ) {
  QPopupMenu menu;
  const int PROPS = 1;
  const int EDIT = 2;
  const int REMOVE = 3;
  const int NEW_ITEM = 4;
  menu.insertItem( PixmapChooser::loadPixmap( "filenew" ), i18n( "New" ), NEW_ITEM );
#ifndef KOMMANDER
  if ( formWindow->project()->language() == "C++" )
#endif
      menu.insertItem( PixmapChooser::loadPixmap( "editslots" ), i18n( "Properties..." ), PROPS );
#ifndef KOMMANDER
        if ( MetaDataBase::hasEditor( formWindow->project()->language() ) )
      menu.insertItem( i18n( "Goto Implementation" ), EDIT );
#endif
  menu.insertSeparator();
  menu.insertItem( PixmapChooser::loadPixmap( "editcut" ), i18n( "Delete" ), REMOVE );
  popupOpen = TRUE;
  int res = menu.exec( pos );
  popupOpen = FALSE;
  if ( res == NEW_ITEM ) {
#ifndef KOMMANDER
      if ( formWindow->project()->language() == "C++" ) {
#endif
    EditSlots dlg( this, formWindow );
    QString access = "public";
    if ( i->parent() && i->parent()->rtti() == HierarchyItem::Protected )
        access = "protected";
    else if  ( i->parent() && i->parent()->rtti() == HierarchyItem::Private )
        access = "private";
    dlg.slotAdd( access );
    dlg.exec();
#ifndef KOMMANDER
      } else {
    insertEntry( i->parent() );
      }
#endif
  }
  else if ( res == PROPS ) {
      EditSlots dlg( this, formWindow );
      dlg.setCurrentSlot( MetaDataBase::normalizeSlot( i->text( 0 ) ) );
      dlg.exec();
  } else if ( res == EDIT ) {
      formWindow->mainWindow()->editFunction( i->text( 0 ) );
  } else if ( res == REMOVE ) {
      MetaDataBase::removeSlot( formWindow, i->text( 0 ) );
      EditSlots::removeSlotFromCode( i->text( 0 ), formWindow );
      formWindow->mainWindow()->objectHierarchy()->updateFormDefinitionView();
      MainWindow::self->slotsChanged();
  }
  return;
    }

    QPopupMenu menu;
    const int NEW_ITEM = 1;
    const int DEL_ITEM = 2;
    const int EDIT_ITEM = 3;
    menu.insertItem( PixmapChooser::loadPixmap( "filenew" ), i18n( "New" ), NEW_ITEM );
    if ( i->rtti() == HierarchyItem::Definition || i->rtti() == HierarchyItem::DefinitionParent ) {
  if ( i->text( 0 ) == "Class Variables" ||
       i->parent() && i->parent()->text( 0 ) == "Class Variables" )
      menu.insertItem( i18n( "Edit...\tAlt+V" ), EDIT_ITEM );
  else
  {
    qWarning("[MB02] Edit");
      menu.insertItem( i18n( "Edit..." ), EDIT_ITEM );
  }
    }
    if ( i->parent() && i->rtti() != HierarchyItem::Public &&
   i->rtti() != HierarchyItem::Protected &&
   i->rtti() != HierarchyItem::Private ) {
  menu.insertSeparator();
  menu.insertItem( PixmapChooser::loadPixmap( "editcut" ), i18n( "Delete" ), DEL_ITEM );
    }
    popupOpen = TRUE;
    int res = menu.exec( pos );
    popupOpen = FALSE;
    if ( res == NEW_ITEM ) {
  HierarchyItem::Type t = getChildType( i->rtti() );
  if ( (int)t == i->rtti() )
      i = i->parent();
#ifndef KOMMANDER
  if ( formWindow->project()->language() == "C++" &&
#else
  if (
#endif
       ( i->rtti() == HierarchyItem::Public ||
         i->rtti() == HierarchyItem::Protected ||
         i->rtti() == HierarchyItem::Private ) ) {
      EditSlots dlg( this, formWindow );
      QString access = "public";
      if ( i->rtti() == HierarchyItem::Protected )
    access = "protected";
      else if  ( i->rtti() == HierarchyItem::Private )
    access = "private";
      dlg.slotAdd( access );
      dlg.exec();
  } else {
      insertEntry( i );
  }
    } else if ( res == DEL_ITEM ) {
  QListViewItem *p = i->parent();
  delete i;
  save( p, 0 );
    } else if ( res == EDIT_ITEM ) {
#ifndef KOMMANDER
  LanguageInterface *lIface = MetaDataBase::languageInterface( formWindow->project()->language() );
#else
  LanguageInterface *lIface = MetaDataBase::languageInterface( "C++" );
#endif
  if ( !lIface )
      return;
  if ( i->rtti() == HierarchyItem::Definition )
      i = i->parent();
  ListEditor dia( this, 0, TRUE );
  dia.setCaption( i18n( "Edit %1" ).arg( i->text( 0 ) ) );
  QStringList entries = lIface->definitionEntries( i->text( 0 ), MainWindow::self->designerInterface() );
  dia.setList( entries );
  dia.exec();
  lIface->setDefinitionEntries( i->text( 0 ), dia.items(), MainWindow::self->designerInterface() );
  refresh( TRUE );
  formWindow->commandHistory()->setModified( TRUE );
    }
}

void FormDefinitionView::renamed( QListViewItem *i )
{
    if ( newItem == i )
  newItem = 0;
    if ( !i->parent() )
  return;
    save( i->parent(), i );
}

void FormDefinitionView::save( QListViewItem *p, QListViewItem *i )
{
    if ( i && i->text( 0 ).isEmpty() ) {
  delete i;
  return;
    }
    if ( i && i->rtti() == HierarchyItem::Slot ) {
  MetaDataBase::addSlot( formWindow, i->text( 0 ).latin1(), "virtual", p->text( 0 ),
#ifndef KOMMANDER
             formWindow->project()->language(), "void" );
#else
             "C++", "void" );
#endif
  MainWindow::self->editFunction( i->text( 0 ).left( i->text( 0 ).find( "(" ) ),
#ifndef KOMMANDER
          formWindow->project()->language(), TRUE );
#else
          "C++", TRUE );
#endif
  MainWindow::self->objectHierarchy()->updateFormDefinitionView();
  return;
    }
#ifndef KOMMANDER
    LanguageInterface *lIface = MetaDataBase::languageInterface( formWindow->project()->language() );
#else
    LanguageInterface *lIface = MetaDataBase::languageInterface( "C++" );
#endif
    if ( !lIface )
  return;
    QStringList lst;
    i = p->firstChild();
    while ( i ) {
  lst << i->text( 0 );
  i = i->nextSibling();
    }
    lIface->setDefinitionEntries( p->text( 0 ), lst, formWindow->mainWindow()->designerInterface() );
    lIface->release();
    setup();
    formWindow->commandHistory()->setModified( TRUE );
}

void FormDefinitionView::editVars()
{
    if ( !formWindow )
  return;
#ifndef KOMMANDER
    LanguageInterface *lIface = MetaDataBase::languageInterface( formWindow->project()->language() );
#else
    LanguageInterface *lIface = MetaDataBase::languageInterface( "C++" );
#endif
    if ( !lIface )
  return;
    ListEditor dia( this, 0, TRUE );
    dia.setCaption( i18n( "Edit Class Variables" ) );
    QStringList entries = lIface->definitionEntries( "Class Variables", MainWindow::self->designerInterface() );
    dia.setList( entries );
    dia.exec();
    lIface->setDefinitionEntries( "Class Variables", dia.items(), MainWindow::self->designerInterface() );
    refresh( TRUE );
    formWindow->commandHistory()->setModified( TRUE );
}
#endif

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
#ifndef KOMMANDER
    editor = 0;
#endif
    setIcon( PixmapChooser::loadPixmap( "logo" ) );
    listview = new HierarchyList( this, formWindow() );
    addTab( listview, i18n( "Widgets" ) );
#ifndef KOMMANDER
  fView = new FormDefinitionView( this, formWindow() );
#endif
#ifndef KOMMANDER
  addTab( fView, i18n( "Source" ) );
#endif

#ifndef KOMMANDER
    if ( !classBrowserInterfaceManager ) {
  classBrowserInterfaceManager = new QPluginManager<ClassBrowserInterface>( IID_ClassBrowser, QApplication::libraryPaths(), "/designer" );
    }
#endif

#ifndef KOMMANDER
    classBrowsers = new QMap<QString, ClassBrowser>();
    QStringList langs = MetaDataBase::languages();
    for ( QStringList::Iterator it = langs.begin(); it != langs.end(); ++it ) {
  QInterfacePtr<ClassBrowserInterface> ciface = 0;
  classBrowserInterfaceManager->queryInterface( *it, &ciface );
  if ( ciface ) {
      ClassBrowser cb( ciface->createClassBrowser( this ), ciface );
      addTab( cb.lv, *it + i18n( " Classes" ) );
      ciface->onClick( this, SLOT( jumpTo( const QString &, const QString &, int ) ) );
      classBrowsers->insert( *it, cb );
  }
    }
#endif
}

HierarchyView::~HierarchyView()
{
}

void HierarchyView::clear()
{
    listview->clear();
#ifndef KOMMANDER
  fView->clear();
    for ( QMap<QString, ClassBrowser>::Iterator it = classBrowsers->begin();
    it != classBrowsers->end(); ++it ) {
  (*it).iface->clear();
    }
#endif
}

void HierarchyView::setFormWindow( FormWindow *fw, QWidget *w )
{
    if ( fw == 0 || w == 0 ) {
  listview->clear();
#ifndef KOMMANDER
  fView->clear();
#endif
  listview->setFormWindow( fw );
#ifndef KOMMANDER
  fView->setFormWindow( fw );
  editor = 0;
#endif
  formwindow = 0;
    }

    if ( fw == formwindow ) {
  listview->setCurrent( w );
  if ( MainWindow::self->qWorkspace()->activeWindow() == fw )
      showPage( listview );
#ifndef KOMMANDER
  else
      showPage( fView );
#endif
  return;
    }

    formwindow = fw;
    listview->setFormWindow( fw );
#ifndef KOMMANDER
  fView->setFormWindow( fw );
#endif
    listview->setup();
    listview->setCurrent( w );
#ifndef KOMMANDER
  fView->setup();
#endif

    if ( MainWindow::self->qWorkspace()->activeWindow() == fw )
  showPage( listview );
#ifndef KOMMANDER
    else
      showPage( fView );

    for ( QMap<QString, ClassBrowser>::Iterator it = classBrowsers->begin();
    it != classBrowsers->end(); ++it )
  (*it).iface->clear();
    editor = 0;
#endif
}

#ifndef KOMMANDER
void HierarchyView::showClasses( SourceEditor *se )
{
    if ( !se->object() )
  return;
    lastSourceEditor = se;
    QTimer::singleShot( 100, this, SLOT( showClassesTimeout() ) );
}
#endif

#ifndef KOMMANDER
void HierarchyView::showClassesTimeout()
{
    if ( !lastSourceEditor )
  return;
    SourceEditor *se = (SourceEditor*)lastSourceEditor;
    if ( !se->object() )
  return;
    if ( se->formWindow() ) {
  setFormWindow( se->formWindow(), se->formWindow()->currentWidget() );
  MainWindow::self->propertyeditor()->setWidget( se->formWindow()->currentWidget(),
                   se->formWindow() );
  return;
    }
    formwindow = 0;
    listview->setFormWindow( 0 );
#ifndef KOMMANDER
  fView->setFormWindow( 0 );
#endif
    listview->clear();
#ifndef KOMMANDER
  fView->clear();
#endif
    MainWindow::self->propertyeditor()->setWidget( 0, 0 );
#ifndef KOMMANDER
    editor = se;

    for ( QMap<QString, ClassBrowser>::Iterator it = classBrowsers->begin();
    it != classBrowsers->end(); ++it ) {
  if ( it.key() == se->project()->language() ) {
  if ( it.key() == "C++" ) {
      (*it).iface->update( se->text() );
      showPage( (*it).lv );
  } else {
      (*it).iface->clear();
  }
    }
#endif
}
#endif

#ifndef KOMMANDER
void HierarchyView::updateClassBrowsers()
{
    if ( !editor )
  return;
    for ( QMap<QString, ClassBrowser>::Iterator it = classBrowsers->begin();
    it != classBrowsers->end(); ++it ) {
  if ( it.key() == editor->project()->language() )
  if ( it.key() == "C++" )
      (*it).iface->update( editor->text() );
  else
      (*it).iface->clear();
    }
}
#endif

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
    if ( fw == formwindow ) {
  listview->clear();
#ifndef KOMMANDER
  fView->clear();
#endif
    }
}

#ifndef KOMMANDER
void HierarchyView::updateFormDefinitionView()
{
    fView->setup();
}
#endif

#ifndef KOMMANDER
void HierarchyView::jumpTo( const QString &func, const QString &clss, int type )
{
    if ( !editor )
  return;
    if ( type == ClassBrowserInterface::Class )
  editor->setClass( func );
    else
  editor->setFunction( func, clss );
}
#endif

#ifndef KOMMANDER
HierarchyView::ClassBrowser::ClassBrowser( QListView *l, ClassBrowserInterface *i )
    : lv( l ), iface( i )
{
}

HierarchyView::ClassBrowser::~ClassBrowser()
{
}
#endif
#include "hierarchyview.moc"
