/***************************************************************************
                         groupbox.cpp - Groupbox widget 
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
/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qgroupbox.h>
#include <qobjectlist.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "fileselector.h"
#include "groupbox.h"

GroupBox::GroupBox(QWidget *a_parent, const char *a_name)
	: QGroupBox(a_parent, a_name), KommanderWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

}

GroupBox::~GroupBox()
{
}

QString GroupBox::currentState() const
{
	return QString("default");
}

bool GroupBox::isKommanderWidget() const
{
	return TRUE;
}

QStringList GroupBox::associatedText() const
{
	return KommanderWidget::associatedText();
}

void GroupBox::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void GroupBox::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString GroupBox::populationText() const
{
    return KommanderWidget::populationText();
}

void GroupBox::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
    setWidgetText( txt );
}

void GroupBox::setWidgetText(const QString &)
{
}

QString GroupBox::widgetText() const
{
#if 1
	QString text;
	QObjectList childList = m_childList;
	QObjectListIt it(childList);
	while(it.current() != 0)
	{
		KommanderWidget *atw = dynamic_cast<KommanderWidget *>(it.current());
		if(atw)
		{
			text += atw->evalAssociatedText();
		}
		++it;
	}
	return text;
#else
	QString text;
	const QObjectList *list = children();
	if(!list)
		return QString::null;

	QObjectListIt it(*list);
	while(it.current() != 0)
	{
		// FIXME : will these be in the right order?
		KommanderWidget *atw = dynamic_cast<KommanderWidget *>(*it);
		if(atw)
		{
			text += atw->evalAssociatedText();
		}
		++it;
	}
	delete list; // qt allocates memory for the list

	return text;
#endif
}

void GroupBox::setSelectedWidgetText( const QString &)
{
}

QString GroupBox::selectedWidgetText() const
{
    // return al lthe selectedWidgetText()s of the groupbox children?
    return QString::null;
}

/* We need to keep track of all widgets created as a child of a groupbox, in order of creation. */
void GroupBox::insertChild(QObject *a_child)
{
	m_childList.append(a_child);
	QObject::insertChild(a_child);
}
 
void GroupBox::removeChild(QObject *a_child)
{
	m_childList.remove(a_child);
	QObject::removeChild(a_child);
}

void GroupBox::showEvent( QShowEvent *e )
{
    QGroupBox::showEvent( e );
    emit widgetOpened();
}

#include "groupbox.moc"
