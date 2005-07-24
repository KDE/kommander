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

#include <qaction.h>
#include <qapplication.h>
#include <qbitmap.h>
#include <qdragobject.h>
#include <kinputdialog.h>
#include <qlayout.h>
#include <qmainwindow.h>
#include <qmenudata.h>
#include <qmessagebox.h>
#include <qobjectlist.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qtimer.h>

#include "actiondnd.h"
#include "command.h"
#include "defs.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "metadatabase.h"
#include "widgetfactory.h"

#include <klocale.h>

bool QDesignerAction::addTo( QWidget *w )
{
    if ( !widgetToInsert )
	return QAction::addTo( w );

    if ( w->inherits( "QPopupMenu" ) )
	return false;

    widgetToInsert->reparent( w, QPoint( 0, 0 ), false );
    addedTo( widgetToInsert, w );
    return true;
}

bool QDesignerAction::removeFrom( QWidget *w )
{
    if ( !widgetToInsert )
	return QAction::removeFrom( w );

    remove();
    return true;
}

void QDesignerAction::remove()
{
    if ( !widgetToInsert )
	return;
    MainWindow::self->formWindow()->selectWidget( widgetToInsert, false );
    widgetToInsert->reparent( 0, QPoint( 0, 0 ), false );
}

QDesignerToolBarSeparator::QDesignerToolBarSeparator(Orientation o , QToolBar *parent,
                                     const char* name )
    : QWidget( parent, name )
{
    connect( parent, SIGNAL(orientationChanged(Orientation)),
             this, SLOT(setOrientation(Orientation)) );
    setOrientation( o );
    setBackgroundMode( parent->backgroundMode() );
    setBackgroundOrigin( ParentOrigin );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
}

void QDesignerToolBarSeparator::setOrientation( Orientation o )
{
    orient = o;
}

void QDesignerToolBarSeparator::styleChange( QStyle& )
{
    setOrientation( orient );
}

QSize QDesignerToolBarSeparator::sizeHint() const
{
    int extent = style().pixelMetric( QStyle::PM_DockWindowSeparatorExtent,
				      this );
    if ( orient == Horizontal )
	return QSize( extent, 0 );
    else
	return QSize( 0, extent );
}

void QDesignerToolBarSeparator::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    QStyle::SFlags flags = QStyle::Style_Default;

    if ( orientation() == Horizontal )
	flags |= QStyle::Style_Horizontal;

    style().drawPrimitive( QStyle::PE_DockWindowSeparator, &p, rect(),
			   colorGroup(), flags );
}



QSeparatorAction::QSeparatorAction( QObject *parent )
    : QAction( parent, "qt_designer_separator" ), wid( 0 )
{
}

bool QSeparatorAction::addTo( QWidget *w )
{
    if ( w->inherits( "QToolBar" ) ) {
	QToolBar *tb = (QToolBar*)w;
	wid = new QDesignerToolBarSeparator( tb->orientation(), tb );
	return true;
    } else if ( w->inherits( "QPopupMenu" ) ) {
	idx = ( (QPopupMenu*)w )->count();
	( (QPopupMenu*)w )->insertSeparator( idx );
	return true;
    }
    return false;
}

bool QSeparatorAction::removeFrom( QWidget *w )
{
    if ( w->inherits( "QToolBar" ) ) {
	delete wid;
	return true;
    } else if ( w->inherits( "QPopupMenu" ) ) {
	( (QPopupMenu*)w )->removeItemAt( idx );
	return true;
    }
    return false;
}

QWidget *QSeparatorAction::widget() const
{
    return wid;
}




QDesignerToolBar::QDesignerToolBar( QMainWindow *mw )
    : QToolBar( mw ), lastIndicatorPos( -1, -1 )
{
    insertAnchor = 0;
    afterAnchor = true;
    setAcceptDrops( true );
    MetaDataBase::addEntry( this );
    lastIndicatorPos = QPoint( -1, -1 );
    indicator = new QDesignerIndicatorWidget( this );
    indicator->hide();
    installEventFilter( this );
    widgetInserting = false;
    findFormWindow();
    mw->setDockEnabled( DockTornOff, false );
}

QDesignerToolBar::QDesignerToolBar( QMainWindow *mw, Dock dock )
    : QToolBar( QString::null, mw, dock), lastIndicatorPos( -1, -1 )
{
    insertAnchor = 0;
    afterAnchor = true;
    setAcceptDrops( true );
    indicator = new QDesignerIndicatorWidget( this );
    indicator->hide();
    MetaDataBase::addEntry( this );
    installEventFilter( this );
    widgetInserting = false;
    findFormWindow();
    mw->setDockEnabled( DockTornOff, false );
}

void QDesignerToolBar::findFormWindow()
{
    QWidget *w = this;
    while ( w ) {
	if ( w->inherits( "FormWindow" ) )
	    formWindow = (FormWindow*)w;
	w = w->parentWidget();
    }
}

void QDesignerToolBar::addAction( QAction *a )
{
    actionList.append( a );
    connect( a, SIGNAL( destroyed() ), this, SLOT( actionRemoved() ) );
    if ( a->inherits( "QActionGroup" ) ) {
	( (QDesignerActionGroup*)a )->widget()->installEventFilter( this );
	actionMap.insert( ( (QDesignerActionGroup*)a )->widget(), a );
    } else if ( a->inherits( "QSeparatorAction" ) ) {
	( (QSeparatorAction*)a )->widget()->installEventFilter( this );
	actionMap.insert( ( (QSeparatorAction*)a )->widget(), a );
    } else {
	( (QDesignerAction*)a )->widget()->installEventFilter( this );
	actionMap.insert( ( (QDesignerAction*)a )->widget(), a );
    }
}

static void fixObject( QObject *&o )
{
    while ( o && o->parent() && !o->parent()->inherits( "QDesignerToolBar" ) )
	o = o->parent();
}

bool QDesignerToolBar::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e || o->inherits( "QDockWindowHandle" ) || o->inherits( "QDockWindowTitleBar" ) )
	return QToolBar::eventFilter( o, e );

    if ( o == this && e->type() == QEvent::MouseButtonPress &&
	 ( ( QMouseEvent*)e )->button() == LeftButton ) {
	mousePressEvent( (QMouseEvent*)e );
	return true;
    }

    if ( o == this )
	return QToolBar::eventFilter( o, e );

    if ( e->type() == QEvent::MouseButtonPress ) {
	QMouseEvent *ke = (QMouseEvent*)e;
	fixObject( o );
	if ( !o )
	    return false;
	buttonMousePressEvent( ke, o );
	return true;
    } else if(e->type() == QEvent::ContextMenu ) {
	QContextMenuEvent *ce = (QContextMenuEvent*)e;
	fixObject( o );
	if( !o )
	    return false;
	buttonContextMenuEvent( ce, o );
	return true;
    } else if ( e->type() == QEvent::MouseMove ) {
	QMouseEvent *ke = (QMouseEvent*)e;
	fixObject( o );
	if ( !o )
	    return false;
	buttonMouseMoveEvent( ke, o );
	return true;
    } else if ( e->type() == QEvent::MouseButtonRelease ) {
	QMouseEvent *ke = (QMouseEvent*)e;
	fixObject( o );
	if ( !o )
	    return false;
	buttonMouseReleaseEvent( ke, o );
	return true;
    } else if ( e->type() == QEvent::DragEnter ) {
	QDragEnterEvent *de = (QDragEnterEvent*)e;
	if ( de->provides( "application/x-designer-actions" ) ||
	     de->provides( "application/x-designer-actiongroup" ) ||
	     de->provides( "application/x-designer-separator" ) )
	    de->accept();
    } else if ( e->type() == QEvent::DragMove ) {
	QDragMoveEvent *de = (QDragMoveEvent*)e;
	if ( de->provides( "application/x-designer-actions" ) ||
	     de->provides( "application/x-designer-actiongroup" ) ||
	     de->provides( "application/x-designer-separator" ) )
	    de->accept();
    }

    return QToolBar::eventFilter( o, e );
}

void QDesignerToolBar::paintEvent( QPaintEvent *e )
{
    QToolBar::paintEvent( e );
    if ( e->rect() != rect() )
	return;
    lastIndicatorPos = QPoint( -1, -1 );
}

void QDesignerToolBar::contextMenuEvent( QContextMenuEvent *e )
{
    e->accept();
    QPopupMenu menu( 0 );
    menu.insertItem( i18n("Delete Toolbar" ), 1 );
    int res = menu.exec( e->globalPos() );
    if ( res != -1 ) {
	RemoveToolBarCommand *cmd = new RemoveToolBarCommand( i18n("Delete Toolbar '%1'" ).arg( name() ),
							      formWindow, 0, this );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    }
}

void QDesignerToolBar::mousePressEvent( QMouseEvent *e )
{
    widgetInserting = false;
    if ( e->button() == LeftButton &&
	 MainWindow::self->currentTool() != POINTER_TOOL &&
	 MainWindow::self->currentTool() != ORDER_TOOL ) {

	if ( MainWindow::self->currentTool() == CONNECT_TOOL ) {

	} else {
	    widgetInserting = true;
	}

	return;
    }
}

void QDesignerToolBar::mouseReleaseEvent( QMouseEvent *e )
{
    if ( widgetInserting )
	doInsertWidget( mapFromGlobal( e->globalPos() ) );
    widgetInserting = false;
}

void QDesignerToolBar::buttonMouseReleaseEvent( QMouseEvent *e, QObject *w )
{
    if ( widgetInserting )
	doInsertWidget( mapFromGlobal( e->globalPos() ) );
    else if ( w->isWidgetType() && formWindow->widgets()->find( w ) ) {
	formWindow->clearSelection( false );
	formWindow->selectWidget( w );
    }
    widgetInserting = false;
}

void QDesignerToolBar::buttonContextMenuEvent( QContextMenuEvent *e, QObject *o )
{
    e->accept();
    QPopupMenu menu( 0 );
    const int ID_DELETE = 1;
    const int ID_SEP = 2;
    const int ID_DELTOOLBAR = 3;
    QMap<QWidget*, QAction*>::Iterator it = actionMap.find( (QWidget*)o );
    if ( it != actionMap.end() && (*it)->inherits( "QSeparatorAction" ) )
	menu.insertItem( i18n("Delete Separator" ), ID_DELETE );
    else
	menu.insertItem( i18n("Delete Item" ), ID_DELETE );
    menu.insertItem( i18n("Insert Separator" ), ID_SEP );
    menu.insertSeparator();
    menu.insertItem( i18n("Delete Toolbar" ), ID_DELTOOLBAR );
    int res = menu.exec( e->globalPos() );
    if ( res == ID_DELETE ) {
	QMap<QWidget*, QAction*>::Iterator it = actionMap.find( (QWidget*)o );
	if ( it == actionMap.end() )
	    return;
	QAction *a = *it;
	int index = actionList.find( a );
	RemoveActionFromToolBarCommand *cmd = new RemoveActionFromToolBarCommand(
	    i18n("Delete Action '%1' From Toolbar '%2'" ).
	    arg( a->name() ).arg( caption() ),
	    formWindow, a, this, index );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    } else if ( res == ID_SEP ) {
	calcIndicatorPos( mapFromGlobal( e->globalPos() ) );
	QAction *a = new QSeparatorAction( 0 );
	int index = actionList.findRef( *actionMap.find( insertAnchor ) );
	if ( index != -1 && afterAnchor )
	    ++index;
	if ( !insertAnchor )
	    index = 0;

	AddActionToToolBarCommand *cmd = new AddActionToToolBarCommand(
	    i18n("Add Separator to Toolbar '%1'" ).
	    arg( a->name() ),
	    formWindow, a, this, index );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    } else if ( res == ID_DELTOOLBAR ) {
	RemoveToolBarCommand *cmd = new RemoveToolBarCommand( i18n("Delete Toolbar '%1'" ).arg( name() ),
							      formWindow, 0, this );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    }
}

void QDesignerToolBar::buttonMousePressEvent( QMouseEvent *e, QObject * )
{
    widgetInserting = false;

    if ( e->button() == MidButton )
	return;

    if ( e->button() == LeftButton &&
	 MainWindow::self->currentTool() != POINTER_TOOL &&
	 MainWindow::self->currentTool() != ORDER_TOOL ) {

	if ( MainWindow::self->currentTool() == CONNECT_TOOL ) {

	} else {
	    widgetInserting = true;
	}

	return;
    }


    dragStartPos = e->pos();
}

void QDesignerToolBar::removeWidget( QWidget *w )
{
    QMap<QWidget*, QAction*>::Iterator it = actionMap.find( w );
    if ( it == actionMap.end() )
	return;
    QAction *a = *it;
    int index = actionList.find( a );
    RemoveActionFromToolBarCommand *cmd =
	new RemoveActionFromToolBarCommand( i18n("Delete Action '%1' From Toolbar '%2'" ).
					    arg( a->name() ).arg( caption() ),
					    formWindow, a, this, index );
    formWindow->commandHistory()->addCommand( cmd );
    cmd->execute();
    QApplication::sendPostedEvents();
    adjustSize();
}

void QDesignerToolBar::buttonMouseMoveEvent( QMouseEvent *e, QObject *o )
{
    if ( widgetInserting || ( e->state() & LeftButton ) == 0 )
	return;
    if ( QABS( QPoint( dragStartPos - e->pos() ).manhattanLength() ) < QApplication::startDragDistance() )
	return;
    QMap<QWidget*, QAction*>::Iterator it = actionMap.find( (QWidget*)o );
    if ( it == actionMap.end() )
	return;
    QAction *a = *it;
    if ( !a )
	return;
    int index = actionList.find( a );
    RemoveActionFromToolBarCommand *cmd =
	new RemoveActionFromToolBarCommand( i18n("Delete Action '%1' From Toolbar '%2'" ).
					    arg( a->name() ).arg( caption() ),
					    formWindow, a, this, index );
    formWindow->commandHistory()->addCommand( cmd );
    cmd->execute();
    QApplication::sendPostedEvents();
    adjustSize();

    QString type = a->inherits( "QActionGroup" ) ? QString( "application/x-designer-actiongroup" ) :
	a->inherits( "QSeparatorAction" ) ? QString( "application/x-designer-separator" ) : QString( "application/x-designer-actions" );
    QStoredDrag *drag = new QStoredDrag( type, this );
    QString s = QString::number( (long)a ); // #### huha, that is evil
    drag->setEncodedData( QCString( s.latin1() ) );
    drag->setPixmap( a->iconSet().pixmap() );
    if ( a->inherits( "QDesignerAction" ) ) {
	if ( formWindow->widgets()->find( ( (QDesignerAction*)a )->widget() ) )
	    formWindow->selectWidget( ( (QDesignerAction*)a )->widget(), false );
    }
    if ( !drag->drag() ) {
	AddActionToToolBarCommand *cmd = new AddActionToToolBarCommand( i18n("Add Action '%1' to Toolbar '%2'" ).
									arg( a->name() ).arg( caption() ),
									formWindow, a, this, index );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    }
    lastIndicatorPos = QPoint( -1, -1 );
    indicator->hide();
}

#ifndef QT_NO_DRAGANDDROP

void QDesignerToolBar::dragEnterEvent( QDragEnterEvent *e )
{
    widgetInserting = false;
    lastIndicatorPos = QPoint( -1, -1 );
    if ( e->provides( "application/x-designer-actions" ) ||
	 e->provides( "application/x-designer-actiongroup" ) ||
	 e->provides( "application/x-designer-separator" ) )
	e->accept();
}

void QDesignerToolBar::dragMoveEvent( QDragMoveEvent *e )
{
    if ( e->provides( "application/x-designer-actions" ) ||
	 e->provides( "application/x-designer-actiongroup" ) ||
	 e->provides( "application/x-designer-separator" ) )
	e->accept();
    else
	return;
    drawIndicator( calcIndicatorPos( e->pos() ) );
}

void QDesignerToolBar::dragLeaveEvent( QDragLeaveEvent * )
{
    indicator->hide();
    insertAnchor = 0;
    afterAnchor = true;
}

void QDesignerToolBar::dropEvent( QDropEvent *e )
{
    if ( e->provides( "application/x-designer-actions" ) ||
	 e->provides( "application/x-designer-actiongroup" ) ||
	 e->provides( "application/x-designer-separator" ) )
	e->accept();
    else
	return;
    QString s;
    if ( e->provides( "application/x-designer-actiongroup" ) )
	s = QString( e->encodedData( "application/x-designer-actiongroup" ) );
    else if ( e->provides( "application/x-designer-separator" ) )
	s = QString( e->encodedData( "application/x-designer-separator" ) );
    else
	s = QString( e->encodedData( "application/x-designer-actions" ) );

    indicator->hide();
    QAction *a = 0;
    int index = actionList.findRef( *actionMap.find( insertAnchor ) );
    if ( index != -1 && afterAnchor )
	++index;
    if ( !insertAnchor )
	index = 0;
    if ( e->provides( "application/x-designer-actions" ) ||
	 e->provides( "application/x-designer-separator" ) ) {
	if ( e->provides( "application/x-designer-actions" ) )
	    a = (QDesignerAction*)s.toLong();
	else
	    a = (QSeparatorAction*)s.toLong();
    } else {
	a = (QDesignerActionGroup*)s.toLong();
    }

    if ( actionList.findRef( a ) != -1 ) {
	QMessageBox::warning( MainWindow::self, i18n("Insert/Move Action" ),
			      i18n("Action '%1' has already been added to this toolbar.\n"
				  "An Action may only occur once in a given toolbar." ).
			      arg( a->name() ) );
	return;
    }

    AddActionToToolBarCommand *cmd = new AddActionToToolBarCommand( i18n("Add Action '%1' to Toolbar '%2'" ).
								    arg( a->name() ).arg( caption() ),
								    formWindow, a, this, index );
    formWindow->commandHistory()->addCommand( cmd );
    cmd->execute();

    lastIndicatorPos = QPoint( -1, -1 );
}

#endif

void QDesignerToolBar::reInsert()
{
    QAction *a = 0;
    actionMap.clear();
    clear();
    for ( a = actionList.first(); a; a = actionList.next() ) {
	a->addTo( this );
	if ( a->inherits( "QActionGroup" ) ) {
	    actionMap.insert( ( (QDesignerActionGroup*)a )->widget(), a );
	    if ( ( (QDesignerActionGroup*)a )->widget() )
		( (QDesignerActionGroup*)a )->widget()->installEventFilter( this );
	} else if ( a->inherits( "QDesignerAction" ) ) {
	    actionMap.insert( ( (QDesignerAction*)a )->widget(), a );
	    ( (QDesignerAction*)a )->widget()->installEventFilter( this );
	} else if ( a->inherits( "QSeparatorAction" ) ) {
	    actionMap.insert( ( (QSeparatorAction*)a )->widget(), a );
	    ( (QSeparatorAction*)a )->widget()->installEventFilter( this );
	}
    }
    QApplication::sendPostedEvents();
    adjustSize();
}

void QDesignerToolBar::actionRemoved()
{
    actionList.removeRef( (QAction*)sender() );
}

QPoint QDesignerToolBar::calcIndicatorPos( const QPoint &pos )
{
    if ( orientation() == Horizontal ) {
	QPoint pnt( width() - 2, 0 );
	insertAnchor = 0;
	afterAnchor = true;
	if ( !children() )
	    return pnt;
	pnt = QPoint( 13, 0 );
	QObjectListIt it( *children() );
	QObject * obj;
	while( (obj=it.current()) != 0 ) {
	    ++it;
	    if ( obj->isWidgetType() &&
		 qstrcmp( "qt_dockwidget_internal", obj->name() ) != 0 ) {
		QWidget *w = (QWidget*)obj;
		if ( w->x() < pos.x() ) {
		    pnt.setX( w->x() + w->width() + 1 );
		    insertAnchor = w;
		    afterAnchor = true;
		}
	    }
	}
	return pnt;
    } else {
	QPoint pnt( 0, height() - 2 );
	insertAnchor = 0;
	afterAnchor = true;
	if ( !children() )
	    return pnt;
	pnt = QPoint( 0, 13 );
	QObjectListIt it( *children() );
	QObject * obj;
	while( (obj=it.current()) != 0 ) {
	    ++it;
	    if ( obj->isWidgetType() &&
		 qstrcmp( "qt_dockwidget_internal", obj->name() ) != 0 ) {
		QWidget *w = (QWidget*)obj;
		if ( w->y() < pos.y() ) {
		    pnt.setY( w->y() + w->height() + 1 );
		    insertAnchor = w;
		    afterAnchor = true;
		}
	    }
	}
	return pnt;
    }
}

void QDesignerToolBar::drawIndicator( const QPoint &pos )
{
    if ( lastIndicatorPos == pos )
	return;
    bool wasVsisible = indicator->isVisible();
    if ( orientation() == Horizontal ) {
	indicator->resize( 3, height() );
	if ( pos != QPoint( -1, -1 ) )
	     indicator->move( pos.x() - 1, 0 );
	indicator->show();
	indicator->raise();
	lastIndicatorPos = pos;
    } else {
	indicator->resize( width(), 3 );
	if ( pos != QPoint( -1, -1 ) )
	     indicator->move( 0, pos.y() - 1 );
	indicator->show();
	indicator->raise();
	lastIndicatorPos = pos;
    }
    if ( !wasVsisible )
	QApplication::sendPostedEvents();
}

void QDesignerToolBar::doInsertWidget( const QPoint &p )
{
    if ( formWindow != MainWindow::self->formWindow() )
	return;
    calcIndicatorPos( p );
    QWidget *w = WidgetFactory::create( MainWindow::self->currentTool(), this, 0, true );
    installEventFilters( w );
    MainWindow::self->formWindow()->insertWidget( w, true );
    QDesignerAction *a = new QDesignerAction( w, parent() );
    int index = actionList.findRef( *actionMap.find( insertAnchor ) );
    if ( index != -1 && afterAnchor )
	++index;
    if ( !insertAnchor )
	index = 0;
    AddActionToToolBarCommand *cmd = new AddActionToToolBarCommand( i18n("Add Widget '%1' to Toolbar '%2'" ).
								    arg( w->name() ).arg( caption() ),
								    formWindow, a, this, index );
    formWindow->commandHistory()->addCommand( cmd );
    cmd->execute();
    MainWindow::self->resetTool();
}

void QDesignerToolBar::clear()
{
    for ( QAction *a = actionList.first(); a; a = actionList.next() ) {
	if ( a->inherits( "QDesignerAction" ) )
	    ( (QDesignerAction*)a )->remove();
    }
    QToolBar::clear();
}

void QDesignerToolBar::installEventFilters( QWidget *w )
{
    if ( !w )
	return;
    QObjectList *l = w->queryList( "QWidget" );
    for ( QObject *o = l->first(); o; o = l->next() )
	o->installEventFilter( this );
    delete l;
}



QDesignerMenuBar::QDesignerMenuBar( QWidget *mw )
    : QMenuBar( mw, 0 )
{
    show();
    setAcceptDrops( true );
    MetaDataBase::addEntry( this );
    itemNum = 0;
    mousePressed = false;
    lastIndicatorPos = QPoint( -1, -1 );
    insertAt = -1;
    indicator = new QDesignerIndicatorWidget( this );
    indicator->hide();
    findFormWindow();
}

void QDesignerMenuBar::findFormWindow()
{
    QWidget *w = this;
    while ( w ) {
	if ( w->inherits( "FormWindow" ) )
	    formWindow = (FormWindow*)w;
	w = w->parentWidget();
    }
}

void QDesignerMenuBar::contextMenuEvent( QContextMenuEvent *e )
{
    e->accept();
    int itm = itemAtPos( e->pos() );
    if ( itm == -1 ) {
	if ( formWindow )
	    formWindow->mainWindow()->popupFormWindowMenu( e->globalPos(), formWindow );
	return;
    }
    QPopupMenu menu( this );
    menu.insertItem( i18n("Delete Item" ), 1 );
    menu.insertItem( i18n("Rename Item..." ), 2 );
    int res = menu.exec( e->globalPos() );
    if ( res == 1 ) {
	QMenuItem *item = findItem( idAt( itm ) );
	RemoveMenuCommand *cmd = new RemoveMenuCommand( i18n("Delete Menu '%1'" ).arg( item->text() ),
							formWindow,
							(QMainWindow*)parentWidget(), this,
							(QDesignerPopupMenu*)item->popup(),
							idAt( itm ), itm, item->text() );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
	// #### need to do a proper invalidate and re-layout
	parentWidget()->layout()->invalidate();
	parentWidget()->layout()->activate();
    } else if ( res == 2 ) {
	bool ok;
	QString old = text( idAt( itm ) );
	QString txt = KInputDialog::getText( i18n("Rename Menu Item" ), i18n("Menu text:" ),
					     text( idAt( itm ) ), &ok, 0 );
	if ( ok ) {
	    RenameMenuCommand *cmd = new RenameMenuCommand(
		i18n("Rename Menu '%1' to '%2'" ).arg( old ).arg( txt ),
		formWindow, this, idAt( itm ), old, txt );
	    formWindow->commandHistory()->addCommand( cmd );
	    cmd->execute();
	}
    }
}

void QDesignerMenuBar::mousePressEvent( QMouseEvent *e )
{
    lastIndicatorPos = QPoint( -1, -1 );
    insertAt = -1;
    mousePressed = true;
    if ( e->button() == MidButton || e->button() == RightButton )
	return;

    dragStartPos = e->pos();
    QMenuBar::mousePressEvent( e );
}

void QDesignerMenuBar::mouseMoveEvent( QMouseEvent *e )
{
    if ( !mousePressed || e->state() == NoButton ) {
	QMenuBar::mouseMoveEvent( e );
	return;
    }
    if ( QABS( QPoint( dragStartPos - e->pos() ).manhattanLength() ) < QApplication::startDragDistance() )
	return;
    hidePopups();
    activateItemAt( -1 );
    int itm = itemAtPos( dragStartPos );
    if ( itm == -1 )
	return;
    QPopupMenu *popup = findItem( idAt( itm ) )->popup();
    QString txt = findItem( idAt( itm ) )->text();
    removeItemAt( itm );

    QStoredDrag *drag = new QStoredDrag( "application/x-designer-menuitem", this );
    QString s = QString::number( (long)popup );
    s += "/" + txt;
    drag->setEncodedData( QCString( s.latin1() ) );
    QSize sz( fontMetrics().boundingRect( txt ).size() );
    QPixmap pix( sz.width() + 20, sz.height() * 2 );
    pix.fill( white );
    QPainter p( &pix, this );
    p.drawText( 2, 0, pix.width(), pix.height(), 0, txt );
    p.end();
    pix.setMask( pix.createHeuristicMask() );
    drag->setPixmap( pix );
    oldPos = itm;
    if ( !drag->drag() ) {
	insertItem( txt, popup, -1, itm );
    }
    lastIndicatorPos = QPoint( -1, -1 );
    indicator->hide();
    mousePressed = false;
}

void QDesignerMenuBar::mouseReleaseEvent( QMouseEvent *e )
{
    QMenuBar::mouseReleaseEvent( e );
    mousePressed = false;
}

#ifndef QT_NO_DRAGANDDROP

void QDesignerMenuBar::dragEnterEvent( QDragEnterEvent *e )
{
    if ( e->provides( "application/x-designer-actions" ) ||
	 e->provides( "application/x-designer-actiongroup" ) ||
	 e->provides( "application/x-designer-separator" ) )
	e->accept();
    if ( e->provides( "application/x-designer-menuitem" ) )
	e->accept();
    lastIndicatorPos = QPoint( -1, -1 );
    insertAt = -1;
}

void QDesignerMenuBar::dragMoveEvent( QDragMoveEvent *e )
{
    if ( e->provides( "application/x-designer-actions" ) ||
	 e->provides( "application/x-designer-menuitem" ) ||
	 e->provides( "application/x-designer-actiongroup" ) ||
	 e->provides( "application/x-designer-separator" ) )
	e->accept();
    else
	return;
    if ( e->provides( "application/x-designer-actions" ) ||
	 e->provides( "application/x-designer-actiongroup" ) ||
	 e->provides( "application/x-designer-separator" ) ) {
	int item = itemAtPos( e->pos() );
	bool uieffect = QApplication::isEffectEnabled( UI_AnimateMenu );
	QApplication::setEffectEnabled( UI_AnimateMenu, false );
	if ( !qApp->activePopupWidget() )
	    actItem = -1;
	activateItemAt( item );
	QApplication::setEffectEnabled( UI_AnimateMenu, uieffect );
	if ( item == -1 )
	    hidePopups();
    } else {
	drawIndicator( calcIndicatorPos( e->pos() ) );
    }
}

void QDesignerMenuBar::dragLeaveEvent( QDragLeaveEvent * )
{
    mousePressed = false;
    lastIndicatorPos = QPoint( -1, -1 );
    insertAt = -1;
}

void QDesignerMenuBar::dropEvent( QDropEvent *e )
{
    mousePressed = false;
    if ( !e->provides( "application/x-designer-menuitem" ) )
	return;
    e->accept();
    QString s( e->encodedData( "application/x-designer-menuitem" ) );
    QString s1 = s.left( s.find( "/" ) );
    QString s2 = s.mid( s.find( "/" ) + 1 );
    QPopupMenu *popup = (QPopupMenu*)s1.toLong();  // #### huha, that is evil
    QString txt = s2;
    insertItem( txt, popup, -1, insertAt );

    MoveMenuCommand *cmd = new MoveMenuCommand( i18n("Move Menu '%1'" ).arg( txt ), formWindow,
						this, (QDesignerPopupMenu*)popup, oldPos, insertAt, txt );
    // do not execute, we did the work already
    formWindow->commandHistory()->addCommand( cmd );

    indicator->hide();
}

#endif

QPoint QDesignerMenuBar::calcIndicatorPos( const QPoint &pos )
{
    int w = frameWidth();
    insertAt = count();
    for ( int i = 0; i < (int)count(); ++i ) {
	QRect r = itemRect( i );
	if ( pos.x() < w + r.width() / 2 ) {
	    insertAt = i;
	    break;
	}
	w += r.width();
    }

    return QPoint( w, 0 );
}

void QDesignerMenuBar::drawIndicator( const QPoint &pos )
{
    if ( lastIndicatorPos == pos )
	return;
    bool wasVsisible = indicator->isVisible();
    indicator->resize( 3, height() );
    indicator->move( pos.x() - 1, 0 );
    indicator->show();
    indicator->raise();
    lastIndicatorPos = pos;
    if ( !wasVsisible )
	QApplication::sendPostedEvents();
}

void QDesignerMenuBar::setItemNumber( int num )
{
    itemNum = num;
}

int QDesignerMenuBar::itemNumber() const
{
    return itemNum;
}

void QDesignerMenuBar::setItemText( const QString &s )
{
    if ( itemNum < 0 || itemNum >= (int)count() )
	return;
    changeItem( idAt( itemNum ), s );
}

QString QDesignerMenuBar::itemText() const
{
    if ( itemNum < 0 || (int)itemNum >= (int)count() )
	return QString::null;
    return text( idAt( itemNum ) );
}

void QDesignerMenuBar::setItemName( const QCString &s )
{
    if ( itemNum < 0 || itemNum >= (int)count() )
	return;
    findItem( idAt( itemNum ) )->popup()->setName( s );
}

QCString QDesignerMenuBar::itemName() const
{
    if ( itemNum < 0 || itemNum >= (int)count() )
	return "";
    return findItem( idAt( itemNum ) )->popup()->name();
}



QDesignerPopupMenu::QDesignerPopupMenu( QWidget *w )
    : QPopupMenu( w, 0 ),
      popupMenu( 0 )
{
    findFormWindow();
    setAcceptDrops( true );
    insertAt = -1;
    mousePressed = false;
    lastIndicatorPos = QPoint( -1, -1 );
    indicator = new QDesignerIndicatorWidget( this );
    indicator->hide();
}

void QDesignerPopupMenu::contextMenuEvent( QContextMenuEvent *e )
{
#if defined( Q_WS_MAC ) //the mac needs us to use context menu rather than right click
    e->accept();
    QMouseEvent me( QEvent::MouseButtonPress, e->pos(), e->globalPos(), RightButton, RightButton );
    mousePressEvent(&me);
#else
    Q_UNUSED( e );
#endif
}

void QDesignerPopupMenu::mousePressEvent( QMouseEvent *e )
{
    if ( e->button() == MidButton )
	return;

    if ( e->button() == RightButton ) {
	// A popup for a popup, we only need one, so make sure that
	// we don't create multiple.  The timer keeps the event loop sane.
	popupPos = e->globalPos();
	popupLocalPos = e->pos();
	if ( popupMenu ) {
	    popupMenu->close();
	}
	e->accept();
	QTimer::singleShot( 0, this, SLOT(createPopupMenu()) );
	return;
    }
    mousePressed = true;
    dragStartPos = e->pos();
    QPopupMenu::mousePressEvent( e );
}

void QDesignerPopupMenu::createPopupMenu()
{
    // actually creates our popup for the popupmenu.
    QPopupMenu menu( 0 );
    popupMenu = &menu;
    int itm;
    const int ID_DELETE = 1;
    const int ID_SEP = 2;
    itm = itemAtPos( popupLocalPos, false );
    if ( itm == -1 )
	return;
    QAction *a = actionList.at( itm );
    if ( a && a->inherits( "QSeparatorAction" ) )
	menu.insertItem( i18n("Delete Separator" ), ID_DELETE );
    else
	menu.insertItem( i18n("Delete Item" ), ID_DELETE );
    menu.insertItem( i18n("Insert Separator" ), ID_SEP );
    int res = menu.exec( popupPos );
    if ( res == ID_DELETE ) {
	QAction *a = actionList.at( itm );
	if ( !a )
	    return;
	RemoveActionFromPopupCommand *cmd = new RemoveActionFromPopupCommand(
									     i18n("Delete Action '%1' From Popup Menu '%2'" ).
									     arg( a->name() ).arg( caption() ),
									     formWindow, a, this, itm );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    } else if ( res == ID_SEP ) {
	QPoint p( pos() );
	calcIndicatorPos( mapFromGlobal( popupPos ) );
	QAction *a = new QSeparatorAction( 0 );
	AddActionToPopupCommand *cmd = new AddActionToPopupCommand(
								   i18n("Add Separator to Popup Menu '%1'" ).
								   arg( name() ),
								   formWindow, a, this, insertAt );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
	( (QDesignerMenuBar*)( (QMainWindow*)parentWidget() )->menuBar() )->hidePopups();
	( (QDesignerMenuBar*)( (QMainWindow*)parentWidget() )->menuBar() )->activateItemAt( -1 );
	popup( p );
    }
    // set this back to zero so we know a popup (will soon) not exist.
    popupMenu = 0;
}

void QDesignerPopupMenu::mouseMoveEvent( QMouseEvent *e )
{
    if ( !mousePressed || e->state() == NoButton ) {
	QPopupMenu::mouseMoveEvent( e );
	return;
    }
    if ( QABS( QPoint( dragStartPos - e->pos() ).manhattanLength() ) < QApplication::startDragDistance() ) {
	QPopupMenu::mouseMoveEvent( e );
	return;
    }
    int itm = itemAtPos( dragStartPos, false );
    if ( itm == -1 )
	return;
    QAction *a = actionList.at( itm );
    if ( !a )
	return;
    RemoveActionFromPopupCommand *cmd = new RemoveActionFromPopupCommand( i18n("Delete Action '%1' From Popup Menu '%2'" ).
									  arg( a->name() ).arg( caption() ),
									  formWindow, a, this, itm );
    formWindow->commandHistory()->addCommand( cmd );
    cmd->execute();

    QString type = a->inherits( "QActionGroup" ) ? QString( "application/x-designer-actiongroup" ) :
	a->inherits( "QSeparatorAction" ) ? QString( "application/x-designer-separator" ) : QString( "application/x-designer-actions" );
    QStoredDrag *drag = new QStoredDrag( type, this );
    QString s = QString::number( (long)a ); // #### huha, that is evil
    drag->setEncodedData( QCString( s.latin1() ) );
    drag->setPixmap( a->iconSet().pixmap() );
    if ( !drag->drag() ) {
	AddActionToPopupCommand *cmd = new AddActionToPopupCommand( i18n("Add Action '%1' to Popup Menu '%2'" ).
								    arg( a->name() ).arg( name() ),
								    formWindow, a, this, itm );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    }
    indicator->hide();
    lastIndicatorPos = QPoint( -1, -1 );
    mousePressed = false;
}

void QDesignerPopupMenu::mouseReleaseEvent( QMouseEvent *e )
{
    mousePressed = false;
    QPopupMenu::mouseReleaseEvent( e );
}

#ifndef QT_NO_DRAGANDDROP

void QDesignerPopupMenu::dragEnterEvent( QDragEnterEvent *e )
{
    mousePressed = false;
    lastIndicatorPos = QPoint( -1, -1 );
    if ( e->provides( "application/x-designer-actions" ) ||
	 e->provides( "application/x-designer-actiongroup" ) ||
	 e->provides( "application/x-designer-separator" ) )
	e->accept();
}

void QDesignerPopupMenu::dragMoveEvent( QDragMoveEvent *e )
{
    mousePressed = false;
    if ( e->provides( "application/x-designer-actions" ) ||
	 e->provides( "application/x-designer-actiongroup" ) ||
	 e->provides( "application/x-designer-separator" ) )
	e->accept();
    else
	return;
    drawIndicator( calcIndicatorPos( e->pos() ) );
}

void QDesignerPopupMenu::dragLeaveEvent( QDragLeaveEvent * )
{
    mousePressed = false;
    indicator->hide();
    insertAt = -1;
}

void QDesignerPopupMenu::dropEvent( QDropEvent *e )
{
    mousePressed = false;
    if ( e->provides( "application/x-designer-actions" ) ||
	 e->provides( "application/x-designer-actiongroup" ) ||
	 e->provides( "application/x-designer-separator" ) )
	e->accept();
    else
	return;

    QPoint p = pos();
    QAction *a = 0;
    if ( e->provides( "application/x-designer-actiongroup" ) ) {
	QString s( e->encodedData( "application/x-designer-actiongroup" ) );
	a = (QDesignerActionGroup*)s.toLong();
    } else {
	QString s;
	if ( e->provides( "application/x-designer-separator" ) ) {
	    s = QString( e->encodedData( "application/x-designer-separator" ) );
	    a = (QSeparatorAction*)s.toLong();
	} else {
	    s = QString( e->encodedData( "application/x-designer-actions" ) );
	    a = (QDesignerAction*)s.toLong();
	}
    }

    if ( actionList.findRef( a ) != -1 ) {
	QMessageBox::warning( MainWindow::self, i18n("Insert/Move Action" ),
			      i18n("Action '%1' has already been added to this menu.\n"
				  "An Action may only occur once in a given menu." ).
			      arg( a->name() ) );
	return;
    }

    AddActionToPopupCommand *cmd = new AddActionToPopupCommand( i18n("Add Action '%1' to Popup Menu '%2'" ).
								arg( a->name() ).arg( name() ),
								formWindow, a, this, insertAt );
    formWindow->commandHistory()->addCommand( cmd );
    cmd->execute();

    ( (QDesignerMenuBar*)( (QMainWindow*)parentWidget() )->menuBar() )->hidePopups();
    ( (QDesignerMenuBar*)( (QMainWindow*)parentWidget() )->menuBar() )->activateItemAt( -1 );
    indicator->hide();
    popup( p );
}

#endif

void QDesignerPopupMenu::reInsert()
{
    clear();
    for ( QAction *a = actionList.first(); a; a = actionList.next() )
	a->addTo( this );
}

void QDesignerPopupMenu::drawIndicator( const QPoint &pos )
{
    if ( lastIndicatorPos == pos )
	return;
    bool wasVsisible = indicator->isVisible();
    indicator->resize( width(), 3 );
    indicator->move( 0, pos.y() - 1 );
    indicator->show();
    indicator->raise();
    lastIndicatorPos = pos;
    if ( !wasVsisible )
	QApplication::sendPostedEvents();
}

QPoint QDesignerPopupMenu::calcIndicatorPos( const QPoint &pos )
{
    int h = frameWidth();
    insertAt = count();
    for ( int i = 0; i < (int)count(); ++i ) {
	QRect r = itemGeometry( i );
	if ( pos.y() < h + r.height() / 2 ) {
	    insertAt = i;
	    break;
	}
	h += r.height();
    }

    return QPoint( 0, h );
}

void QDesignerPopupMenu::addAction( QAction *a )
{
    actionList.append( a );
    connect( a, SIGNAL( destroyed() ), this, SLOT( actionRemoved() ) );
}

void QDesignerPopupMenu::actionRemoved()
{
    actionList.removeRef( (QAction*)sender() );
}

void QDesignerPopupMenu::paintEvent( QPaintEvent *e )
{
    QPopupMenu::paintEvent( e );
    if ( e->rect() != rect() )
	return;
    lastIndicatorPos = QPoint( -1, -1 );
}

void QDesignerPopupMenu::findFormWindow()
{
    QWidget *w = this;
    while ( w ) {
	if ( w->inherits( "FormWindow" ) )
	    formWindow = (FormWindow*)w;
	w = w->parentWidget();
    }
}

#include "actiondnd.moc"
