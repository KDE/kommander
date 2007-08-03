/***************************************************************************
                          checkbox.cpp - Checkbox widget 
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
#include <qcheckbox.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <specials.h>
#include "checkbox.h"

CheckBox::CheckBox(QWidget *a_parent, const char *a_name)
  : QCheckBox(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "unchecked";
  states << "semichecked";
  states << "checked";
  setStates(states);
  QStringList displayStates;
  displayStates << "checked";
  displayStates << "semichecked";
  displayStates << "unchecked";
  setDisplayStates(displayStates);
}

CheckBox::~CheckBox()
{
}

QString CheckBox::currentState() const
{
  if (state() == QCheckBox::Off)
    return "unchecked";
  else if (state() == QCheckBox::NoChange)
    return "semichecked";
  else if (state() == QCheckBox::On)
    return "checked";
  return QString::null;
}

bool CheckBox::isKommanderWidget() const
{
  return true;
}

QStringList CheckBox::associatedText() const 
{
  return KommanderWidget::associatedText();
}

void CheckBox::setAssociatedText(const QStringList & a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void CheckBox::setPopulationText(const QString & a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString CheckBox::populationText() const
{
  return KommanderWidget::populationText();
}

void CheckBox::populate()
{
  setWidgetText(KommanderWidget::evalAssociatedText(populationText()));
}

void CheckBox::setWidgetText(const QString& a_text)
{
  setText(a_text);
  emit widgetTextChanged(a_text);
}

void CheckBox::showEvent(QShowEvent* e)
{
  QCheckBox::showEvent(e);
  emit widgetOpened();
}

bool CheckBox::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::setText || f == DBUS::checked || f == DBUS::setChecked;
}

QString CheckBox::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
      return text();
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    case DBUS::checked:
      return QString::number(isChecked());
    case DBUS::setChecked:
      setChecked(args[0] != "false");
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString::null;
}


#include "checkbox.moc"
