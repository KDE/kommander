/***************************************************************************
                          buttongroup.cpp - Button group widget 
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
#include <qbuttongroup.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "buttongroup.h"

#include "radiobutton.h" // include a button header for the compiler with dynamic cast below

ButtonGroup::ButtonGroup(QWidget *a_parent, const char *a_name)
	: QButtonGroup(a_parent, a_name), KommanderWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

	emit widgetOpened();
}

ButtonGroup::~ButtonGroup()
{
}

QString ButtonGroup::currentState() const
{
	return QString("default");
}

bool ButtonGroup::isKommanderWidget() const
{
	return TRUE;
}

QStringList ButtonGroup::associatedText() const
{
	return KommanderWidget::associatedText();
}

void ButtonGroup::setAssociatedText(QStringList a_at)
{
	KommanderWidget::setAssociatedText(a_at);
}

void ButtonGroup::setWidgetText(const QString &)
{
}

QString ButtonGroup::widgetText() const
{
	int i = 0;
	QString text;
	for(;i < count();++i)
	{
		QButton *button = find(i);
		KommanderWidget *textWidget = dynamic_cast<KommanderWidget *>(button);
		if(textWidget)
			text += textWidget->evalAssociatedText();
	}
	return text;
}
#include "buttongroup.moc"
