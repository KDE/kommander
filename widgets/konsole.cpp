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
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "konsole.h"
#include <myprocess.h>

Konsole::Konsole(QWidget* a_parent, const char* a_name)
  : KListBox(a_parent, a_name), KommanderWidget(this)
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

void Konsole::setWidgetText(const QString& a_text)
{
//  emit widgetTextChanged(a_text);
}

void Konsole::execute()
{
  QString at = evalAssociatedText().stripWhiteSpace();
  MyProcess* process = new MyProcess(this);
  process->setBlocking(false);
  connect(process, SIGNAL(processExited(MyProcess*)), SLOT(processExited(MyProcess*)));
  connect(process, SIGNAL(processReceivedStdout(MyProcess*, char*, int)), SLOT(processReceivedStdout(MyProcess*, char*, int)));
  process->run(at);
}


void Konsole::processReceivedStdout(MyProcess*, char* buffer, int buflen)
{
  QString pBuf = QString::fromLocal8Bit(buffer, buflen);  
  QStringList items = QStringList::split("\n", pBuf);
  insertStringList(items);  
}

void Konsole::processExited(MyProcess* p)
{
  delete p;
}

bool Konsole::isFunctionSupported(int f)
{
  return f == DCOP::setText or f == DCOP::clear or f == DCOP::execute;
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
    default:
      break;
  }
  return QString::null;
}


#include "konsole.moc"
