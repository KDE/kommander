/***************************************************************************
                          functionsimpl.cpp - Function browser implementation 
                             -------------------
    copyright            : (C) 2004    Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or mody  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* QT INCLUDES */
#include <qcheckbox.h>
#include <qlabel.h>
#include <qmetaobject.h>
#include <qstringlist.h>
#include <qregexp.h>

/* KDE INCLUDES */
#include <kcombobox.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klistbox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <ktextbrowser.h>
#include <kdebug.h>

/* OTHER INCLUDES */ 
#include "functionsimpl.h"
#include "kommanderwidget.h"
#include "invokeclass.h"

const int MaxFunctionArgs = 6;

 
FunctionsDialog::FunctionsDialog(QWidget* a_parent, const QDict<QWidget>& a_widgetList, bool useInternalParser, char* a_name, 
  bool a_modal)
  : FunctionsDialogBase(a_parent, a_name, a_modal), m_widgetList(a_widgetList), m_useInternalParser(useInternalParser)
{
  clearButton->setPixmap(KGlobal::iconLoader()->loadIcon("locationbar_erase", KIcon::Toolbar));
  copyButton->setPixmap(KGlobal::iconLoader()->loadIcon("1downarrow", KIcon::Toolbar));

  groupComboBox->insertStringList(SpecialInformation::groups());
  connect(groupComboBox, SIGNAL(activated(int)), SLOT(groupChanged(int)));
  connect(widgetComboBox, SIGNAL(activated(int)), SLOT(groupChanged(int)));
  connect(functionListBox, SIGNAL(highlighted(int)), SLOT(functionChanged(int)));
  connect(copyButton, SIGNAL(clicked()), SLOT(copyText()));
  connect(clearButton, SIGNAL(clicked()), insertedText, SLOT(clear()));

  // build widget name list
  QStringList widgets;
  for (QDictIterator<QWidget> It(m_widgetList); It.current(); ++It)
    widgets.append(It.currentKey());
  widgets.sort();
  widgetComboBox->insertStringList(widgets);

  m_acceptedSlots = InvokeClass::acceptedSlots();
  
  m_DCOP = -1;    // Select DCOP functions by default
  m_Slots = -1;
  for (int i=0; i<groupComboBox->count(); i++)
  {
     if (groupComboBox->text(i) == "DCOP")
     {
       m_DCOP = i; 
     }
     if (groupComboBox->text(i) == i18n("Slots"))
     {
       m_Slots = i; 
     }
  }
  if (!useInternalParser)
    groupComboBox->removeItem(m_Slots);
  groupComboBox->setCurrentItem(m_DCOP);
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
  QString prefix, function;
  if (m_useInternalParser)
  {
    function = SpecialInformation::parserGroupName(groupComboBox->currentText());
    if (!function.isEmpty())
      function += "_";
  }
  else 
  {
    prefix = "@";
    function = groupComboBox->currentText() + ".";
  }
  if (groupComboBox->currentText() == "Kommander")
    return QString("%1%2%3").arg(prefix).arg(functionListBox->currentText()).arg(params());
  else if (groupComboBox->currentItem() == m_DCOP || groupComboBox->currentItem() == m_Slots)
    return QString("%1%2.%3%4").arg(prefix).arg(widgetComboBox->currentText().section(' ', 0, 0))
        .arg(functionListBox->currentText().left(functionListBox->currentText().find('('))).arg(params());
  else 
    return QString("%1%2%3%4").arg(prefix).arg(function)
        .arg(functionListBox->currentText()).arg(params());
}

void FunctionsDialog::groupChanged(int index)
{
  index = groupComboBox->currentItem();
  functionListBox->clear();
  m_slotList.clear();
  if (index == m_Slots && m_useInternalParser)
  {
    KommanderWidget* a_atw = dynamic_cast<KommanderWidget *>(m_widgetList[widgetComboBox->currentText()]);
    QStringList pFunctions = QStringList::fromStrList(a_atw->object()->metaObject()->slotNames(true));
    for (uint i=0; i<pFunctions.count(); i++)
    {
      QString slot = pFunctions[i];
      QString slotArgStr = slot.section(QRegExp("\\(|\\)"), 1);
      if (slotArgStr.isEmpty() || m_acceptedSlots.contains(slotArgStr))
      {
        QString name = slot.remove("()");
        if (!m_slotList.contains(name))
        {
          m_slotList[name] = slot;
          functionListBox->insertItem(name);
        }
      }
    }
  } else
  {
    QStringList pFunctions = SpecialInformation::functions(groupComboBox->text(index));
    KommanderWidget* a_atw = 0;
    if (index == m_DCOP)
      a_atw = dynamic_cast<KommanderWidget *>(m_widgetList[widgetComboBox->currentText()]);
    int pGroup = SpecialInformation::group(groupComboBox->text(index));
    SpecialFunction::ParserType pType = m_useInternalParser 
        ? SpecialFunction::InternalParser : SpecialFunction::MacroParser;
  
    for (uint i=0; i<pFunctions.count(); i++)
    {
      int pFunction = SpecialInformation::function(pGroup, pFunctions[i]);
      if (!SpecialInformation::isValid(pGroup, pFunction, pType))
        continue;
      if (a_atw && !a_atw->isFunctionSupported(pFunction) && !a_atw->isCommonFunction(pFunction))
          continue;
      functionListBox->insertItem(pFunctions[i]);
    }
  }

  functionListBox->sort();
  functionListBox->setCurrentItem(0);
  functionChanged(functionListBox->currentItem());
}

void FunctionsDialog::functionChanged(int)
{
  if (groupComboBox->currentItem() == m_Slots)
  { 
    KommanderWidget* w = dynamic_cast<KommanderWidget *>(m_widgetList[widgetComboBox->currentText()]);
    QObject *o = w->object();
    QString slotHelp = i18n("To learn more about the slot, look at the documentation of the base Qt/KDE class, most probably <i>%1</i>.").arg(o->metaObject()->superClassName() ? QString(o->metaObject()->superClassName()) : QString(o->className()) );
    QString slotName = functionListBox->currentText();
    QString slot = m_slotList[slotName];
    descriptionText->clear();
    descriptionText->setText(i18n("<qt><h3>%1</h3>"
      "<p><b>Description:</b> %2\n<p><b>Syntax:</b> <i>%3</i>%4</qt>")
      .arg(slotName).arg(slotHelp).arg(slot).arg(""));
  } else
  {
    m_function = SpecialInformation::functionObject(groupComboBox->currentText(),
        functionListBox->currentText());
    QString defArgs;
    if (m_function.minArg() < m_function.argumentCount()) 
      if (!m_function.minArg())
          defArgs = i18n("<p>Parameters are not obligatory.");
      else
          defArgs = i18n("<p>Only first argument is obligatory.", 
            "<p>Only first %n arguments are obligatory.", 
            m_function.minArg());
  
    uint pflags = SpecialFunction::ShowArgumentNames;
    if (m_function.maxArg() && m_function.argumentName(0) == "widget")
      pflags = pflags | SpecialFunction::SkipFirstArgument;
  
    descriptionText->clear();
    descriptionText->setText(i18n("<qt><h3>%1</h3>"
      "<p><b>Description:</b> %2\n<p><b>Syntax:</b> <i>%3</i>%4</qt>")
      .arg(functionListBox->currentText()).arg(m_function.description())
      .arg(m_function.prototype(pflags)).arg(defArgs));
  
  }
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
  KLineEdit* edits[MaxFunctionArgs] = {arg1Edit, arg2Edit, arg3Edit, arg4Edit, arg5Edit, arg6Edit};
  QLabel* labels[MaxFunctionArgs] = {argLabel1, argLabel2, argLabel3, argLabel4, argLabel5, argLabel6};
  KComboBox* combos[MaxFunctionArgs] = {combo1, combo2, combo3, combo4, combo5, combo6};
  QCheckBox* quotes[MaxFunctionArgs] = {quote1, quote2, quote3, quote4, quote5, quote6};

  if (groupComboBox->currentItem() == m_Slots)
  {
    widgetComboBox->setShown(true);
    widgetLabel->setShown(true);
    QString slot = m_slotList[functionListBox->currentText()];
    QStringList slotArgs = QStringList::split(',', slot.section(QRegExp("\\(|\\)"), 1), false);
    int argsCount = slotArgs.count();
    for (int i = 0; i < MaxFunctionArgs; i++)
    {
      labels[i]->setShown(i < argsCount);
      QString type;
      if (i < argsCount)
      {
        type = slotArgs[i].remove(QRegExp("\\*|\\&|const\\s"));
        labels[i]->setText(QString("%1:").arg(type));
      }
      quotes[i]->setChecked(true);
      quotes[i]->setShown(false);
      if (type == "bool")
      {
        edits[i]->setShown(false);
        combos[i]->setShown(i < argsCount);
        combos[i]->clear();
        combos[i]->insertItem("true");
        combos[i]->insertItem("false");    
      } else
      {
        combos[i]->setShown(false);
        edits[i]->setShown(i < argsCount);
        edits[i]->clear();
        if (type == "QString")
        {
          quotes[i]->setShown(i < argsCount);
        } 
      }  
    }
  } else
  {
    int start = (m_function.argumentCount() && m_function.argumentName(0) == "widget");
  
    widgetComboBox->setShown(start);
    widgetLabel->setShown(start);
    if (start)
    {
      arg1Edit->setShown(false);
      argLabel1->setShown(false);
      combo1->setShown(false);
      quote1->setShown(false);
    }
    int argsCount = m_function.argumentCount();
    for (int i=start; i<MaxFunctionArgs; i++)
    {
      labels[i]->setShown(i < argsCount);
      if (i < argsCount)
        labels[i]->setText(QString("%1:").arg(m_function.argumentName(i)));
      quotes[i]->setChecked(true);
      quotes[i]->setShown(false);
      if (m_function.argumentType(i) == "bool")
      {
        edits[i]->setShown(false);
        combos[i]->setShown(i < argsCount);
        combos[i]->clear();
        combos[i]->insertItem("true");
        combos[i]->insertItem("false");    
      } else
      {
        combos[i]->setShown(false);
        edits[i]->setShown(i < argsCount);
        edits[i]->clear();
        if (m_function.argumentType(i) == "QString")
        {
          quotes[i]->setShown(i < argsCount);
        } 
      }
    }
  }
}

QString FunctionsDialog::params()
{
  QLabel* labels[MaxFunctionArgs] = {argLabel1, argLabel2, argLabel3, argLabel4, argLabel5, argLabel6};
  KLineEdit* edits[MaxFunctionArgs] = {arg1Edit, arg2Edit, arg3Edit, arg4Edit, arg5Edit, arg6Edit};
  KComboBox* combos[MaxFunctionArgs] = {combo1, combo2, combo3, combo4, combo5, combo6};
  QStringList pars;
  QCheckBox* quotes[MaxFunctionArgs] = {quote1, quote2, quote3, quote4, quote5, quote6};
  bool params = false;
  bool slotsShown = (groupComboBox->currentItem() == m_Slots);
  for (int i=0; i<MaxFunctionArgs; i++)
  {
    if (edits[i]->isShown())
    {
      QString s = edits[i]->text();
      if (quotes[i]->isChecked() && ( (!slotsShown && m_function.argumentType(i) == "QString") 
            || (slotsShown && labels[i]->text().startsWith("QString")) ) )
        s = '"' + s + '"';
      pars.append(s);
      params = true;
    } else
    if (combos[i]->isShown())
    {
      pars.append(combos[i]->currentText());
      params = true;
    }
  }
  QString a_param = pars.join(", ");
  if (params)
    return QString("(%1)").arg(a_param);
  else
    return a_param;
}


#include "functionsimpl.moc"
