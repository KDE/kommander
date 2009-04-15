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

/* OTHER INCLUDES */
#include <specials.h>
#include "statusbar.h"

StatusBar::StatusBar(QWidget *a_parent, const char *a_name)
  : KStatusBar(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  insertItem(QString::null, 0, 1);
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

void StatusBar::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

bool StatusBar::isFunctionSupported(int f)
{
  return f == DCOP::setText || f == DCOP::insertItem || f == DCOP::removeItem || f == DCOP::clear || f == DCOP::getBackgroundColor || f == DCOP::setBackgroundColor;
}

QString StatusBar::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::setText:
      changeItem(args[0], 0);
      break;
    case DCOP::insertItem:
      if (hasItem(args[1].toInt()))
        changeItem(args[0], args[1].toInt());
      else 
        insertItem(args[0], args[1].toInt());
      break;
    case DCOP::removeItem:
      removeItem(args[0].toInt());
      break;
    case DCOP::clear:
      clear();
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

#include "statusbar.moc"
