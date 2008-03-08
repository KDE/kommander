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
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <specials.h>
#include "radiobutton.h"

RadioButton::RadioButton(QWidget *a_parent, const char *a_name)
  : QRadioButton(a_parent), KommanderWidget(this)
{
  QStringList states;
  this->setObjectName(a_name);
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
  return true;
}

QStringList RadioButton::associatedText() const
{
  return KommanderWidget::associatedText();
}

void RadioButton::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void RadioButton::setWidgetText(const QString &a_text)
{
  setText(a_text);
  emit widgetTextChanged(a_text);
}

void RadioButton::setPopulationText(const QString& a_text)
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

void RadioButton::showEvent( QShowEvent *e )
{
    QRadioButton::showEvent( e );
    emit widgetOpened();
}

bool RadioButton::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::setText || f == DBUS::setChecked;
}

QString RadioButton::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
      return text();
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    case DBUS::setChecked:
      setChecked(args[0] != "false");
      break;
    case DBUS::checked:
      return QString::number(isChecked());
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}

#include "radiobutton.moc"
