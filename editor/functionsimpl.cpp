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
#include <klineedit.h>
#include <ktextbrowser.h>

/* OTHER INCLUDES */ 
#include "functionsimpl.h"
#include "specials.h"

 
FunctionsDialog::FunctionsDialog(QWidget* a_parent, char* a_name, bool a_modal)
   : FunctionsDialogBase(a_parent, a_name, a_modal)
{
  groupComboBox->insertStringList(SpecialInformation::groups());
  connect(groupComboBox, SIGNAL(activated(int)), SLOT(groupChanged(int)));
  connect(functionComboBox, SIGNAL(activated(int)), SLOT(functionChanged(int)));
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

void FunctionsDialog::setFunctionText()
{
  if (groupComboBox->currentText() == "Kommander")
     insertedText->setText(QString("%1()").arg(functionComboBox->currentText()));
  else if (groupComboBox->currentText() == "DCOP")
    insertedText->setText(QString("@Widget.%1()").arg(functionComboBox->currentText()));
  else 
    insertedText->setText(QString("%1.%2()").arg(groupComboBox->currentText())
      .arg(functionComboBox->currentText()));
}

void FunctionsDialog::groupChanged(int index)
{
  functionComboBox->clear();
  functionComboBox->insertStringList(SpecialInformation::functions(groupComboBox->text(index)));
  functionComboBox->setCurrentItem(0);
  functionChanged(functionComboBox->currentItem());
}

void FunctionsDialog::functionChanged(int index)
{
  SpecialFunction function = SpecialInformation::functionObject(groupComboBox->currentText(),
      functionComboBox->currentText());
  
  descriptionText->clear();
  descriptionText->setText(QString("<qt><h1>%1</h1>"
    "<p><b>Description:</b> %2"
    "<p><b>Syntax:</b> <i>%3</i></qt>").arg(function.name()).arg(function.description()).
    arg(function.longPrototype()));
   setFunctionText(); 
}

