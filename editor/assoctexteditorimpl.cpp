/***************************************************************************
                          assoctexteditorimpl.cpp  - Associated text editor implementation
                             -------------------
    copyright            : (C) 2002-2004 by Marc Britton
    email                : consume@optusnet.com.au
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

/* QT INCLUDES */
#include <qstringlist.h>
#include <qmetaobject.h>
#include <qcombobox.h>
#include <qtextedit.h>
#include <qstring.h>
#include <qfile.h>
#include <qobject.h>
#include <qobjectlist.h>
#include <qpushbutton.h>

/* OTHER INCLUDES */
#include <cstdio>
#include "assoctexteditorimpl.h"
#include <kommanderwidget.h>
#include <formwindow.h>
#include <propertyeditor.h>
#include <command.h>
#include <metadatabase.h>

AssocTextEditor::AssocTextEditor(QWidget *a_widget, FormWindow* a_form, 
    PropertyEditor* a_property, QWidget *a_parent, const char *a_name, bool a_modal)
    : AssocTextEditorBase(a_parent, a_name, a_modal)
{
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
  buildFunctionList();
  setWidget(a_widget);
  
  connect(associatedTextEdit, SIGNAL(textChanged()), SLOT(textEditChanged()));
  connect(widgetsComboBox, SIGNAL(activated(int)), SLOT(widgetChanged(int)));
  connect(stateComboBox, SIGNAL(activated(int)), SLOT(stateChanged(int)));
  connect(filePushButton, SIGNAL(clicked()), SLOT(insertFile()));
  connect(widgetComboBox, SIGNAL(activated(int)), SLOT(insertWidgetName(int)));
  connect(functionComboBox, SIGNAL(activated(int)), SLOT(insertFunction(int)));
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
      m_atdict[(*s_it)] = QString::null;
  }
  m_populationText = a_atw->populationText();
    
  // initial text for initial state
  m_currentState = stateComboBox->currentText();

  // show current state
  stateComboBox->setCurrentItem(0);
  stateChanged(0);
  
}

void AssocTextEditor::save() const
{
//FIXME: Use only KommanderWidget
  KommanderWidget *atw = dynamic_cast<KommanderWidget *>(m_widget);
  if (!atw)
    return;

  if (atw->associatedText() != associatedText())
  {
    QString text = QString("Set the \'text association\' of \'%1\'").arg(m_widget->name());
    SetPropertyCommand *cmd  = new SetPropertyCommand(text, m_formWindow, 
        m_widget, m_propertyEditor, "associations", atw->associatedText(),
        associatedText(), QString::null, QString::null, false);
    cmd->execute();
    m_formWindow->commandHistory()->addCommand(cmd);
    MetaDataBase::setPropertyChanged(m_widget, "associations", true);
  }
  if (atw->populationText() != populationText())
  {
    QString text = QString("Set the \'population text\' of \'%1\'").arg(m_widget->name());
    SetPropertyCommand *cmd  = new SetPropertyCommand(text, m_formWindow, m_widget, 
        m_propertyEditor, "populationText", atw->populationText(),
        populationText(), QString::null, QString::null, false);
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

void AssocTextEditor::buildWidgetList()
{
  QStringList widgetList;
  QObject* thisObject = m_formWindow->mainContainer();
  QObjectList *objectList = thisObject->queryList();
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
}

void AssocTextEditor::buildFunctionList()
{
  functionComboBox->insertItem("@dcop()");
  functionComboBox->insertItem("@dexec()");
  functionComboBox->insertItem("@dcopid");
  functionComboBox->insertItem("@pid");
  functionComboBox->insertItem("@widgetText");
  functionComboBox->insertItem("@readSetting()");
  functionComboBox->insertItem("@writeSetting()");
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
}

void AssocTextEditor::widgetChanged(int index)
{
  save();
  setWidget( widgetFromString( widgetsComboBox->text(index)) ) ;
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
  if(!insertFile.open(IO_ReadOnly))
  {
    KMessageBox::error( this, i18n("<qt>Cannot open file<br><b>%1</b></qt").arg( fileName.local8Bit().data() ) );
    return;
  }
  QTextStream insertStream(&insertFile);
  QString insertText = insertStream.read();
  insertAssociatedText(insertText);
  insertFile.close();
}

void AssocTextEditor::insertWidgetName(int index)
{
  insertAssociatedText( QString(QChar(ESCCHAR))+
       widgetToString ( widgetFromString(widgetComboBox->text(index)), false) );
}

void AssocTextEditor::insertFunction(int index)
{
  insertAssociatedText(functionComboBox->text(index));
}

QString AssocTextEditor::widgetToString(QWidget* widget, bool formatted)
{
   if (!widget)
     return QString::null;
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




/* Paste text encoded
QString AssocTextEditor::escapeText(QString &a_text)
{
	char *text = a_text.latin1();
	QString escapedText;

	int textLength = a_text.length();
	int i = 0;
	while(i < textLength)
	{
		while(i < textLength && text[i] != '$')
				escapedText += text[i++];

		if(i < textLength)
		{
			escapedText += "\\$";
		}
		++i;
	}
	return escapedText;
}

void AssocTextEditor::escapeDollarSigns()
{
	QString text = associatedTextEdit->text();
	associatedTextEdit->setText(escapeText(text));
}

AssocTextEdit::AssocTextEdit(QWidget *a_parent, const char *a_name)
    : QTextEdit(a_parent, a_name)
{
}

void AssocTextEdit::insert(const QString &a_text, bool a_indent, bool a_checkNewLine, bool a_removeSelected)
{
    QString text = a_text;
    QString escapedText;
    
    int textLength = a_text.length();
    int i = 0;
    while(i < textLength)
    {
        while(i < textLength && text[i] != '$')
   	    escapedText += text[i++];

        if(i < textLength)
        {
				escapedText += "\\$";
        }
        ++i;
    }
    QTextEdit::insert(escapedText, a_indent, a_checkNewLine, a_removeSelected);
}
*/



#include "assoctexteditorimpl.moc"
