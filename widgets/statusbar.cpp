/***************************************************************************
                         statusbar.cpp - StatusBar widget 
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

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <specials.h>
#include "statusbar.h"

StatusBar::StatusBar(QWidget *a_parent, const char *a_name)
  : KStatusBar(a_parent), KommanderWidget(this)
{
  setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  insertItem(QString::null, 0, 1);	//krazy:exclude=nullstrassign for old broken gcc
  setItemAlignment(0, Qt::AlignLeft);
}

StatusBar::~StatusBar()
{
}

QString StatusBar::currentState() const
{
  return QString("default");
}

bool StatusBar::isKommanderWidget() const
{
  return true;
}

QStringList StatusBar::associatedText() const
{
  return KommanderWidget::associatedText();
}

void StatusBar::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void StatusBar::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString StatusBar::populationText() const
{
  return KommanderWidget::populationText();
}

void StatusBar::populate()
{
  changeItem(KommanderWidget::evalAssociatedText(populationText()), 0);
}

void StatusBar::showEvent(QShowEvent *e)
{
  QStatusBar::showEvent(e);
  emit widgetOpened();
}

bool StatusBar::isFunctionSupported(int f)
{
  return f == DBUS::setText || f == DBUS::insertItem || f == DBUS::removeItem || f == DBUS::clear;
}

QString StatusBar::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::setText:
      changeItem(args[0], 0);
      break;
    case DBUS::insertItem:
      if (hasItem(args[1].toInt()))
        changeItem(args[0], args[1].toInt());
      else 
        insertItem(args[0], args[1].toInt());
      break;
    case DBUS::removeItem:
      removeItem(args[0].toInt());
      break;
    case DBUS::clear:
      clearMessage();
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}

#include "statusbar.moc"
