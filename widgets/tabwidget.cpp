/***************************************************************************
                          tabwidget.cpp - Widget with tabs 
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
#include <ktabwidget.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *a_parent, const char *a_name)
	: KTabWidget(a_parent), KommanderWidget(this)
{
  setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
}

TabWidget::~TabWidget()
{
}

QString TabWidget::currentState() const
{
  return QString("default");
}

bool TabWidget::isKommanderWidget() const
{
  return true;
}

QStringList TabWidget::associatedText() const
{
  return KommanderWidget::associatedText();
}

void TabWidget::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void TabWidget::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText( a_text );
}

QString TabWidget::populationText() const
{
  return KommanderWidget::populationText();
}

void TabWidget::populate()
{
}

void TabWidget::showEvent(QShowEvent* e)
{
  QTabWidget::showEvent(e);
  emit widgetOpened();
}

void TabWidget::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

bool TabWidget::isFunctionSupported(int f)
{
  return f == DBUS::currentItem || f == DBUS::setCurrentItem;
}

QString TabWidget::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::currentItem:
      return QString::number(currentIndex());
    case DBUS::setCurrentItem:
      setCurrentIndex(args[0].toInt());
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}

#include "tabwidget.moc"
