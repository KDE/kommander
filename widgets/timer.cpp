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
//Added by qt3to4:
#include <QLabel>
//#include <QFrame>

/* KDE INCLUDES */
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>

/* OTHER INCLUDES */
#include "kommanderwidget.h"
#include "timer.h"
#include "kommanderplugin.h"
#include "specials.h"
#include "myprocess.h"


enum Functions {
  FirstFunction = 179,
  SetInterval,
  LastFunction
};

Timer::Timer(QWidget *a_parent, const char *a_name)
  : QLabel(a_parent), KommanderWidget(this)
{
  setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  if (KommanderWidget::inEditor)
  {
    setPixmap(KIconLoader::global()->loadIcon("kalarm", KIconLoader::NoGroup, KIconLoader::SizeMedium));
    //setFrameStyle(Q3Frame::Box | Q3Frame::Plain);
    setLineWidth(1);
    setFixedSize(pixmap()->size());
  }
  else
    setHidden(true);
  
  mTimer = new QTimer(this);
  setInterval(5000);
  setSingleShot(false);
  connect(mTimer, SIGNAL(timeout()), SLOT(timeout()));

  KommanderPlugin::setDefaultGroup(Group::DBUS);
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
  if (mTimer->isActive())
  {
    mTimer->setInterval(a_interval);
  }
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
  KommanderWidget::setAssociatedText(a_text.split("\n"));
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
  setAssociatedText(KommanderWidget::evalAssociatedText(populationText()).split("\n"));
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
  return f == DBUS::setText || f == DBUS::execute || f == DBUS::cancel;
}

QString Timer::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::setText:
      setAssociatedText(args[0].split("\n"));
      break;
    case DBUS::execute:
      execute();
      break;
    case DBUS::cancel:
      cancel();
      break;
    case SetInterval:
      setInterval(args[0].toInt());
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}

#include "timer.moc"
