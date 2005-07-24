/***************************************************************************
                         pixmaplabel.cpp - Pixmap label widget 
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
#include <qpixmap.h>

/* KDE INCLUDES */
#include <kglobal.h>
#include <kiconloader.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "pixmaplabel.h"

PixmapLabel::PixmapLabel(QWidget *a_parent, const char *a_name)
  : QLabel(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  if (KommanderWidget::inEditor)
    setPixmap(KGlobal::iconLoader()->loadIcon("tux", KIcon::NoGroup, 128));
  setAutoResize(false);
}

PixmapLabel::~PixmapLabel()
{
}

QString PixmapLabel::currentState() const
{
  return QString("default");
}

bool PixmapLabel::isKommanderWidget() const
{
  return true;
}

QStringList PixmapLabel::associatedText() const
{
  return KommanderWidget::associatedText();
}

void PixmapLabel::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void PixmapLabel::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString PixmapLabel::populationText() const
{
  return KommanderWidget::populationText();
}

void PixmapLabel::populate()
{
  setWidgetText(KommanderWidget::evalAssociatedText(populationText()));
}

void PixmapLabel::setWidgetText(const QString& a_text)
{
  QPixmap pixmap;
  if (pixmap.load(a_text))
    setPixmap(pixmap);
}

void PixmapLabel::showEvent(QShowEvent *e)
{
  QLabel::showEvent(e);
  emit widgetOpened();
}

bool PixmapLabel::isFunctionSupported(int f)
{
  return f == DCOP::text || f == DCOP::setText || f == DCOP::clear;
}

QString PixmapLabel::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    case DCOP::clear:
      setPixmap(QPixmap());
      break;
    case DCOP::text:
      return text();
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString::null;
}

#include "pixmaplabel.moc"
