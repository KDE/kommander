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

#include "editslotsimpl.h"
#include "formwindow.h"
#include "metadatabase.h"
#include "asciivalidator.h"
#include "mainwindow.h"
#include "hierarchyview.h"
#ifndef KOMMANDER
#include "project.h"
#endif
#include "formfile.h"

#include <qlistview.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qstrlist.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>

EditSlots::EditSlots( QWidget *parent, FormWindow *fw )
    : EditSlotsBase( parent, 0, TRUE ), formWindow( fw )
{
#ifndef KOMMANDER
	LanguageInterface *iface = MetaDataBase::languageInterface( fw->project()->language() );
#else
	LanguageInterface *iface = MetaDataBase::languageInterface( "C++" );
#endif
    if ( iface && !iface->supports( LanguageInterface::ReturnType ) ) {
	slotListView->removeColumn( 3 );
	editType->hide();
	labelType->hide();
    }

    connect( helpButton, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
    QValueList<MetaDataBase::Slot> slotList = MetaDataBase::slotList( fw );
    for ( QValueList<MetaDataBase::Slot>::Iterator it = slotList.begin(); it != slotList.end(); ++it ) {
	QListViewItem *i = new QListViewItem( slotListView );
	oldSlotNames.insert( i, QString( (*it).slot ) );
	i->setPixmap( 0, PixmapChooser::loadPixmap( "editslots.xpm" ) );
	i->setText( 0, (*it).slot );
	i->setText( 1, (*it).returnType );
	i->setText( 2, (*it).specifier );
	i->setText( 3, (*it).access  );
	if ( MetaDataBase::isSlotUsed( formWindow, MetaDataBase::normalizeSlot( (*it).slot ).latin1() ) )
	    i->setText( 4, tr( "Yes" ) );
	else
	    i->setText( 4, tr( "No" ) );
    }

    boxProperties->setEnabled( FALSE );
    slotName->setValidator( new AsciiValidator( TRUE, slotName ) );

    if ( slotListView->firstChild() )
	slotListView->setCurrentItem( slotListView->firstChild() );
}

void EditSlots::okClicked()
{
    QValueList<MetaDataBase::Slot> slotList = MetaDataBase::slotList( formWindow );
    MacroCommand *rmSlt = 0, *addSlt = 0;
    QString n = tr( "Add/Remove slots of '%1'" ).arg( formWindow->name() );
    QValueList<MetaDataBase::Slot>::Iterator sit;
    if ( !slotList.isEmpty() ) {
	QPtrList<Command> commands;
	for ( sit = slotList.begin(); sit != slotList.end(); ++sit ) {
	    commands.append( new RemoveSlotCommand( tr( "Remove slot" ),
						    formWindow, (*sit).slot, (*sit).specifier, (*sit).access,
#ifndef KOMMANDER
						    formWindow->project()->language(), (*sit).returnType ) );
#else
						    "C++",(*sit).returnType ) );
#endif
	}
	rmSlt = new MacroCommand( tr( "Remove slots" ), formWindow, commands );
    }

    bool invalidSlots = FALSE;
    QPtrList<QListViewItem> invalidItems;
    if ( slotListView->firstChild() ) {
	QPtrList<Command> commands;
	QListViewItemIterator it( slotListView );
	QStrList lst;
	for ( ; it.current(); ++it ) {
	    MetaDataBase::Slot slot;
	    slot.slot = it.current()->text( 0 );
	    slot.returnType = it.current()->text( 1 );
	    slot.specifier = it.current()->text( 2 );
	    slot.access = it.current()->text( 3 );
#ifndef KOMMANDER
	    slot.language = formWindow->project()->language();
#else
	    slot.language = "C++";
#endif
	    if ( slot.returnType.isEmpty() )
		slot.returnType = "void";
	    QString s = slot.slot;
	    s = s.simplifyWhiteSpace();
	    bool startNum = s[ 0 ] >= '0' && s[ 0 ] <= '9';
	    bool noParens = s.contains( '(' ) != 1 || s.contains( ')' ) != 1;
	    bool illegalSpace = s.find( ' ' ) != -1 && s.find( ' ' ) < s.find( '(' );

	    if ( startNum || noParens || illegalSpace || lst.find( slot.slot ) != -1 ) {
		invalidSlots = TRUE;
		invalidItems.append( it.current() );
		continue;
	    }
	    commands.append( new AddSlotCommand( tr( "Add slot" ),
						 formWindow, slot.slot, slot.specifier, slot.access,
#ifndef KOMMANDER
						 formWindow->project()->language(),
#else
						 "C++",
#endif
						 slot.returnType ) );
	    QMap<QListViewItem*, QString>::Iterator sit = oldSlotNames.find( it.current() );
	    if ( sit != oldSlotNames.end() ) {
		if ( *sit != it.current()->text( 0 ) )
		    MetaDataBase::functionNameChanged( formWindow, *sit, it.current()->text( 0 ) );
	    }
	    lst.append( slot.slot );
	}

	if ( !commands.isEmpty() )
	    addSlt = new MacroCommand( tr( "Add slots" ), formWindow, commands );
    }

    if ( invalidSlots ) {
	if ( QMessageBox::information( this, tr( "Edit Slots" ),
					     tr( "Some syntactically incorrect slots have been defined.\n"
						 "Remove these slots?" ),
				       tr( "&Yes" ), tr( "&No" ) ) == 0 ) {
	    QListViewItemIterator it( slotListView );
	    QListViewItem *i;
	    while ( (i = it.current() ) ) {
		++it;
		if ( invalidItems.findRef( i ) != -1 )
		    delete i;
	    }
	    if ( slotListView->firstChild() ) {
		slotListView->setCurrentItem( slotListView->firstChild() );
		slotListView->setSelected( slotListView->firstChild(), TRUE );
	    }
	}
#ifndef KOMMANDER
	formWindow->mainWindow()->objectHierarchy()->updateFormDefinitionView();
#endif
	return;
    }

    for ( QStringList::Iterator rsit = removedSlots.begin(); rsit != removedSlots.end(); ++rsit )
	removeSlotFromCode( *rsit, formWindow );

    if ( rmSlt || addSlt ) {
	QPtrList<Command> commands;
	if ( rmSlt )
	    commands.append( rmSlt );
	if ( addSlt )
	    commands.append( addSlt );
	MacroCommand *cmd = new MacroCommand( n, formWindow, commands );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    }

#ifndef KOMMANDER
	formWindow->mainWindow()->objectHierarchy()->updateFormDefinitionView();
#endif
    accept();
}

void EditSlots::slotAdd( const QString& access )
{
    QListViewItem *i = new QListViewItem( slotListView );
    i->setPixmap( 0, PixmapChooser::loadPixmap( "editslots.xpm" ) );
    i->setText( 0, "newSlot()" );
    i->setText( 1, "void" );
    i->setText( 2, "virtual" );
    if ( access.isEmpty() )
	i->setText( 3, "public" );
    else
	i->setText( 3, access );
    if ( MetaDataBase::isSlotUsed( formWindow, "newSlot()" ) )
	i->setText( 4, tr( "Yes" ) );
    else
	i->setText( 4, tr( "No" ) );
    slotListView->setCurrentItem( i );
    slotListView->setSelected( i, TRUE );
    slotName->setFocus();
    slotName->selectAll();
}

void EditSlots::slotRemove()
{
    if ( !slotListView->currentItem() )
	return;

    slotListView->blockSignals( TRUE );
    removedSlots << MetaDataBase::normalizeSlot( slotListView->currentItem()->text( 0 ) );
    delete slotListView->currentItem();
    if ( slotListView->currentItem() )
	slotListView->setSelected( slotListView->currentItem(), TRUE );
    slotListView->blockSignals( FALSE );
    currentItemChanged( slotListView->currentItem() );
}

void EditSlots::currentItemChanged( QListViewItem *i )
{
    slotName->blockSignals( TRUE );
    slotName->setText( "" );
    slotAccess->setCurrentItem( 0 );
    slotName->blockSignals( FALSE );

    if ( !i ) {
	boxProperties->setEnabled( FALSE );
	return;
    }

    slotName->blockSignals( TRUE );
    slotName->setText( i->text( 0 ) );
    editType->setText( i->text( 1 ) );
    QString specifier = i->text( 2 );
    QString access = i->text( 3 );
    if ( specifier == "pure virtual" )
	slotSpecifier->setCurrentItem( 2 );
    else if ( specifier == "non virtual" )
	slotSpecifier->setCurrentItem( 0 );
    else
	slotSpecifier->setCurrentItem( 1 );
    if ( access == "private" )
	slotAccess->setCurrentItem( 2 );
    else if ( access == "protected" )
	slotAccess->setCurrentItem( 1 );
    else
	slotAccess->setCurrentItem( 0 );
    slotName->blockSignals( FALSE );
    boxProperties->setEnabled( TRUE );
}

void EditSlots::currentTextChanged( const QString &txt )
{
    if ( !slotListView->currentItem() )
	return;

    slotListView->currentItem()->setText( 0, txt );
    if ( MetaDataBase::isSlotUsed( formWindow, MetaDataBase::normalizeSlot( txt.latin1() ).latin1() ) )
	slotListView->currentItem()->setText( 4, tr( "Yes" ) );
    else
	slotListView->currentItem()->setText( 4, tr( "No" ) );
}

void EditSlots::currentSpecifierChanged( const QString& s )
{
    if ( !slotListView->currentItem() )
	return;

    slotListView->currentItem()->setText( 2, s );
}

void EditSlots::currentAccessChanged( const QString& a )
{
    if ( !slotListView->currentItem() )
	return;
    slotListView->currentItem()->setText( 3, a );
}


void EditSlots::currentTypeChanged( const QString &type )
{
    if ( !slotListView->currentItem() )
	return;

    slotListView->currentItem()->setText( 1, type );
}

void EditSlots::removeSlotFromCode( const QString &slot, FormWindow *formWindow )
{
    formWindow->formFile()->checkTimeStamp();
    QString code = formWindow->formFile()->code();
    if ( code.isEmpty() || !formWindow->formFile()->hasFormCode() )
	return;
#ifndef KOMMANDER
    LanguageInterface *iface = MetaDataBase::languageInterface( formWindow->project()->language() );
#else
    LanguageInterface *iface = MetaDataBase::languageInterface( "C++" );
#endif
    if ( !iface )
	return;
    QValueList<LanguageInterface::Function> functions;
    iface->functions( code, &functions );
    QString sl = MetaDataBase::normalizeSlot( slot );
    for ( QValueList<LanguageInterface::Function>::Iterator fit = functions.begin(); fit != functions.end(); ++fit ) {
	if ( MetaDataBase::normalizeSlot( (*fit).name ) == sl ) {
	    int line = 0;
	    int start = 0;
	    while ( line < (*fit).start - 1 ) {
		start = code.find( '\n', start );
		if ( start == -1 )
		    return;
		start++;
		line++;
	    }
	    if ( start == -1 )
		return;
	    int end = start;
	    while ( line < (*fit).end + 1 ) {
		end = code.find( '\n', end );
		if ( end == -1 ) {
		    if ( line <= (*fit).end )
			end = code.length() - 1;
		    else
			return;
		}
		end++;
		line++;
	    }
	    if ( end < start )
		return;
	    code.remove( start, end - start );
	    formWindow->formFile()->setCode( code );
	}
    }
}

void EditSlots::setCurrentSlot( const QString &slot )
{
    QListViewItemIterator it( slotListView );
    while ( it.current() ) {
	if ( MetaDataBase::normalizeSlot( it.current()->text( 0 ) ) == slot ) {
	    slotListView->setCurrentItem( it.current() );
	    slotListView->setSelected( it.current(), TRUE );
	    currentItemChanged( it.current() );
	    return;
	}
	++it;
    }

}
