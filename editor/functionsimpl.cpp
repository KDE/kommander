/***************************************************************************
                          functionsimpl.cpp - Function browser implementation 
                             -------------------
    copyright            : (C) 2004    Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* KDE INCLUDES */
#include <kcombobox.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <ktextbrowser.h>

/* OTHER INCLUDES */ 
#include "functionsimpl.h"
#include "specials.h"

 
FunctionsDialog::FunctionsDialog(QWidget* a_parent, char* a_name, bool a_modal)
   : FunctionsDialogBase(a_parent, a_name, a_modal)
{
  clearButton->setPixmap(KGlobal::iconLoader()->loadIcon("locationbar_erase", KIcon::Toolbar));
  copyButton->setPixmap(KGlobal::iconLoader()->loadIcon("1downarrow", KIcon::Toolbar));
  
  groupComboBox->insertStringList(SpecialInformation::groups());
  connect(groupComboBox, SIGNAL(activated(int)), SLOT(groupChanged(int)));
  connect(functionComboBox, SIGNAL(activated(int)), SLOT(functionChanged(int)));
  connect(copyButton, SIGNAL(clicked()), SLOT(copyText()));
  connect(clearButton, SIGNAL(clicked()), insertedText, SLOT(clear()));
  groupComboBox->setCurrentItem(0);
  groupChanged(groupComboBox->currentItem());
}

FunctionsDialog::~FunctionsDialog()
{
}
  
QString FunctionsDialog::functionText() const
{
  return insertedText->text();
}

QString FunctionsDialog::currentFunctionText()
{
  if (groupComboBox->currentText() == "Kommander")
    return QString("%1()").arg(functionComboBox->currentText());
  else if (groupComboBox->currentText() == "DCOP")
    return QString("@Widget.%1()").arg(functionComboBox->currentText());
  else 
    return QString("@%1.%2()").arg(groupComboBox->currentText())
      .arg(functionComboBox->currentText());
}

void FunctionsDialog::groupChanged(int index)
{
  functionComboBox->clear();
  functionComboBox->insertStringList(SpecialInformation::functions(groupComboBox->text(index)));
  functionComboBox->setCurrentItem(0);
  functionChanged(functionComboBox->currentItem());
}

void FunctionsDialog::functionChanged(int)
{
  SpecialFunction function = SpecialInformation::functionObject(groupComboBox->currentText(),
      functionComboBox->currentText());
  QString defArgs;
  if (function.minArg() < function.maxArg()) 
     if (!function.minArg())
        defArgs = i18n("<p>Parameters are not obligatory.");
     else    
        defArgs = i18n("<p>Only first argument is obligatory.", 
           "<p>Only first %1 arguments are obligatory.", 
           function.minArg()).arg(function.minArg());
  
  descriptionText->clear();
  descriptionText->setText(QString("<qt><h1>%1</h1>"
    "<p><b>Description:</b> %2"
    "<p><b>Syntax:</b> <i>%3</i>%4</qt>").arg(function.name()).arg(function.description()).
    arg(function.longPrototype()).arg(defArgs));
}

void FunctionsDialog::copyText()
{
  QString text = currentFunctionText();
  int cursorPos = insertedText->cursorPosition();
  insertedText->insert(text);
  insertedText->setCursorPosition(cursorPos + text.find('(') + 1);
}

