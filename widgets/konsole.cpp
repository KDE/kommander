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
  : KListBox(a_parent, a_name), KommanderWidget(this), mSeenEOL(true), mProcess(false)
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
}


void Konsole::processReceivedStdout(MyProcess*, char* buffer, int buflen)
{
  QString pBuf = QString::fromLocal8Bit(buffer, buflen);  
  QStringList items = QStringList::split("\n", pBuf);
  insertStringList(items); 
  setCurrentItem(count()-1); 
  ensureCurrentVisible();
}

void Konsole::processExited(MyProcess* p)
{
  unsetCursor();
  delete p;
  mProcess = 0;
}

bool Konsole::isFunctionSupported(int f)
{
  return f == DCOP::setText or f == DCOP::clear or f == DCOP::execute or f == DCOP::cancel;
}

QString Konsole::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
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
      break;
  }
  return QString::null;
}


#include "konsole.moc"
