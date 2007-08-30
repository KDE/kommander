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

/* KDE INCLUDES */
#include <kglobal.h>
#include <kiconloader.h>
#include <kicon.h>

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QShowEvent>
#include <QLabel>


/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "pixmaplabel.h"

PixmapLabel::PixmapLabel(QWidget *a_parent, const char *a_name)
  : QLabel(a_parent), KommanderWidget(this)
{
  setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  if (KommanderWidget::inEditor)
    setPixmap(KIconLoader::global()->loadIcon(QString("tux"),K3Icon::NoGroup));
  //setAutoResize(false);
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
  return f == DBUS::text || f == DBUS::setText || f == DBUS::clear;
}

QString PixmapLabel::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    case DBUS::clear:
      setPixmap(QPixmap());
      break;
    case DBUS::text:
      return text();
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}

#include "pixmaplabel.moc"
