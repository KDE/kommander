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

#include "multilineeditorimpl.h"
#include "formwindow.h"
#include "command.h"
#include "mainwindow.h"

#include <qmultilineedit.h>
#include <qpushbutton.h>

#include <klocale.h>

MultiLineEditor::MultiLineEditor( QWidget *parent, QWidget *editWidget, FormWindow *fw )
    : MultiLineEditorBase( parent, 0, true ), formwindow( fw )
{
    connect( buttonHelp, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
    mlined = (QMultiLineEdit*)editWidget;
    // #### complete list of properties here
    preview->setAlignment( mlined->alignment() );
    preview->setMaxLines( mlined->maxLines() );
    preview->setWordWrap( mlined->wordWrap() );
    preview->setWrapColumnOrWidth( mlined->wrapColumnOrWidth() );
    preview->setWrapPolicy( mlined->wrapPolicy() );
    //preview->setTextFormat( mlined->textFormat() );
    preview->setText( mlined->text() );
    preview->setFocus();
    preview->selectAll();
}

void MultiLineEditor::okClicked()
{
    applyClicked();
    accept();
}

void MultiLineEditor::applyClicked()
{
    PopulateMultiLineEditCommand *cmd = new PopulateMultiLineEditCommand( i18n("Set Text of '%1'" ).arg( mlined->name() ),
									  formwindow, mlined, preview->text() );
    cmd->execute();
    formwindow->commandHistory()->addCommand( cmd );
    preview->setFocus();
}


TextEditor::TextEditor( QWidget *parent, const QString &text )
    : MultiLineEditorBase( parent, 0, true )
{
    buttonApply->hide();
    setCaption( i18n("Text" ) );
    preview->setText( text );
    preview->setFocus();
    preview->selectAll();
}

QString TextEditor::getText( QWidget *parent, const QString &text )
{
    TextEditor dlg( parent, text );
    if ( dlg.exec() == QDialog::Accepted ) {
	QString txt = dlg.preview->text();
	int i = txt.length() - 1;
	while ( i >= 0 &&
		( txt[ i ] == '\n' || txt[ i ] == ' ' || txt[ i ] == '\t' || txt[ i ].isSpace() ) ) {
	    txt.remove( i, 1 );
	    i--;
	}
	return txt;
    }
    return QString::null;
}

void TextEditor::okClicked()
{
    accept();
}

void TextEditor::applyClicked()
{
}
#include "multilineeditorimpl.moc"
