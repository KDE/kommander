/***************************************************************************
                          buttongroup.cpp - Button group widget 
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

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qbuttongroup.h>
#include <qevent.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "buttongroup.h"

#include "radiobutton.h" // include a button header for the compiler with dynamic cast below

ButtonGroup::ButtonGroup(QWidget *a_parent, const char *a_name)
	: QButtonGroup(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "checked";
  states << "unchecked";
  setStates(states);
  setDisplayStates(states);

}

ButtonGroup::~ButtonGroup()
{
}

QString ButtonGroup::currentState() const
{
  if (!isCheckable() || isChecked())
    return "checked";
  else
    return "unchecked";
}

bool ButtonGroup::isKommanderWidget() const
{
	return TRUE;
}

QStringList ButtonGroup::associatedText() const
{
	return KommanderWidget::associatedText();
}

void ButtonGroup::setAssociatedText(const QStringList& a_at)
{
	KommanderWidget::setAssociatedText(a_at);
}

void ButtonGroup::setPopulationText(const QString& a_text)
{
    KommanderWidget::setPopulationText( a_text );
}

QString ButtonGroup::populationText() const
{
    return KommanderWidget::populationText();
}

void ButtonGroup::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
    //implement me
}

void ButtonGroup::setWidgetText(const QString&)
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

void ButtonGroup::setSelectedWidgetText(const QString&)
{
}

QString ButtonGroup::selectedWidgetText() const
{
  return QString::null;
}

void ButtonGroup::showEvent(QShowEvent* e)
{
  QButtonGroup::showEvent( e );
  emit widgetOpened();
}

#include "buttongroup.moc"
