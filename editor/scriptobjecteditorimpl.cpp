/***************************************************************************
                          scriptobjecteditorimpl.cpp  -  Script Object Editor implementation
                             -------------------
    copyright            : (C) 2003 by Marc Britton
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
/* Qt Includes */
#include <qlineedit.h>
#include <qwidget.h>
#include <qstring.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qtextedit.h>

/* Other Includes */
#include "formwindow.h"
#include <scriptobject.h>
#include "widgetfactory.h"
#include "widgetdatabase.h"
#include "scriptobjecteditorimpl.h"

ScriptObjectEditor::ScriptObjectEditor(QMap<QString, QString> a_objects, QWidget *a_parent, const char *a_name)
 : ScriptObjectEditorBase(a_parent, a_name), m_objects(a_objects)
{
	// populate object list
	QMap<QString, QString>::Iterator it;

	for(it = a_objects.begin();it != a_objects.end();++it) scriptObjectsList->insertItem(QString(it.key()));
	scriptObjectsList->sort();

	m_objects = a_objects;

	connect(createPushButton, SIGNAL(clicked()), this, SLOT(createScriptObject()));
	connect(objectName, SIGNAL(textChanged(const QString &)), this, SLOT(changeObjectName(const QString &)));
	connect(removePushButton, SIGNAL(clicked()), this, SLOT(removeScriptObject()));
	connect(scriptObjectsList, SIGNAL(highlighted(const QString &)), objectName, SLOT(setText(const QString &)));
	connect(scriptContent, SIGNAL(textChanged()), this, SLOT(setScript()));
	connect(scriptObjectsList, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}

ScriptObjectEditor::~ScriptObjectEditor()
{
}

/* Private Slots */
void ScriptObjectEditor::createScriptObject()
{
	//create a unique name
	QString baseName = "scriptObject", uniqueID;
	for(int num = 1;;++num)
	{
		uniqueID.setNum(num);
		if(m_objects.find(baseName + uniqueID) == m_objects.end()) break;
	}
	scriptObjectsList->insertItem(baseName + uniqueID);

	m_objects[baseName + uniqueID] = "";
}

void ScriptObjectEditor::removeScriptObject()
{
	QListBoxItem *item = scriptObjectsList->selectedItem();
	if(!item) return;

	m_objects.remove(item->text());
	delete item;

	objectName->setText(QString(""));
	scriptContent->setText(QString(""));
}

void ScriptObjectEditor::selectionChanged()
{
	QListBoxItem *selectedItem = scriptObjectsList->selectedItem();
	if(!selectedItem)
	{
		scriptContent->setText(QString(""));
	  	objectName->setText(QString(""));	
	}
	else
	{
		scriptContent->setText(m_objects[selectedItem->text()]);
		objectName->setText(selectedItem->text());
	}
}

void ScriptObjectEditor::changeObjectName(const QString &a_name)
{
	QListBoxItem *selectedItem = scriptObjectsList->selectedItem();
	if(!selectedItem) return;

	QString oldItemText = selectedItem->text();
	if(a_name == oldItemText) return;

	m_objects[a_name] = m_objects[oldItemText];
	m_objects.remove(oldItemText);

	// hack to stop the list emitting a selection changed signal when we're just changing its name
	disconnect(scriptObjectsList, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
	scriptObjectsList->changeItem(a_name, scriptObjectsList->currentItem());
	connect(scriptObjectsList, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}

void ScriptObjectEditor::setScript()
{
	QListBoxItem *item = scriptObjectsList->selectedItem();
	if(!item) return;

	m_objects[item->text()] = scriptContent->text();
}

QMap<QString, QString> ScriptObjectEditor::scriptObjects() const
{
	return m_objects;
}

#include "scriptobjecteditorimpl.moc"
