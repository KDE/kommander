/***************************************************************************
                          radiobutton.cpp - Radiobutton class 
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
#include <qradiobutton.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "radiobutton.h"

RadioButton::RadioButton(QWidget *a_parent, const char *a_name)
	: QRadioButton(a_parent, a_name), KommanderWidget(this)
{
	QStringList states;
	states << "unchecked";
	states << "checked";
	setStates(states);
	QStringList displayStates;
	displayStates << "checked";
	displayStates << "unchecked";
	setDisplayStates(displayStates);

}

RadioButton::~RadioButton()
{
}

QString RadioButton::currentState() const
{
	return (isChecked() ? "checked" : "unchecked");
}

bool RadioButton::isKommanderWidget() const
{
	return TRUE;
}

QStringList RadioButton::associatedText() const
{
	return KommanderWidget::associatedText();
}

void RadioButton::setAssociatedText(QStringList a_at)
{
	KommanderWidget::setAssociatedText(a_at);
}

void RadioButton::setWidgetText(const QString &a_text)
{
    setText(a_text);
	emit widgetTextChanged(a_text);
}

void RadioButton::setPopulationText( QString a_text )
{
    KommanderWidget::setPopulationText( a_text );
}

QString RadioButton::populationText() const
{
    return KommanderWidget::populationText();
}

void RadioButton::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
    setWidgetText( txt );
}

QString RadioButton::widgetText() const
{
	return QString::null;
}

void RadioButton::showEvent( QShowEvent *e )
{
    QRadioButton::showEvent( e );
    emit widgetOpened();
}

#include "radiobutton.moc"
