/***************************************************************************
                          assoctexteditorimpl.cpp  - Associated text editor implementation
                             -------------------
    copyright            : (C) 2003    Marc Britton <consume@optusnet.com.au>
                           (C) 2004    Michal Rudolf <mrudolf@kdewebdev.org>
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
#include <klocale.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <ktextedit.h>

/* QT INCLUDES */
#include <qstringlist.h>
#include <qmetaobject.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qfile.h>
#include <qobject.h>
#include <qobject.h>
//Added by qt3to4:
#include <QPixmap>

/* OTHER INCLUDES */
#include <cstdio>
#include "assoctexteditorimpl.h"
#include "kommanderwidget.h"
#include "formwindow.h"
#include "propertyeditor.h"
#include "command.h"
#include "metadatabase.h"
#include "choosewidgetimpl.h"
#include "functionsimpl.h"

AssocTextEditor::AssocTextEditor(QWidget *a_widget, FormWindow* a_form,
    PropertyEditor* a_property, QWidget *a_parent, const char *a_name, bool a_modal)
    : AssocTextEditorBase(a_parent, a_name, a_modal)
{
  // text editor
  associatedTextEdit->setFont(KGlobalSettings::fixedFont());
  associatedTextEdit->setTabStopWidth(associatedTextEdit->fontMetrics().maxWidth() * 3);
  associatedTextEdit->setTextFormat(Qt::PlainText);
  
  // icon for non-empty scripts
  scriptPixmap = KIconLoader::global()->loadIcon("source", KIcon::Small);

  // signals and slots connections
  m_formWindow = a_form;
  m_propertyEditor = a_property;
  m_widget = a_widget;

  // list of widgets that can be edited
  buildWidgetList();
  for (int i=0; i<widgetsComboBox->count(); i++)
    if (widgetFromString(widgetsComboBox->text(i)) == m_widget)
    {
      widgetsComboBox->setCurrentItem(i);
      break;
    }
  setWidget(a_widget);

  connect(associatedTextEdit, SIGNAL(textChanged()), SLOT(textEditChanged()));
  connect(widgetsComboBox, SIGNAL(activated(int)), SLOT(widgetChanged(int)));
  connect(stateComboBox, SIGNAL(activated(int)), SLOT(stateChanged(int)));
  connect(filePushButton, SIGNAL(clicked()), SLOT(insertFile()));
  connect(functionButton, SIGNAL(clicked()), SLOT(insertFunction()));
  connect(widgetComboBox, SIGNAL(activated(int)), SLOT(insertWidgetName(int)));
  connect(treeWidgetButton, SIGNAL(clicked()), SLOT(selectWidget()));
  
  associatedTextEdit->setFocus();
}

AssocTextEditor::~AssocTextEditor()
{
  save();
}

void AssocTextEditor::setWidget(QWidget *a_widget)
{
  KommanderWidget *a_atw = dynamic_cast<KommanderWidget *>(a_widget);
  if (!a_widget || !a_atw)
    return;

  m_widget = a_widget;
  m_states = a_atw->states();
  m_populationText = a_atw->populationText();

  // list of states of current widget (plus population text)
  stateComboBox->clear();
  stateComboBox->insertStringList(a_atw->displayStates());
  stateComboBox->insertItem("population");

  // set states and population scripts
  QStringList at = a_atw->associatedText();
  m_atdict.clear();
  QStringList::iterator at_it = at.begin();
  for(QStringList::ConstIterator s_it = m_states.begin(); s_it != m_states.end(); ++s_it)
  {
    if(at_it != at.end())
    {
      m_atdict[(*s_it)] = (*at_it);
      ++at_it;
    }
    else
      m_atdict[(*s_it)] = QString();
  }
  m_populationText = a_atw->populationText();

  // show pixmaps for nonempty scripts
  int p_population = stateComboBox->count()-1;
  for (int i=0; i<p_population; i++)
    if (!m_atdict[stateComboBox->text(i)].isEmpty())
       stateComboBox->changeItem(scriptPixmap, stateComboBox->text(i), i);
  if (!m_populationText.isEmpty())
    stateComboBox->changeItem(scriptPixmap, stateComboBox->text(p_population), p_population);

  // initial text for initial state
  stateComboBox->setCurrentItem(0);
  m_currentState = stateComboBox->currentText();

  stateChanged(0);

}

void AssocTextEditor::save() const
{
  KommanderWidget *atw = dynamic_cast<KommanderWidget *>(m_widget);
  if (!atw)
    return;

  if (atw->associatedText() != associatedText())
  {
    QString text = i18n("Set the \'text association\' of \'%1\'", m_widget->name());
    SetPropertyCommand *cmd  = new SetPropertyCommand(text, m_formWindow,
        m_widget, m_propertyEditor, "associations", atw->associatedText(),
        associatedText(), QString(), QString(), false);
    cmd->execute();
    m_formWindow->commandHistory()->addCommand(cmd);
    MetaDataBase::setPropertyChanged(m_widget, "associations", true);
  }
  if (atw->populationText() != populationText())
  {
    QString text = i18n("Set the \'population text\' of \'%1\'", m_widget->name());
    SetPropertyCommand *cmd  = new SetPropertyCommand(text, m_formWindow, m_widget,
        m_propertyEditor, "populationText", atw->populationText(),
        populationText(), QString(), QString(), false);
    cmd->execute();
    m_formWindow->commandHistory()->addCommand(cmd);
    MetaDataBase::setPropertyChanged(m_widget, "populationText", true);
  }
}


QStringList AssocTextEditor::associatedText() const
{
  QStringList at;
  for(QStringList::ConstIterator it = m_states.begin(); it != m_states.end(); ++it)
    at.append(m_atdict[(*it)]);
  return at;
}

QString AssocTextEditor::populationText() const
{
  return m_populationText;
}

QStringList AssocTextEditor::buildWidgetList()
{
  QStringList widgetList;
  QObject* thisObject = m_formWindow->mainContainer();
  QObjectListobjectList = thisObject->queryList();
  objectList->prepend(thisObject);

  for (QObjectListIt it(*objectList); it.current(); ++it)
  {
    // There is a warning message with the property() function if it does not exist.
    // Verify the property exists with the meta information first */
    bool pExists = false;
    QMetaObject *metaObj = it.current()->metaObject();
    if(metaObj)
    {
      int id = metaObj->findProperty("KommanderWidget", true);
      const QMetaProperty *metaProp = metaObj->property(id, true);
      if(metaProp && metaProp->isValid())
        pExists = true;
    }
    if(pExists)
    {
      QVariant flag = (it.current())->property("KommanderWidget");
      if(flag.isValid() && !(QString(it.current()->name()).startsWith("qt_")))
      {
        widgetList.append( widgetToString( (QWidget*)it.current()) );
        m_widgetList.insert(it.current()->name(), (QWidget*)it.current());
      }
    }
  }
  delete objectList;

  widgetList.sort();
  widgetComboBox->clear();
  widgetComboBox->insertStringList(widgetList);
  widgetsComboBox->clear();
  widgetsComboBox->insertStringList(widgetList);
  return widgetList;
}

void AssocTextEditor::stateChanged(int a_index)
{
  m_currentState = stateComboBox->text(a_index);
  if (a_index == stateComboBox->count() - 1)
    associatedTextEdit->setText(m_populationText);
  else
    associatedTextEdit->setText(m_atdict[m_currentState]);
}

void AssocTextEditor::textEditChanged()
{
  if (m_currentState == "population")
    m_populationText = associatedTextEdit->text();
  m_atdict[m_currentState] = associatedTextEdit->text();
  int index = stateComboBox->currentItem();
  if (associatedTextEdit->text().isEmpty())
    stateComboBox->changeItem(QPixmap(), stateComboBox->currentText(), index);
  else if (!stateComboBox->pixmap(index) || stateComboBox->pixmap(index)->isNull())
    stateComboBox->changeItem(scriptPixmap, stateComboBox->currentText(), index);
}

void AssocTextEditor::widgetChanged(int index)
{
  save();
  setWidget( widgetFromString( widgetsComboBox->text(index)) ) ;
}

void AssocTextEditor::selectWidget()
{
  ChooseWidget cDialog(this);
  cDialog.setWidget(m_formWindow->mainContainer());
  if (cDialog.exec()) {
    QString newWidget = cDialog.selection();
    for (int i = 0; i<widgetsComboBox->count(); i++)
      if (widgetsComboBox->text(i) == newWidget) {
        widgetsComboBox->setCurrentItem(i);
        widgetChanged(i);
        break;
      }
  }
}



void AssocTextEditor::insertAssociatedText(const QString& a_text)
{
    associatedTextEdit->insert(a_text);
}

void AssocTextEditor::insertFile()
{
  QString fileName = KFileDialog::getOpenFileName();

  if(fileName.isEmpty())
    return;

  QFile insertFile(fileName);
  if(!insertFile.open(QIODevice::ReadOnly))
  {
    KMessageBox::error( this, i18n("<qt>Cannot open file<br><b>%1</b></qt", fileName ) );
    return;
  }
  Q3TextStream insertStream(&insertFile);
  QString insertText = insertStream.read();
  insertAssociatedText(insertText);
  insertFile.close();
}

void AssocTextEditor::insertWidgetName(int index)
{
  QString prefix;
  if (!KommanderWidget::useInternalParser)
    prefix = QString(QChar(ESCCHAR));
  insertAssociatedText(prefix + widgetToString(widgetFromString(widgetComboBox->text(index)), false));
}

void AssocTextEditor::insertFunction()
{
  FunctionsDialog pDialog(this, m_widgetList, 0);
  if (pDialog.exec())
    insertAssociatedText(pDialog.functionText());
}

QString AssocTextEditor::widgetToString(QWidget* widget, bool formatted)
{
   if (!widget)
     return QString();
   else if (formatted)
     return QString("%1 (%2)").arg(widget->name()).arg(widget->className());
   else
     return widget->name();
}

QWidget* AssocTextEditor::widgetFromString(const QString& name)
{
  QString realname = name;
  int i = realname.find(' ');
  if (i != -1)
    realname.truncate(i);
  return m_widgetList[realname];
}



#include "assoctexteditorimpl.moc"
