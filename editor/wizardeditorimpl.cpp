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

#include "wizardeditorimpl.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "pixmapchooser.h"
#include "command.h"

#include <qwizard.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qinputdialog.h>

#include <klocale.h>

WizardEditor::WizardEditor( QWidget *parent, QWizard *w, FormWindow *fw )
    : WizardEditorBase( parent, 0 ), formwindow( fw ), wizard( w )
{
    connect( buttonHelp, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
    fillListBox();
}

WizardEditor::~WizardEditor()
{
    commands.setAutoDelete( TRUE );
}

void WizardEditor::okClicked()
{
    applyClicked();
    accept();
}

void WizardEditor::cancelClicked()
{
    reject();
}

void WizardEditor::applyClicked()
{
    if ( commands.isEmpty() ) return;

    // schedule macro command
    MacroCommand* cmd = new MacroCommand( i18n( "Edit Wizard Pages" ), formwindow, commands );
    formwindow->commandHistory()->addCommand( cmd );
    cmd->execute();

    // clear command list
    commands.clear();

    // fix wizard buttons
    for ( int i = 0; i < wizard->pageCount(); i++ ) {

	QWidget * page = wizard->page( i );
	if ( i == 0 ) { // first page

	    wizard->setBackEnabled( page, FALSE );
	    wizard->setNextEnabled( page, TRUE );
	}
	else if ( i == wizard->pageCount() - 1 ) { // last page

	    wizard->setBackEnabled( page, TRUE );
	    wizard->setNextEnabled( page, FALSE );
	}
	else {

	    wizard->setBackEnabled( page, TRUE );
	    wizard->setNextEnabled( page, TRUE );
	}
	wizard->setFinishEnabled( page, FALSE );
    }

    // update listbox
    int index = listBox->currentItem();
    fillListBox();
    listBox->setCurrentItem( index );

    // show current page
    wizard->showPage( wizard->page( 0 ) );
}

void WizardEditor::helpClicked()
{

}

void WizardEditor::addClicked()
{
    int index = listBox->currentItem() + 1;
    // update listbox
    listBox->insertItem( "Page", index );

    // schedule add command
    AddWizardPageCommand *cmd = new AddWizardPageCommand( i18n( "Add Page to %1" ).arg( wizard->name() ),
							  formwindow, wizard, "Page", index, FALSE);
    commands.append( cmd );

    // update buttons
    updateButtons();
}

void WizardEditor::removeClicked()
{
    if (listBox->count() < 2 ) return;

    int index = listBox->currentItem();

    // update listbox
    listBox->removeItem( index);

    // schedule remove command
    DeleteWizardPageCommand *cmd = new DeleteWizardPageCommand( i18n( "Delete Page %1 of %2" )
								.arg( listBox->text( index ) ).arg( wizard->name() ),
								formwindow, wizard, index, FALSE );
    commands.append( cmd );

    // update buttons
    updateButtons();
}

void WizardEditor::upClicked()
{
    int index1 = listBox->currentItem();
    int index2 = index1 - 1;

    // swap listbox items
    QString item1 = listBox->text( index1 );
    listBox->removeItem( index1 );
    listBox->insertItem( item1, index2 );
    listBox->setCurrentItem( index2 );

    // schedule swap command
    SwapWizardPagesCommand *cmd = new SwapWizardPagesCommand( i18n( "Swap pages %1 and %2 of %1" ).arg( index1 ).arg( index2 )
							     .arg( wizard->name() ), formwindow, wizard, index1, index2);
    commands.append( cmd );

    // update buttons
    updateButtons();
}

void WizardEditor::downClicked()
{
    int index1 = listBox->currentItem();
    int index2 = index1 + 1;

    // swap listbox items
    QString item1 = listBox->text( index1 );
    listBox->removeItem( index1 );
    listBox->insertItem( item1, index2 );
    listBox->setCurrentItem( index2 );

    // schedule swap command
    SwapWizardPagesCommand *cmd = new SwapWizardPagesCommand( i18n( "Swap pages %1 and %2 of %1" ).arg( index1 ).arg( index2 )
							     .arg( wizard->name() ), formwindow, wizard, index2, index1);
    commands.append( cmd );

    // update buttons
    updateButtons();
}

void WizardEditor::fillListBox()
{
    listBox->clear();

    if ( !wizard ) return;
    for ( int i = 0; i < wizard->pageCount(); i++ )
	listBox->insertItem( wizard->title( wizard->page( i ) ) );

    updateButtons();
}

void WizardEditor::itemHighlighted( int )
{
    updateButtons();
}

void WizardEditor::itemSelected( int index )
{
    if ( index < 0 ) return;

    bool ok = FALSE;
    QString text = QInputDialog::getText( i18n("Page Title"), i18n( "New page title" ), QLineEdit::Normal, listBox->text( index ), &ok, this );
    if ( ok ) {
	QString pn( i18n( "Rename page %1 of %2" ).arg( listBox->text( index ) ).arg( wizard->name() ) );
	RenameWizardPageCommand *cmd = new RenameWizardPageCommand( pn, formwindow, wizard, index, text );
	commands.append( cmd );
	listBox->changeItem( text, index );
    }
}

void WizardEditor::updateButtons()
{
    int index = listBox->currentItem();

    buttonUp->setEnabled( index > 0 );
    buttonDown->setEnabled( index < (int)listBox->count() - 1 );
    buttonRemove->setEnabled( index >= 0 );

    if ( listBox->count() < 2 )
	buttonRemove->setEnabled( FALSE );
}
#include "wizardeditorimpl.moc"
