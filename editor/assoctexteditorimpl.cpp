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
#include <kfiledialog.h>

/* QT INCLUDES */
#include <qstringlist.h>
#include <qmetaobject.h>
#include <qdict.h>
#include <qcombobox.h>
#include <qtextedit.h>
#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qobject.h>
#include <qobjectlist.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

/* OTHER INCLUDES */
#include <cstdio>
#include "assoctexteditorimpl.h"
#include <kommanderwidget.h>

AssocTextEditor::AssocTextEditor(QWidget *a_widget, KommanderWidget *a_atw, QWidget *a_parent, const char *a_name, bool a_modal)
    : AssocTextEditorBase(a_parent, a_name, a_modal)
{
    // signals and slots connections
	m_widget = a_widget;
	build(a_atw);
	connect(stateComboBox, SIGNAL(activated(int)), SLOT(stateChanged(int)));
	connect(associatedTextEdit, SIGNAL(textChanged()), SLOT(textEditChanged()));
	connect(widgetComboBox, SIGNAL(activated(int)), SLOT(insertWidgetName(int)));
	connect(filePushButton, SIGNAL(clicked()), SLOT(insertFile()));
	connect(allWidgetsCheckBox, SIGNAL(clicked()), SLOT(updateTextWidgets()));
	connect(insertIdentifierButton, SIGNAL(clicked()), SLOT(insertIdentifier()));
	connect( typeButtonGroup, SIGNAL(clicked(int)), this, SLOT(typeChanged(int)) );
}

AssocTextEditor::~AssocTextEditor()
{
}

void AssocTextEditor::build(KommanderWidget *a_atw)
{
	QStringList at = a_atw->associatedText();
	m_states = a_atw->states();
	m_populationText = a_atw->populationText();

	stateComboBox->insertStringList(a_atw->displayStates());
	QStringList::iterator s_it = m_states.begin();
	QStringList::iterator at_it = at.begin();
	for(;s_it != m_states.end();++s_it)// fill dict with text for each state
	{
		if(at_it != at.end())
		{
			m_atdict[(*s_it)] = (*at_it);
			++at_it;
		}
		else
		{
			m_atdict[(*s_it)] = QString::null;
		}
	}
	// initial text for initial state
	m_currentState = stateComboBox->currentText();

	//state combo initially hidden
	stateComboBox->hide();
	m_curTypePopulation = TRUE;
	populationRadio->setChecked( TRUE );
	associatedTextEdit->setText( m_populationText );

	// populate widget combo
	updateTextWidgets();
}

void AssocTextEditor::typeChanged( int id )
{
    if( typeButtonGroup->find( id ) == populationRadio )
    {
	m_curTypePopulation = TRUE;
	stateComboBox->hide();
	associatedTextEdit->setText( m_populationText );
    }
    else
    {
	m_curTypePopulation = FALSE;
	stateComboBox->show();
	associatedTextEdit->setText(m_atdict[m_currentState]);
    }
}

void AssocTextEditor::updateTextWidgets()
{
	QObject *thisObject = m_widget;

	if(!(allWidgetsCheckBox->isChecked())) // list all of the widgets
	{
		QObject *tmp;
		while((tmp = thisObject->parent()) != 0)
		{
			thisObject = tmp;
			if(thisObject->inherits("QDialog")) // only track back to parent dialog
				break;
		}
	}

	QObjectList *objectList = thisObject->queryList();

	widgetComboBox->clear();
	if(objectList)
	{
		if(objectList->find(m_widget) != -1)
			objectList->remove(objectList->current());
			
		QObjectListIt it(*objectList);

		while(it.current() != 0)
		{
		/* There is a warning message with the property() function if it does not exist. Verify the property exists with the meta information first */
			bool pExists = FALSE;

			QMetaObject *metaObj = it.current()->metaObject();
			if(metaObj)
			{
				int id = metaObj->findProperty("KommanderWidget", TRUE);
				const QMetaProperty *metaProp = metaObj->property(id, TRUE);
				if(metaProp && metaProp->isValid()) pExists = TRUE;
			}

			if(pExists)
			{
				QVariant flag = (it.current())->property("KommanderWidget");
				if(flag.isValid() && !(QString(it.current()->name()).startsWith("qt_")))
					widgetComboBox->insertItem((it.current())->name());
			}
			++it;
		}
		delete objectList;
	}
}

void AssocTextEditor::stateChanged(int a_index)
{
    if( m_curTypePopulation )
    {
	qDebug("BUG : stateChanged when current editing type is population");
	return;
    }
    m_currentState = stateComboBox->text(a_index);
    associatedTextEdit->setText(m_atdict[m_currentState]);
}

void AssocTextEditor::insertAssociatedText(QString a_text)
{
    associatedTextEdit->insert(a_text);
}

void AssocTextEditor::textEditChanged()
{
    if( m_curTypePopulation )
    {
	m_populationText = associatedTextEdit->text();
    }
    else
	m_atdict[m_currentState] = associatedTextEdit->text();
}

QStringList AssocTextEditor::associatedText() const
{
    QStringList at;
    QStringList states = m_states;
    QStringList::iterator it = states.begin();
    for(;it != states.end();++it)
    {
	    at.append(m_atdict[(*it)]);
    }
    return at;
}

QString AssocTextEditor::populationText() const
{
    return m_populationText;
}

void AssocTextEditor::insertFile()
{
    QString fileName = KFileDialog::getOpenFileName();

    if(!fileName.isEmpty())
    {
	    QFile insertFile(fileName);
	    if(!insertFile.open(IO_ReadOnly))
	    {
		    qWarning("Failed to open '%s' for insertion", fileName.local8Bit().data());
		    return;
	    }

	    QTextStream insertStream(&insertFile);

	    QString insertText = insertStream.read();

	    insertAssociatedText(insertText);

	    insertFile.close();
    }
}

void AssocTextEditor::insertWidgetName(int index)
{
	QString name = widgetComboBox->text(index);
	
	if(!name.isEmpty())
		insertAssociatedText(QString(QString(QChar(ESCCHAR)))+name);
}

void AssocTextEditor::insertIdentifier()
{
	insertAssociatedText(QString("@widgetText"));
}

#if 0
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
#endif
#include "assoctexteditorimpl.moc"
