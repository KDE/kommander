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

#include "formsettingsimpl.h"
#include "formwindow.h"
#include "metadatabase.h"
#include "command.h"
#include "asciivalidator.h"
#include "mainwindow.h"
#ifndef KOMMANDER
#include "project.h"
#endif

#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qspinbox.h>

FormSettings::FormSettings( QWidget *parent, FormWindow *fw )
    : FormSettingsBase( parent, 0, TRUE ), formwindow( fw )
{
    connect( buttonHelp, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
    MetaDataBase::MetaInfo info = MetaDataBase::metaInfo( fw );
    if ( info.classNameChanged && !info.className.isEmpty() )
	editClassName->setText( info.className );
    else
	editClassName->setText( fw->name() );
    editComment->setText( info.comment );
    editAuthor->setText( info.author );

    editClassName->setValidator( new AsciiValidator( editClassName ) );
    editPixmapFunction->setValidator( new AsciiValidator( QString( ":" ), editClassName ) );

    if ( formwindow->savePixmapInline() )
	radioPixmapInline->setChecked( TRUE );
#ifndef KOMMANDER
    else if ( formwindow->savePixmapInProject() )
	radioProjectImageFile->setChecked( TRUE );
#endif
    else
	radioPixmapFunction->setChecked( TRUE );
    editPixmapFunction->setText( formwindow->pixmapLoaderFunction() );
#ifndef KOMMANDER
    radioProjectImageFile->setEnabled( !fw->project()->isDummy() );
#endif
    spinSpacing->setValue( formwindow->layoutDefaultSpacing() );
    spinMargin->setValue( formwindow->layoutDefaultMargin() );
}

void FormSettings::okClicked()
{
    MetaDataBase::MetaInfo info;
    info.className = editClassName->text();
    info.classNameChanged = info.className != QString( formwindow->name() );
    info.comment = editComment->text();
    info.author = editAuthor->text();
    MetaDataBase::setMetaInfo( formwindow, info );

    formwindow->commandHistory()->setModified( TRUE );

    if ( formwindow->savePixmapInline() ) {
	MetaDataBase::clearPixmapArguments( formwindow );
	MetaDataBase::clearPixmapKeys( formwindow );
    }
#ifndef KOMMANDER
    else if ( formwindow->savePixmapInProject() ) 
    {
	MetaDataBase::clearPixmapArguments( formwindow );
    }
#endif
    else {
	MetaDataBase::clearPixmapKeys( formwindow );
    }

    if ( radioPixmapInline->isChecked() ) {
	formwindow->setSavePixmapInline( TRUE );
#ifndef KOMMANDER
	formwindow->setSavePixmapInProject( FALSE );
#endif
	}
     else if ( radioProjectImageFile->isChecked() ){
	formwindow->setSavePixmapInline( FALSE );
    }
    else {
	formwindow->setSavePixmapInline( FALSE );
#ifndef KOMMANDER
	formwindow->setSavePixmapInProject( FALSE );
#endif
    }

    formwindow->setPixmapLoaderFunction( editPixmapFunction->text() );
    formwindow->setLayoutDefaultSpacing( spinSpacing->value() );
    formwindow->setLayoutDefaultMargin( spinMargin->value() );

    accept();
}

#include "formsettingsimpl.moc"
