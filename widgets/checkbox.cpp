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
#include <qbutton.h>

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
  if (state() == QButton::Off)
    return "unchecked";
  else if (state() == QButton::NoChange)
    return "semichecked";
  else if (state() == QButton::On)
    return "checked";
  return QString();
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
  return f == DCOP::text || f == DCOP::setText || f == DCOP::checked || f == DCOP::setChecked || f == DCOP::getBackgroundColor || f == DCOP::setBackgroundColor;
}

void CheckBox::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

QString CheckBox::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::text:
      return text();
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    case DCOP::checked:
      return QString::number(isChecked());
    case DCOP::setChecked:
      setChecked(args[0] != "false" && args[0] != "0");
      break;
    case DCOP::getBackgroundColor:
      return this->paletteBackgroundColor().name();
      break;
    case DCOP::setBackgroundColor:
    {
      QColor color;
      color.setNamedColor(args[0]);
      this->setPaletteBackgroundColor(color);
      break;
    }
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}


#include "checkbox.moc"
