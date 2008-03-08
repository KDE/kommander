/***************************************************************************
                         combobox.cpp - Combobox widget 
                             -------------------
    copyright            : (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
                           (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
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
#include <qdatetime.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
//Added by qt3to4:
#include <QShowEvent>
#include <QContextMenuEvent>

/* OTHER INCLUDES */
#include <specials.h>
#include "datepicker.h"

DatePicker::DatePicker(QWidget *a_parent, const char *a_name)
  : KDatePicker(a_parent), KommanderWidget(this)
{
  setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
}

DatePicker::~DatePicker()
{
}

QString DatePicker::currentState() const
{
  return QString("default");
}

bool DatePicker::isKommanderWidget() const
{
  return true;
}

QStringList DatePicker::associatedText() const
{
  return KommanderWidget::associatedText();
}

void DatePicker::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void DatePicker::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString DatePicker::populationText() const
{
  return KommanderWidget::populationText();
}

void DatePicker::populate()
{
  setWidgetText(KommanderWidget::evalAssociatedText( populationText()));
}

void DatePicker::setWidgetText(const QString& a_text)
{
  setDate(QDate::fromString(a_text, Qt::ISODate));
  emit widgetTextChanged(a_text);
}

QString DatePicker::widgetText() const
{
    return date().toString(Qt::ISODate); 
}


void DatePicker::showEvent(QShowEvent *e)
{
    KDatePicker::showEvent( e );
    emit widgetOpened();
}
void DatePicker::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}


bool DatePicker::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::setText;
}

QString DatePicker::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
      return date().toString(Qt::ISODate);
    case DBUS::setText:
      setDate(QDate::fromString(args[0], Qt::ISODate));
      break;    
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}

#include "datepicker.moc"
