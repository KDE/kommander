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

#include "listboxeditorimpl.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "pixmapchooser.h"
#include "command.h"

#include <qlineedit.h>
#include <qlabel.h>
#include <q3listbox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3ValueList>

#include <klocale.h>

ListBoxEditor::ListBoxEditor( QWidget *parent, QWidget *editWidget, FormWindow *fw )
    : ListBoxEditorBase( parent, 0, true ), formwindow( fw )
{
    connect( helpButton, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
    listbox = (Q3ListBox*)editWidget;

    itemText->setText( "" );
    itemText->setEnabled( false );
    itemPixmap->setText( "" );
    itemChoosePixmap->setEnabled( false );
    itemDeletePixmap->setEnabled( false );

    Q3ListBoxItem *i = 0;
    for ( i = listbox->firstItem(); i; i = i->next() ) {
	if ( i->pixmap() )
	    (void)new Q3ListBoxPixmap( preview, *i->pixmap(), i->text() );
	else
	    (void)new Q3ListBoxText( preview, i->text() );
    }

    if ( preview->firstItem() )
	preview->setCurrentItem( preview->firstItem() );
}

void ListBoxEditor::insertNewItem()
{
    Q3ListBoxItem *i = new Q3ListBoxText( preview, i18n("New Item") );
    preview->setCurrentItem( i );
    preview->setSelected( i, true );
}

void ListBoxEditor::deleteCurrentItem()
{
    delete preview->item( preview->currentItem() );
    if ( preview->currentItem() != -1 )
	preview->setSelected( preview->currentItem(), true );
}

void ListBoxEditor::currentItemChanged( Q3ListBoxItem *i )
{
    itemText->blockSignals( true );
    itemText->setText( "" );
    itemPixmap->setText( "" );
    itemText->blockSignals( false );

    if ( !i ) {
	itemText->setEnabled( false );
	itemChoosePixmap->setEnabled( false );
	itemDeletePixmap->setEnabled( false );
	return;
    }

    itemText->blockSignals( true );
    itemText->setEnabled( true );
    itemChoosePixmap->setEnabled( true );
    itemDeletePixmap->setEnabled( i->pixmap() && !i->pixmap()->isNull() );

    itemText->setText( i->text() );
    if ( i->pixmap() )
	itemPixmap->setPixmap( *i->pixmap() );
    itemText->blockSignals( false );
}

void ListBoxEditor::currentTextChanged( const QString &txt )
{
    if ( preview->currentItem() == -1 )
	return;

    preview->blockSignals( true );
    if ( preview->item( preview->currentItem() )->pixmap() )
	preview->changeItem( *preview->item( preview->currentItem() )->pixmap(), txt, preview->currentItem() );
    else
	preview->changeItem( txt, preview->currentItem() );
    preview->blockSignals( false );
}

void ListBoxEditor::okClicked()
{
    applyClicked();
    accept();
}

void ListBoxEditor::cancelClicked()
{
    reject();
}

void ListBoxEditor::applyClicked()
{
    Q3ListBoxItem *i = 0;
    Q3ValueList<PopulateListBoxCommand::Item> items;
    for ( i = preview->firstItem(); i; i = i->next() ) {
	PopulateListBoxCommand::Item item;
	if ( i->pixmap() )
	    item.pix = *i->pixmap();
	item.text = i->text();
	items.append( item );
    }

    PopulateListBoxCommand *cmd = new PopulateListBoxCommand( i18n("Edit Items of '%1'", listbox->name() ),
							      formwindow, listbox, items );
    cmd->execute();
    formwindow->commandHistory()->addCommand( cmd );
}

void ListBoxEditor::choosePixmap()
{
    if ( preview->currentItem() == -1 )
	return;

    QPixmap pix;
    if ( preview->item( preview->currentItem() )->pixmap() )
    	pix = qChoosePixmap( this, formwindow, *preview->item( preview->currentItem() )->pixmap() );
    else
    	pix = qChoosePixmap( this, formwindow, QPixmap() );

    if ( pix.isNull() )
	return;

    QString txt = preview->item( preview->currentItem() )->text();
    preview->changeItem( pix, txt, preview->currentItem() );
    itemDeletePixmap->setEnabled( true );
}

void ListBoxEditor::moveItemUp()
{
    if ( preview->currentItem() < 1 )
	return;

    Q3ListBoxItem *i = preview->item( preview->currentItem() );
    bool hasPix = (i->pixmap() != 0);
    QPixmap pix;
    if ( hasPix )
	pix = *i->pixmap();
    QString txt = i->text();

    Q3ListBoxItem *p = i->prev();
    if ( p->pixmap() )
	preview->changeItem( *p->pixmap(), p->text(), preview->currentItem() );
    else
	preview->changeItem( p->text(), preview->currentItem() );

    if ( hasPix )
	preview->changeItem( pix, txt, preview->currentItem() - 1 );
    else
	preview->changeItem( txt, preview->currentItem() - 1 );
}

void ListBoxEditor::moveItemDown()
{
    if ( preview->currentItem() == -1 || preview->currentItem() > (int)preview->count() - 2 )
	return;

    Q3ListBoxItem *i = preview->item( preview->currentItem() );
    bool hasPix = (i->pixmap() != 0);
    QPixmap pix;
    if ( hasPix )
	pix = *i->pixmap();
    QString txt = i->text();

    Q3ListBoxItem *n = i->next();
    if ( n->pixmap() )
	preview->changeItem( *n->pixmap(), n->text(), preview->currentItem() );
    else
	preview->changeItem( n->text(), preview->currentItem() );

    if ( hasPix )
	preview->changeItem( pix, txt, preview->currentItem() + 1 );
    else
	preview->changeItem( txt, preview->currentItem() + 1 );
}

void ListBoxEditor::deletePixmap()
{
    if ( preview->currentItem() == -1 )
	return;

    Q3ListBoxItem *i = preview->item( preview->currentItem() );
    preview->changeItem( i->text(), preview->currentItem() );
    itemDeletePixmap->setEnabled( false );
}
#include "listboxeditorimpl.moc"
