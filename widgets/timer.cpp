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
#include <klocale.h>


/* OTHER INCLUDES */
#include <myprocess.h>
#include <specials.h>
#include "timer.h"
#include "kommanderplugin.h"
#include "specials.h"


enum Functions {
  FirstFunction = 179,
  SetInterval,
  LastFunction
};

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
  setSingleShot(false);
  connect(mTimer, SIGNAL(timeout()), SLOT(timeout()));

  KommanderPlugin::setDefaultGroup(Group::DCOP);
  KommanderPlugin::registerFunction(SetInterval, "setInterval(QString widget, int interval)",  i18n("Set the timer timeout interval in ms."), 2);
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
      
bool Timer::singleShot() const
{
  return mSingleShot;
}

void Timer::setSingleShot(bool a_shot)
{
  mSingleShot = a_shot;
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
  if (blocking)
    emit finished();
}

void Timer::timeout()
{
  executeProcess(true);
}

void Timer::execute()
{
  if (mSingleShot)
    QTimer::singleShot(mInterval, this, SLOT(timeout()));
  else
    mTimer->start(mInterval);
}

void Timer::cancel()
{
  mTimer->stop();
}
      
      

bool Timer::isFunctionSupported(int f)
{
  return f == DCOP::setText || f == DCOP::execute || f == DCOP::cancel || (f > FirstFunction && f < LastFunction);
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
    case SetInterval:
      mTimer->changeInterval(args[0].toInt());
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}

#include "timer.moc"
