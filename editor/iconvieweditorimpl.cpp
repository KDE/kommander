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

#include "iconvieweditorimpl.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "pixmapchooser.h"

#include <qlineedit.h>
#include <qlabel.h>
#include <qiconview.h>
#include <qpushbutton.h>
#include <qcheckbox.h>

#include <klocale.h>

IconViewEditor::IconViewEditor( QWidget *parent, QWidget *editWidget, FormWindow *fw )
    : IconViewEditorBase( parent, 0, true ), formwindow( fw )
{
    connect( buttonHelp, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
    iconview = (QIconView*)editWidget;

    itemText->setText( "" );
    itemText->setEnabled( false );
    itemPixmap->setText( "" );
    itemChoosePixmap->setEnabled( false );
    itemDeletePixmap->setEnabled( false );

    QIconViewItem *i = 0;
    for ( i = iconview->firstItem(); i; i = i->nextItem() ) {
	(void)new QIconViewItem( preview, i->text(), *i->pixmap()  );
    }

    if ( preview->firstItem() )
	preview->setCurrentItem( preview->firstItem() );
}

void IconViewEditor::insertNewItem()
{
    QIconViewItem *i = new QIconViewItem( preview, i18n("New Item" ) );
    preview->setCurrentItem( i );
    preview->setSelected( i, true );
}

void IconViewEditor::deleteCurrentItem()
{
    delete preview->currentItem();
    if ( preview->currentItem() )
	preview->setSelected( preview->currentItem(), true );
}

void IconViewEditor::currentItemChanged( QIconViewItem *i )
{
    itemText->blockSignals( true );
    itemText->setText( "" );
    itemPixmap->setText( "" );
    itemText->blockSignals( false );

    if ( !i ) {
	itemText->setEnabled( false );
	itemChoosePixmap->setEnabled( false );
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

void IconViewEditor::currentTextChanged( const QString &txt )
{
    if ( !preview->currentItem() )
	return;

    preview->currentItem()->setText( txt );
}

void IconViewEditor::okClicked()
{
    applyClicked();
    accept();
}

void IconViewEditor::cancelClicked()
{
    reject();
}

void IconViewEditor::applyClicked()
{
    QIconViewItem *i = 0;
    QValueList<PopulateIconViewCommand::Item> items;
    for ( i = preview->firstItem(); i; i = i->nextItem() ) {
	PopulateIconViewCommand::Item item;
	if ( i->pixmap() )
	    item.pix = *i->pixmap();
	item.text = i->text();
	items.append( item );
    }

    PopulateIconViewCommand *cmd = new PopulateIconViewCommand( i18n("Edit Items of '%1'" ).arg( iconview->name() ),
								formwindow, iconview, items );
    cmd->execute();
    formwindow->commandHistory()->addCommand( cmd );
}

void IconViewEditor::choosePixmap()
{
    if ( !preview->currentItem() )
	return;

    QPixmap pix;
    if ( preview->currentItem()->pixmap() )
    	pix = qChoosePixmap( this, formwindow, *preview->currentItem()->pixmap() );
    else
    	pix = qChoosePixmap( this, formwindow, QPixmap() );

    if ( pix.isNull() )
	return;

    preview->currentItem()->setPixmap( pix );
    itemPixmap->setPixmap( pix );
    itemDeletePixmap->setEnabled( true );
}

void IconViewEditor::deletePixmap()
{
    if ( !preview->currentItem() )
	return;

    preview->currentItem()->setPixmap( QPixmap() );
    itemPixmap->setText( "" );
    itemDeletePixmap->setEnabled( false );
}
#include "iconvieweditorimpl.moc"
