/***************************************************************************
                          slider.cpp - Slider widget
                             -------------------
    copyright            : (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
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

/* QT INCLUDES */
#include <qstringlist.h>
#include <qevent.h>

/* OTHER INCLUDES */
#include <specials.h>
#include "slider.h"

Slider::Slider(QWidget * a_parent, const char *a_name)
  : QSlider(a_parent, a_name), KommanderWidget((QObject *) this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
}

QString Slider::currentState() const
{
  return QString("default");
}

Slider::~Slider()
{
}

bool Slider::isKommanderWidget() const
{
  return TRUE;
}

QStringList Slider::associatedText() const
{
  return KommanderWidget::associatedText();
}

void Slider::setAssociatedText(const QStringList & a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void Slider::setPopulationText(const QString & a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString Slider::populationText() const
{
  return KommanderWidget::populationText();
}

void Slider::populate()
{
  QString txt = KommanderWidget::evalAssociatedText(populationText());
  setWidgetText(txt);
}

void Slider::setWidgetText(const QString & a_text)
{
  setValue(a_text.toInt());
}

void Slider::showEvent(QShowEvent * e)
{
  QSlider::showEvent(e);
  emit widgetOpened();
}

bool Slider::isFunctionSupported(int f)
{
  return f == DCOP::text || f == DCOP::setText || f == DCOP::clear;
}

QString Slider::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::text:
      return QString::number(value());
    case DCOP::setText:
      setValue(args[0].toInt());
      break;
    case DCOP::clear:
      setValue(minValue());
      break;
    default:
      break;
  }
  return QString::null;
}


#include "slider.moc"
