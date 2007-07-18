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

#include "actionlistview.h"
#include <q3dragobject.h>
#include <q3header.h>
//Added by qt3to4:
#include <Q3CString>
#include <Q3PopupMenu>

#include <klocale.h>

ActionListView::ActionListView( QWidget *parent, const char *name )
    : Q3ListView( parent, name )
{
    header()->setStretchEnabled( true );
    setRootIsDecorated( true );
    setSorting( -1 );
    connect( this, SIGNAL( contextMenuRequested( Q3ListViewItem *, const QPoint &, int ) ),
	     this, SLOT( rmbMenu( Q3ListViewItem *, const QPoint & ) ) );
}

ActionItem::ActionItem( Q3ListView *lv, QAction *ac )
    : Q3ListViewItem( lv ), a( 0 ), g( 0 )
{
    if ( ac->inherits( "QActionGroup" ) )
	g = (QDesignerActionGroup*)ac;
    else
	a = (QDesignerAction*)ac;
    setDragEnabled( true );
}

ActionItem::ActionItem( Q3ListViewItem *i, QAction *ac )
    : Q3ListViewItem( i ), a( 0 ), g( 0 )
{
    if ( ac->inherits( "QActionGroup" ) )
	g = (QDesignerActionGroup*)ac;
    else
	a = (QDesignerAction*)ac;
    setDragEnabled( true );
    moveToEnd();
}

void ActionItem::moveToEnd()
{
    Q3ListViewItem *i = this;
    while ( i->nextSibling() )
	i = i->nextSibling();
    if ( i != this )
	moveItem( i );
}

Q3DragObject *ActionListView::dragObject()
{
    ActionItem *i = (ActionItem*)currentItem();
    if ( !i )
	return 0;
    Q3StoredDrag *drag = 0;
    if ( i->action() ) {
	drag = new Q3StoredDrag( "application/x-designer-actions", viewport() );
	QString s = QString::number( (long)i->action() ); // #### huha, that is evil
	drag->setEncodedData( Q3CString( s.toLatin1() ) );
	drag->setPixmap( i->action()->iconSet().pixmap() );
    } else {
	drag = new Q3StoredDrag( "application/x-designer-actiongroup", viewport() );
	QString s = QString::number( (long)i->actionGroup() ); // #### huha, that is evil
	drag->setEncodedData( Q3CString( s.toLatin1() ) );
	drag->setPixmap( i->actionGroup()->iconSet().pixmap() );
    }
    return drag;
}

void ActionListView::rmbMenu( Q3ListViewItem *i, const QPoint &p )
{
    Q3PopupMenu *popup = new Q3PopupMenu( this );
    popup->insertItem( i18n("New &Action" ), 0 );
    popup->insertItem( i18n("New Action &Group" ), 1 );
    popup->insertItem( i18n("New &Dropdown Action Group" ), 2 );
    if ( i ) {
	popup->insertSeparator();
	popup->insertItem( i18n("&Connect Action..." ), 3 );
	popup->insertSeparator();
	popup->insertItem( i18n("Delete Action" ), 4 );
    }
    int res = popup->exec( p );
    if ( res == 0 )
	emit insertAction();
    else if ( res == 1 )
	emit insertActionGroup();
    else if ( res == 2 )
	emit insertDropDownActionGroup();
    else if ( res == 3 )
	emit connectAction();
    else if ( res == 4 )
	emit deleteAction();
}
#include "actionlistview.moc"
