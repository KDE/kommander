/***************************************************************************
                          tabwidget.cpp - Widget with tabs 
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
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qtabwidget.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *a_parent, const char *a_name, int a_flags)
	: QTabWidget(a_parent, a_name, a_flags), KommanderWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

}

TabWidget::~TabWidget()
{
}

QString TabWidget::currentState() const
{
	return QString("default");
}

bool TabWidget::isKommanderWidget() const
{
	return TRUE;
}

QStringList TabWidget::associatedText() const
{
	return KommanderWidget::associatedText();
}

void TabWidget::setAssociatedText(QStringList a_at)
{
	KommanderWidget::setAssociatedText(a_at);
}

void TabWidget::setPopulationText( QString a_text )
{
    KommanderWidget::setPopulationText( a_text );
}

QString TabWidget::populationText() const
{
    return KommanderWidget::populationText();
}

void TabWidget::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
    //implement me
}

void TabWidget::setWidgetText(const QString &a_text)
{
	setCaption(a_text);
	emit widgetTextChanged(a_text);
}

QString TabWidget::widgetText() const
{
	return caption();
}

void TabWidget::setSelectedWidgetText( const QString & )
{
}

QString TabWidget::selectedWidgetText() const
{
    return QString::null;
}

void TabWidget::showEvent( QShowEvent *e )
{
    QTabWidget::showEvent( e );
    emit widgetOpened();
}

#include "tabwidget.moc"
