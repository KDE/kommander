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

/* QT INCLUDES */
#include <qlabel.h>
#include <qstringlist.h>

/* KDE INCLUDES */
#include <kcombobox.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klistbox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <ktextbrowser.h>

/* OTHER INCLUDES */ 
#include "functionsimpl.h"

 
FunctionsDialog::FunctionsDialog(QWidget* a_parent, char* a_name, bool a_modal)
   : FunctionsDialogBase(a_parent, a_name, a_modal)
{
  clearButton->setPixmap(KGlobal::iconLoader()->loadIcon("locationbar_erase", KIcon::Toolbar));
  copyButton->setPixmap(KGlobal::iconLoader()->loadIcon("1downarrow", KIcon::Toolbar));
  
  groupComboBox->insertStringList(SpecialInformation::groups());
  connect(groupComboBox, SIGNAL(activated(int)), SLOT(groupChanged(int)));
  connect(functionListBox, SIGNAL(highlighted(int)), SLOT(functionChanged(int)));
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
    return QString("%1(%2)").arg(functionListBox->currentText()).arg(params());
  else if (groupComboBox->currentText() == "DCOP")
    return QString("@%1.%2(%3)").arg(widgetComboBox->currentText().section(' ', 0, 0))
      .arg(functionListBox->currentText()).arg(params());
  else 
    return QString("@%1.%2(%3)").arg(groupComboBox->currentText())
      .arg(functionListBox->currentText()).arg(params());
}

void FunctionsDialog::groupChanged(int index)
{
  functionListBox->clear();
  functionListBox->insertStringList(SpecialInformation::functions(groupComboBox->text(index)));
  functionListBox->setCurrentItem(0);
  functionChanged(functionListBox->currentItem());
}

void FunctionsDialog::functionChanged(int)
{
  m_function = SpecialInformation::functionObject(groupComboBox->currentText(),
      functionListBox->currentText());
  QString defArgs;
  if (m_function.minArg() < m_function.maxArg()) 
     if (!m_function.minArg())
        defArgs = i18n("<p>Parameters are not obligatory.");
     else    
        defArgs = i18n("<p>Only first argument is obligatory.", 
           "<p>Only first %1 arguments are obligatory.", 
           m_function.minArg()).arg(m_function.minArg());
  
  descriptionText->clear();
  descriptionText->setText(QString("<qt><h3>%1</h3>"
    "<p><b>Description:</b> %2"
    "<p><b>Syntax:</b> <i>%3</i>%4</qt>").arg(m_function.name()).arg(m_function.description()).
    arg(m_function.longPrototype()).arg(defArgs));
  
  showParameters();
}

void FunctionsDialog::copyText()
{
  QString text = currentFunctionText();
  int cursorPos = insertedText->cursorPosition();
  insertedText->insert(text);
  insertedText->setCursorPosition(cursorPos + text.find('(') + 1);
}

void FunctionsDialog::showParameters()
{
  KLineEdit* edits[3] = {arg1Edit, arg2Edit, arg3Edit};
  QLabel* labels[3] = {argLabel1, argLabel2, argLabel3};
  int start = (m_function.maxArg() && m_function.argumentName(0) == "widget");
  
  widgetComboBox->setShown(start);
  widgetLabel->setShown(start);
  if (start)
  {
    arg1Edit->setShown(false);
    argLabel1->setShown(false);
  }
  for (int i=start; i<3; i++)
  {
    edits[i]->setShown(i<m_function.maxArg());
    edits[i]->clear();
    labels[i]->setShown(i<m_function.maxArg());
    if (i<m_function.maxArg())
      labels[i]->setText(QString("%1:").arg(m_function.argumentName(i)));
  }
}

QString FunctionsDialog::params()
{
  KLineEdit* edits[3] = {arg1Edit, arg2Edit, arg3Edit};
  QStringList pars;
  for (int i=0; i<3; i++)
    if (edits[i]->isShown())
      pars.append(edits[i]->text());
  return pars.join(", ");
}

void FunctionsDialog::setWidgetList(const QStringList& list)
{
  widgetComboBox->clear();
  widgetComboBox->insertStringList(list);
  widgetComboBox->setCurrentItem(0);
}

