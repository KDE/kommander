/***************************************************************************
                          scriptobject.cpp - Widget for holding scripts 
                             -------------------
    copyright            : (C) 2002 by Marc Britton
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
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>

/* Other Includes */
#include <kommanderwidget.h>
#include "scriptobject.h"

ScriptObject::ScriptObject(QWidget *a_parent, const char *a_name)
	: QWidget(a_parent, a_name), KommanderWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

	setHidden(TRUE);

	QStringList at("@widgetText");
	setAssociatedText(at);

}

ScriptObject::~ScriptObject()
{
}

QString ScriptObject::currentState() const
{
	return QString("default");
}

bool ScriptObject::isKommanderWidget() const
{
	return TRUE;
}

QStringList ScriptObject::associatedText() const
{
	return KommanderWidget::associatedText();
}

void ScriptObject::setAssociatedText(const QStringList& a_at)
{
	KommanderWidget::setAssociatedText(a_at);
}

void ScriptObject::setPopulationText(const QString& a_text)
{
    KommanderWidget::setPopulationText( a_text );
}

QString ScriptObject::populationText() const
{
    return KommanderWidget::populationText();
}

void ScriptObject::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
    setWidgetText( txt );
}

void ScriptObject::setWidgetText(const QString &a_text)
{
	m_script = a_text;
	emit widgetTextChanged(a_text);
}

QString ScriptObject::widgetText() const
{
	return m_script;
}

void ScriptObject::setSelectedWidgetText(const QString &)
{
}

QString ScriptObject::selectedWidgetText() const
{
    return QString::null;
}

void ScriptObject::show()
{
	return; // widget is never shown
}



void ScriptObject::showEvent( QShowEvent *e )
{
    QWidget::showEvent( e );
    emit widgetOpened();
}

#include "scriptobject.moc"
