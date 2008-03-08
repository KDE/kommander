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

/* KDE INCLUDES */
#include <kprocess.h>

/* QT INCLUDES */
#include <qcursor.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "konsole.h"

Konsole::Konsole(QWidget* a_parent, const char* a_name)
  : KTextEdit(a_parent), KommanderWidget(this), mSeenEOL(false)
{
  setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  mProcess = 0;
}

Konsole::~Konsole()
{
  delete mProcess;
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
  mProcess = new KProcess(this);
  connect(mProcess, SIGNAL(processExited(int, QProcess::ExitStatus)), SLOT(processExited(int, QProcess::ExitStatus)));
  connect(mProcess, SIGNAL(readyReadStandardOutput()), SLOT(processReceivedStdout()));
  setCursor(QCursor(Qt::WaitCursor));
  mProcess->setProgram(at);
  mProcess->start();
  if (!mProcess->waitForStarted()) {
    delete mProcess;
    mProcess = 0;
  }
}

void Konsole::cancel()
{
  if (!mProcess) 
    return;
  mProcess->kill(); // terminate FIXME
  processExited(mProcess->exitCode(), mProcess->exitStatus());
}


void Konsole::processReceivedStdout()
{
  QString pBuf = QString::fromLocal8Bit(mProcess->readAllStandardOutput().data());  
  if (mSeenEOL)
    pBuf = "\n" + pBuf;
  mSeenEOL = pBuf[pBuf.length()-1] == '\n';
  if (mSeenEOL)
    pBuf = pBuf.left(pBuf.length()-1);
  insertPlainText(pBuf);
}

void Konsole::processExited(int c, QProcess::ExitStatus exitStatus)
{
  unsetCursor();
  processReceivedStdout();
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
      return toPlainText();
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
