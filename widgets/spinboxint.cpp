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
  return f == DCOP::text || f == DCOP::setText || f == DCOP::setMaximum || f == DCOP::getBackgroundColor || f == DCOP::setBackgroundColor;
}

QString SpinBoxInt::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::text:
      return cleanText();
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    case DCOP::setMaximum:
      setMaxValue(args[0].toUInt());
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

#include "spinboxint.moc"
