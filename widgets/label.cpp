/***************************************************************************
                         label.cpp - label widget 
                          -------------------
    copyright            : (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
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
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "label.h"

Label::Label(QWidget *a_parent, const char *a_name)
  : QLabel(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  if (KommanderWidget::inEditor && a_name)
    setText(a_name);
}

Label::~Label()
{
}

QString Label::currentState() const
{
  return QString("default");
}

bool Label::isKommanderWidget() const
{
  return TRUE;
}

QStringList Label::associatedText() const
{
  return KommanderWidget::associatedText();
}

void Label::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void Label::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString Label::populationText() const
{
  return KommanderWidget::populationText();
}

void Label::populate()
{
  setWidgetText(KommanderWidget::evalAssociatedText(populationText()));
}

void Label::setWidgetText(const QString& a_text)
{
  setText(a_text);
}

void Label::showEvent(QShowEvent *e)
{
  QLabel::showEvent(e);
  emit widgetOpened();
}

QString Label::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    case DCOP::clear:
      setWidgetText("");
      break;
    case DCOP::text:
      return text();
  }
  return QString::null;
}

#include "label.moc"
