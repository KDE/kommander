/***************************************************************************
                          execbutton.cpp - Button that runs its text association 
                             -------------------
    copyright            : (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
                           (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
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
#include <kapplication.h>
#include <klocale.h>
#include <kmessagebox.h>

/* QT INCLUDES */
#include <qcursor.h>
#include <qevent.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "execbutton.h"
#include <myprocess.h>
#include <iostream>

using namespace std;

ExecButton::ExecButton(QWidget* a_parent, const char* a_name)
  : KPushButton(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  setWriteStdout(true);
  setBlockGUI(Button);
  connect(this, SIGNAL(clicked()), this, SLOT(startProcess()));
}

ExecButton::~ExecButton()
{
}

QString ExecButton::currentState() const
{
  return QString("default");
}

bool ExecButton::isKommanderWidget() const
{
  return true;
}

QStringList ExecButton::associatedText() const
{
  return KommanderWidget::associatedText();
}

void ExecButton::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void ExecButton::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString ExecButton::populationText() const
{
  return KommanderWidget::populationText();
}

void ExecButton::populate()
{
  QString txt = KommanderWidget::evalAssociatedText(populationText());
  setWidgetText(txt);
}

void ExecButton::setWidgetText(const QString& a_text)
{
  setText(a_text);
  emit widgetTextChanged(a_text);
}

void ExecButton::startProcess()
{
  QString at = evalAssociatedText().stripWhiteSpace();
  bool enabledStatus = isEnabled();
  if (m_blockGUI != None)
    setEnabled(false);
  if (m_blockGUI == GUI)
    KApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  MyProcess* process = new MyProcess(this);
  process->setBlocking(m_blockGUI == GUI);
  connect(process, SIGNAL(processExited(MyProcess*)), SLOT(processExited(MyProcess*)));
  m_output = process->run(at);
  if (m_blockGUI == GUI)
  {
    KApplication::restoreOverrideCursor();
    if (writeStdout())
      cout << m_output << flush;
  }
  setEnabled(enabledStatus);
}


bool ExecButton::writeStdout() const
{
  return m_writeStdout;
}

void ExecButton::setWriteStdout(bool a_enable)
{
  m_writeStdout = a_enable;
}

void ExecButton::setBlockGUI(Blocking a_enable)
{
  m_blockGUI = a_enable;
}

ExecButton::Blocking ExecButton::blockGUI() const
{
  return m_blockGUI;
}

void ExecButton::processExited(MyProcess* p)
{
  if (blockGUI() != None)
    setEnabled(true);
  if (p)
  {
    m_output = p->output();
    if (writeStdout())
      cout << m_output << flush;
    delete p;
  }
}

void ExecButton::showEvent(QShowEvent* e)
{
  KPushButton::showEvent(e);
  emit widgetOpened();
}

bool ExecButton::isFunctionSupported(int f)
{
  return f == DCOP::text || f == DCOP::setText || f == DCOP::execute;
}

QString ExecButton::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::text:
      return m_output;
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    case DCOP::execute:
      startProcess();
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}


#include "execbutton.moc"
