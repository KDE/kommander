/***************************************************************************
                          timer.cpp - Widget for running scripts periodically
                             -------------------
    copyright            : (C) 2004 Michal Rudolf <mrudolf@kdewebdev.org>
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
#include <qtimer.h>
#include <qwidget.h>

/* KDE INCLUDES */
#include <kglobal.h>
#include <kiconloader.h>


/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "timer.h"
#include "myprocess.h"
#include "specials.h"

Timer::Timer(QWidget *a_parent, const char *a_name)
  : QLabel(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  if (KommanderWidget::inEditor)
  {
    setPixmap(KGlobal::iconLoader()->loadIcon("kalarm", KIcon::NoGroup, KIcon::SizeMedium));
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(1);
    setFixedSize(pixmap()->size());
  }
  else
    setHidden(true);
  
  mTimer = new QTimer(this);
  setInterval(5000);
  connect(mTimer, SIGNAL(timeout()), SLOT(timeout()));
}

Timer::~Timer()
{
}

int Timer::interval() const
{
  return mInterval;
}


void Timer::setInterval(int a_interval)
{
  mInterval = a_interval;
}
      
QString Timer::currentState() const
{
  return QString("default");
}

bool Timer::isKommanderWidget() const
{
  return true;
}

QStringList Timer::associatedText() const
{
  return KommanderWidget::associatedText();
}

void Timer::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void Timer::setWidgetText(const QString& a_text)
{
  KommanderWidget::setAssociatedText(a_text);
}

void Timer::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString Timer::populationText() const
{
  return KommanderWidget::populationText();
}

void Timer::populate()
{
  setAssociatedText(KommanderWidget::evalAssociatedText(populationText()));
}

void Timer::executeProcess(bool blocking)
{
  MyProcess process(this);
  process.setBlocking(blocking);
  process.run(evalAssociatedText());
}

void Timer::timeout()
{
  executeProcess(true);
}

void Timer::execute()
{
  mTimer->start(mInterval);
}

void Timer::cancel()
{
  mTimer->stop();
}
      
      

bool Timer::isFunctionSupported(int f)
{
  return f == DCOP::setText || f == DCOP::execute || f == DCOP::cancel;
}

QString Timer::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::setText:
      setAssociatedText(args[0]);
      break;
    case DCOP::execute:
      execute();
      break;
    case DCOP::cancel:
      cancel();
      break;
    default:
      break;
  }
  return QString::null;
}

#include "timer.moc"