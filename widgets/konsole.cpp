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
  : KTextEdit(a_parent, a_name), KommanderWidget(this), mSeenEOL(false), mProcess(false)
{
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
  return TRUE;
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
  QString at = evalAssociatedText().stripWhiteSpace();
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
  return f == DCOP::setText || f == DCOP::text || f == DCOP::clear || f == DCOP::execute || f == DCOP::cancel;
}

QString Konsole::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    case DCOP::text:
      return text();
    case DCOP::clear:
      clear();
      break;
    case DCOP::execute:
      execute();
      break;
    case DCOP::cancel:
      cancel();
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString::null;
}


#include "konsole.moc"
