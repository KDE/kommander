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

#include "actioneditorimpl.h"
#include "formwindow.h"
#include "metadatabase.h"
#include "actionlistview.h"
#include "connectioneditorimpl.h"
#include "mainwindow.h"

#include <qaction.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <qlistview.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qobjectlist.h>

#include <klocale.h>

ActionEditor::ActionEditor( QWidget* parent,  const char* name, WFlags fl )
    : ActionEditorBase( parent, name, fl ), currentAction( 0 ), formWindow( 0 )
{
    listActions->addColumn( i18n( "Actions" ) );
    setEnabled( FALSE );
    buttonConnect->setEnabled( FALSE );

    QPopupMenu *popup = new QPopupMenu( this );
    popup->insertItem( i18n( "New &Action" ), this, SLOT( newAction() ) );
    popup->insertItem( i18n( "New Action &Group" ), this, SLOT( newActionGroup() ) );
    popup->insertItem( i18n( "New &Dropdown Action Group" ), this, SLOT( newDropDownActionGroup() ) );
    buttonNewAction->setPopup( popup );
    buttonNewAction->setPopupDelay( 0 );

    connect( listActions, SIGNAL( insertAction() ), this, SLOT( newAction() ) );
    connect( listActions, SIGNAL( insertActionGroup() ), this, SLOT( newActionGroup() ) );
    connect( listActions, SIGNAL( insertDropDownActionGroup() ), this, SLOT( newDropDownActionGroup() ) );
    connect( listActions, SIGNAL( deleteAction() ), this, SLOT( deleteAction() ) );
    connect( listActions, SIGNAL( connectAction() ), this, SLOT( connectionsClicked() ) );
}

void ActionEditor::closeEvent( QCloseEvent *e )
{
    emit hidden();
    e->accept();
}

void ActionEditor::currentActionChanged( QListViewItem *i )
{
    buttonConnect->setEnabled( i != 0 );
    if ( !i )
	return;
    currentAction = ( (ActionItem*)i )->action();
    if ( !currentAction )
	currentAction = ( (ActionItem*)i )->actionGroup();
    if ( formWindow && currentAction )
	formWindow->setActiveObject( currentAction );
}

void ActionEditor::deleteAction()
{
    if ( !currentAction )
	return;

    QListViewItemIterator it( listActions );
    while ( it.current() ) {
	if ( ( (ActionItem*)it.current() )->action() == currentAction ) {
	    formWindow->actionList().removeRef( currentAction );
	    delete currentAction;
	    QValueList<MetaDataBase::Connection> conns =
		MetaDataBase::connections( formWindow, currentAction );
	    for ( QValueList<MetaDataBase::Connection>::Iterator it2 = conns.begin();
		  it2 != conns.end(); ++it2 )
		MetaDataBase::removeConnection( formWindow, (*it2).sender, (*it2).signal,
						(*it2).receiver, (*it2).slot );
	    delete it.current();
	    break;
	} else if ( ( (ActionItem*)it.current() )->actionGroup() == currentAction ) {
	    formWindow->actionList().removeRef( currentAction );
	    delete currentAction;
	    QValueList<MetaDataBase::Connection> conns =
		MetaDataBase::connections( formWindow, currentAction );
	    for ( QValueList<MetaDataBase::Connection>::Iterator it2 = conns.begin();
		  it2 != conns.end(); ++it2 )
		MetaDataBase::removeConnection( formWindow, (*it2).sender, (*it2).signal,
						(*it2).receiver, (*it2).slot );
	    delete it.current();
	    break;
	}
	++it;
    }

    if ( formWindow )
	formWindow->setActiveObject( formWindow->mainContainer() );
}

void ActionEditor::newAction()
{
    ActionItem *actionParent = (ActionItem*)listActions->selectedItem();
    if ( actionParent ) {
	if ( !actionParent->actionGroup() ||
	     !actionParent->actionGroup()->inherits( "QActionGroup" ) )
	    actionParent = (ActionItem*)actionParent->parent();
    }

    ActionItem *i = 0;
    if ( actionParent )
	i = new ActionItem( actionParent );
    else
	i = new ActionItem( listActions, (bool)FALSE );
    MetaDataBase::addEntry( i->action() );
    QString n = "Action";
    formWindow->unify( i->action(), n, TRUE );
    i->setText( 0, n );
    i->action()->setName( n );
    i->action()->setText( i->action()->name() );
    if ( actionParent && actionParent->actionGroup() &&
	 actionParent->actionGroup()->usesDropDown() ) {
	i->action()->setToggleAction( TRUE );
	MetaDataBase::setPropertyChanged( i->action(), "toggleAction", TRUE );
    }
    MetaDataBase::setPropertyChanged( i->action(), "text", TRUE );
    MetaDataBase::setPropertyChanged( i->action(), "name", TRUE );
    listActions->setCurrentItem( i );
    if ( !actionParent )
	formWindow->actionList().append( i->action() );
}

void ActionEditor::newActionGroup()
{
    ActionItem *actionParent = (ActionItem*)listActions->selectedItem();
    if ( actionParent ) {
	if ( !actionParent->actionGroup() ||
	     !actionParent->actionGroup()->inherits( "QActionGroup" ) )
	    actionParent = (ActionItem*)actionParent->parent();
    }

    ActionItem *i = 0;
    if ( actionParent )
	i = new ActionItem( actionParent, TRUE );
    else
	i = new ActionItem( listActions, TRUE );

    MetaDataBase::addEntry( i->actionGroup() );
    MetaDataBase::setPropertyChanged( i->actionGroup(), "usesDropDown", TRUE );
    QString n = "ActionGroup";
    formWindow->unify( i->action(), n, TRUE );
    i->setText( 0, n );
    i->actionGroup()->setName( n );
    i->actionGroup()->setText( i->actionGroup()->name() );
    MetaDataBase::setPropertyChanged( i->actionGroup(), "text", TRUE );
    MetaDataBase::setPropertyChanged( i->actionGroup(), "name", TRUE );
    listActions->setCurrentItem( i );
    i->setOpen( TRUE );
    if ( !actionParent )
	formWindow->actionList().append( i->actionGroup() );
}

void ActionEditor::newDropDownActionGroup()
{
    newActionGroup();
    ( (ActionItem*)listActions->currentItem() )->actionGroup()->setUsesDropDown( TRUE );
}

void ActionEditor::setFormWindow( FormWindow *fw )
{
    listActions->clear();
    formWindow = fw;
    if ( !formWindow ||
	 !formWindow->mainContainer() ||
	 !formWindow->mainContainer()->inherits( "QMainWindow" ) ) {
	setEnabled( FALSE );
    } else {
	setEnabled( TRUE );
	for ( QAction *a = formWindow->actionList().first(); a; a = formWindow->actionList().next() ) {
	    ActionItem *i = 0;
	    if ( a->parent() && a->parent()->inherits( "QAction" ) )
		continue;
	    i = new ActionItem( listActions, a );
	    i->setText( 0, a->name() );
	    i->setPixmap( 0, a->iconSet().pixmap() );
	    if ( a->inherits( "QActionGroup" ) ) {
		insertChildActions( i );
	    }
	}
	if ( listActions->firstChild() ) {
	    listActions->setCurrentItem( listActions->firstChild() );
	    listActions->setSelected( listActions->firstChild(), TRUE );
	}
    }
}

void ActionEditor::insertChildActions( ActionItem *i )
{
    if ( !i->actionGroup() || !i->actionGroup()->children() )
	return;
    QObjectListIt it( *i->actionGroup()->children() );
    while ( it.current() ) {
	QObject *o = it.current();
	++it;
	if ( !o->inherits( "QAction" ) )
	    continue;
	QAction *a = (QAction*)o;
	ActionItem *i2 = new ActionItem( (QListViewItem*)i, a );
	i->setOpen( TRUE );
	i2->setText( 0, a->name() );
	i2->setPixmap( 0, a->iconSet().pixmap() );
	if ( a->inherits( "QActionGroup" ) )
	    insertChildActions( i2 );
    }
}

void ActionEditor::updateActionName( QAction *a )
{
    QListViewItemIterator it( listActions );
    while ( it.current() ) {
	if ( ( (ActionItem*)it.current() )->action() == a )
	    ( (ActionItem*)it.current() )->setText( 0, a->name() );
	else if ( ( (ActionItem*)it.current() )->actionGroup() == a )
	    ( (ActionItem*)it.current() )->setText( 0, a->name() );
	++it;
    }
}

void ActionEditor::updateActionIcon( QAction *a )
{
    QListViewItemIterator it( listActions );
    while ( it.current() ) {
	if ( ( (ActionItem*)it.current() )->action() == a )
	    ( (ActionItem*)it.current() )->setPixmap( 0, a->iconSet().pixmap() );
	else if ( ( (ActionItem*)it.current() )->actionGroup() == a )
	    ( (ActionItem*)it.current() )->setPixmap( 0, a->iconSet().pixmap() );
	++it;
    }
}

void ActionEditor::connectionsClicked()
{
    ConnectionEditor editor( formWindow->mainWindow(), currentAction, formWindow, formWindow );
    editor.exec();
}
#include "actioneditorimpl.moc"
