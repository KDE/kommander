/**********************************************************************
 This file is based on Qt Designer, Copyright (C) 2000 Trolltech AS. All rights reserved.

 This file may be distributed and/or modified under the terms of the
 GNU General Public License version 2 as published by the Free Software
 Foundation and appearing in the file LICENSE.GPL included in the
 packaging of this file.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

 See http://www.trolltech.com/gpl/ for GPL licensing information.

 Modified for Kommander:
  (C) 2005      Michal Rudolf <mrudolf@kdewebdev.org>

**********************************************************************/

#include "formsettingsimpl.h"
#include "formwindow.h"
#include "metadatabase.h"
#include "command.h"
#include "asciivalidator.h"
#include "mainwindow.h"

#include <q3multilineedit.h>
#include <klineedit.h>
#include <qspinbox.h>

FormSettings::FormSettings( QWidget *parent, FormWindow *fw )
    : FormSettingsBase( parent, 0, true ), formwindow( fw )
{
  MetaDataBase::MetaInfo info = MetaDataBase::metaInfo(fw);
  editAuthor->setText(info.author);
  editVersion->setText(info.version);
  editLicense->setText(info.license);
  editComment->setText(info.comment);
  spinSpacing->setValue(formwindow->layoutDefaultSpacing());
  spinMargin->setValue(formwindow->layoutDefaultMargin());
}

void FormSettings::okClicked()
{
  MetaDataBase::MetaInfo info;
  info.author = editAuthor->text();
  info.version = editVersion->text();
  info.license = editLicense->text();
  info.comment = editComment->text();
  MetaDataBase::setMetaInfo(formwindow, info);
  formwindow->commandHistory()->setModified(true);

  formwindow->setLayoutDefaultSpacing(spinSpacing->value());
  formwindow->setLayoutDefaultMargin(spinMargin->value());
  accept();
}

#include "formsettingsimpl.moc"

