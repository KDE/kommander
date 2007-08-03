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
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <specials.h>
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
  return true;
}

QStringList SpinBoxInt::associatedText() const
{
  return KommanderWidget::associatedText();
}

void SpinBoxInt::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void SpinBoxInt::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString SpinBoxInt::populationText() const
{
  return KommanderWidget::populationText();
}

void SpinBoxInt::populate()
{
  setWidgetText(KommanderWidget::evalAssociatedText( populationText()));
}

void SpinBoxInt::setWidgetText(const QString &a_text)
{
  setValue(a_text.toInt());
  emit widgetTextChanged(a_text);
}

void SpinBoxInt::showEvent( QShowEvent *e )
{
  QSpinBox::showEvent(e);
  emit widgetOpened();
}

bool SpinBoxInt::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::setText || f == DBUS::setMaximum;
}

QString SpinBoxInt::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
      return cleanText();
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    case DBUS::setMaximum:
      setMaxValue(args[0].toInt());
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString::null;
}

#include "spinboxint.moc"
