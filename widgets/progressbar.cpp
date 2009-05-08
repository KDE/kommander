/***************************************************************************
                         progressbar.cpp - ProgressBar widget 
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
#include <klocale.h>

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>

/* OTHER INCLUDES */
#include <specials.h>
#include "progressbar.h"
#include "kommanderplugin.h"

enum Functions {
  FirstFunction = 585,
  PB_setHighlightColor,
  PB_setHightlightTextColor,
  LastFunction
};

ProgressBar::ProgressBar(QWidget *a_parent, const char *a_name)
  : KProgress(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  KommanderPlugin::setDefaultGroup(Group::DCOP);
  KommanderPlugin::registerFunction(PB_setHighlightColor, "setBarColor(QString widget, QString Color)",  i18n("Sets the ProgresBar color"), 2);
  KommanderPlugin::registerFunction(PB_setHightlightTextColor, "setBarTextColor(QString widget, QString Color)",  i18n("Sets the ProgresBar text color"), 2);
}

ProgressBar::~ProgressBar()
{
}

QString ProgressBar::currentState() const
{
  return QString("default");
}

bool ProgressBar::isKommanderWidget() const
{
  return true;
}

QStringList ProgressBar::associatedText() const
{
  return KommanderWidget::associatedText();
}

void ProgressBar::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void ProgressBar::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString ProgressBar::populationText() const
{
  return KommanderWidget::populationText();
}

void ProgressBar::populate()
{
  setProgress(KommanderWidget::evalAssociatedText(populationText()).toUInt());
}

void ProgressBar::showEvent(QShowEvent *e)
{
  QProgressBar::showEvent(e);
  emit widgetOpened();
}

bool ProgressBar::isFunctionSupported(int f)
{
  return f == DCOP::text || f == DCOP::setText || f == DCOP::clear || f == DCOP::setMaximum || f == DCOP::geometry || (f > FirstFunction && f < LastFunction);
}

QString ProgressBar::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::text:
      return QString::number(progress());
    case DCOP::setText:
      setProgress(args[0].toUInt());
      break;
    case DCOP::clear:
      setProgress(0);
      break;
    case DCOP::setMaximum:
      setTotalSteps(args[0].toUInt());
      break;
    case DCOP::geometry:
    {
      QString geo = QString::number(this->x())+" "+QString::number(this->y())+" "+QString::number(this->width())+" "+QString::number(this->height());
      return geo;
      break;
    }
    case PB_setHighlightColor:
    {
      QColor color;
      color.setNamedColor(args[0]);
      QPalette p = this->palette();
      p.setColor(QPalette::Active, QColorGroup::Highlight, color);
      this->setPalette( p, TRUE );
      break;
    }
    case PB_setHightlightTextColor:
    {
      QColor color;
      color.setNamedColor(args[0]);
      QPalette p = this->palette();
      p.setColor(QPalette::Active, QColorGroup::HighlightedText, color);
      this->setPalette( p, TRUE );
      break;
    }
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}

#include "progressbar.moc"
