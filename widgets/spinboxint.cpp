/***************************************************************************
                         spinboxint.cpp - Integer spinbox widget 
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
#include <qspinbox.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "spinboxint.h"

SpinBoxInt::SpinBoxInt(QWidget *a_parent, const char *a_name)
	: QSpinBox(a_parent, a_name), KommanderWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

}

SpinBoxInt::~SpinBoxInt()
{
}

QString SpinBoxInt::currentState() const
{
	return "default";
}

bool SpinBoxInt::isKommanderWidget() const
{
	return TRUE;
}

QStringList SpinBoxInt::associatedText() const
{
	return KommanderWidget::associatedText();
}

void SpinBoxInt::setAssociatedText(QStringList a_at)
{
	KommanderWidget::setAssociatedText(a_at);
}

void SpinBoxInt::setPopulationText( QString a_text )
{
    KommanderWidget::setPopulationText( a_text );
}

QString SpinBoxInt::populationText() const
{
    return KommanderWidget::populationText();
}

void SpinBoxInt::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
    //implement me
}

void SpinBoxInt::setWidgetText(const QString &a_text)
{
	emit widgetTextChanged(a_text);
}

QString SpinBoxInt::widgetText() const
{
	return text();
}

void SpinBoxInt::setSelectedWidgetText( const QString & )
{
}

QString SpinBoxInt::selectedWidgetText() const
{
    return QString::null;
}

void SpinBoxInt::showEvent( QShowEvent *e )
{
    QSpinBox::showEvent( e );
    emit widgetOpened();
}

#include "spinboxint.moc"
