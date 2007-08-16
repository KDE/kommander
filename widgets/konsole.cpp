/***************************************************************************
                          konsole.cpp - Widget that shows output of a process
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
#include <qcursor.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "konsole.h"
#include <myprocess.h>

Konsole::Konsole(QWidget* a_parent, const char* a_name)
  : KTextEdit(a_parent), KommanderWidget(this), mSeenEOL(false), mProcess(false)
{
  this->setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
}

Konsole::~Konsole()
{
}

QString Konsole::currentState() const
{
  return QString("default");
}

bool Konsole::isKommanderWidget() const
{
  return true;
}

QStringList Konsole::associatedText() const
{
  return KommanderWidget::associatedText();
}

void Konsole::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void Konsole::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString Konsole::populationText() const
{
  return KommanderWidget::populationText();
}

void Konsole::populate()
{
  QString txt = KommanderWidget::evalAssociatedText(populationText());
  setWidgetText(txt);
}

void Konsole::setWidgetText(const QString&)
{
//  FIXME: emit widgetTextChanged(a_text);
}

void Konsole::execute()
{
  QString at = evalAssociatedText().trimmed();
  if (mProcess)
    cancel();
  mSeenEOL = false;
  mProcess = new MyProcess(this);
  mProcess->setBlocking(false);
  connect(mProcess, SIGNAL(processExited(MyProcess*)), SLOT(processExited(MyProcess*)));
  connect(mProcess, SIGNAL(processReceivedStdout(MyProcess*, char*, int)), SLOT(processReceivedStdout(MyProcess*, char*, int)));
  setCursor(QCursor(Qt::WaitCursor));
  mProcess->run(at);
}

void Konsole::cancel()
{
  if (!mProcess) 
    return;
  mProcess->cancel();
  processExited(mProcess);
}


void Konsole::processReceivedStdout(MyProcess*, char* buffer, int buflen)
{
  QString pBuf = QString::fromLocal8Bit(buffer, buflen);  
  if (mSeenEOL)
    pBuf = "\n" + pBuf;
  mSeenEOL = pBuf[pBuf.length()-1] == '\n';
  if (mSeenEOL)
    pBuf = pBuf.left(pBuf.length()-1);
  insert(pBuf);
}

void Konsole::processExited(MyProcess*)
{
  unsetCursor();
  delete mProcess;
  mProcess = 0;
  emit finished();
}

bool Konsole::isFunctionSupported(int f)
{
  return f == DBUS::setText || f == DBUS::text || f == DBUS::clear || f == DBUS::execute || f == DBUS::cancel;
}

QString Konsole::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    case DBUS::text:
      return text();
    case DBUS::clear:
      clear();
      break;
    case DBUS::execute:
      execute();
      break;
    case DBUS::cancel:
      cancel();
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}


#include "konsole.moc"
