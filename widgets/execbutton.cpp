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
#include <kprocess.h>

/* QT INCLUDES */
#include <qcursor.h>
#include <qevent.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "execbutton.h"
#include <iostream>

using namespace std;

ExecButton::ExecButton(QWidget* a_parent, const char* a_name)
  : KPushButton(a_parent), KommanderWidget(this)
{
  QStringList states;
  setObjectName(a_name);
  states << "default";
  setStates(states);
  setDisplayStates(states);
  setWriteStdout(true);
  setBlockGUI(Button);
  connect(this, SIGNAL(clicked()), this, SLOT(startProcess()));
  m_process = 0;
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
  if (m_process != 0)
    return;
  QString at = evalAssociatedText().trimmed();
  if (at.isEmpty())
      return;
  bool enabledStatus = isEnabled();
  if (m_blockGUI != None)
    setEnabled(false);
  if (m_blockGUI == GUI)
    KApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  m_process = new KProcess();
  connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(processExited(int, QProcess::ExitStatus)));
  //m_blockGUI == GUI;
  m_process->setProgram(at);
  m_process->start();
  if (m_blockGUI == GUI)
    if (m_process->waitForFinished())
    {
    }
  if (m_blockGUI == GUI)
  {
    KApplication::restoreOverrideCursor();
    if (writeStdout())
      cout << m_process->readAll().data() << flush; //FIXME m_output.data() << flush;
    delete m_process;
    m_process = 0;
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

void ExecButton::processExited(int c, QProcess::ExitStatus exitStatus)
{
  if (blockGUI() != None)
    setEnabled(true);
  if (m_process != 0)
  {
    if (writeStdout())
    {
      m_output = m_process->readAll();
      cout << m_output.data() << flush;
    }
    delete m_process;
    m_process = 0;
  }
}

void ExecButton::showEvent(QShowEvent* e)
{
  KPushButton::showEvent(e);
  emit widgetOpened();
}

bool ExecButton::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::setText;
}

QString ExecButton::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
      return m_output;
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}

#include "execbutton.moc"
