/***************************************************************************
                         myprocess.cpp  - Wrapper class for running shell processes
                             -------------------
    copyright            : (C) 2002 by Marc Britton
    email                : consume@optusnet.com.au
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
#include <klocale.h>
#include <kprocess.h>
#include <klocale.h>

/* QT INCLUDES */
#include <qapplication.h>
#include <qobject.h>
#include <qstring.h>

/* OTHER INCLUDES */
#include "myprocess.h"
#include "kommanderwidget.h"

MyProcess::MyProcess(const KommanderWidget *a_atw)
  : m_atw(a_atw), m_loopStarted(false), m_blocking(true), mProcess(0)
{
}

void qt_enter_modal(QWidget *widget);
void qt_leave_modal(QWidget *widget);

void MyProcess::setBlocking(bool blocking)
{
  m_blocking = blocking;
}

QString MyProcess::output() const
{
  return m_output;  
}    
    
bool MyProcess::isBlocking() const
{
  return m_blocking;
}

  
void MyProcess::cancel()
{
  if (mProcess) {
    delete mProcess;
    mProcess = 0;
  }
}

QString MyProcess::run(const QString& a_command, const QString& a_shell)
{
  QString at = a_command.stripWhiteSpace();
  if (at.isEmpty())
  {
    emit processExited(0);
    return QString();
  }

  QString shellName = a_shell;
  if (shellName.isEmpty())
    shellName = "/bin/sh";

  // Look for shell
  if (at.startsWith("#!")) {
    int eol = at.find("\n");
    if (eol == -1)
      eol = at.length();
    shellName = at.mid(2, eol-1).stripWhiteSpace();
    at = at.mid(eol+1);
  }
  m_input = at.local8Bit();

  mProcess = new KProcess;
  (*mProcess) << shellName.latin1();

  connect(mProcess, SIGNAL(receivedStdout(KProcess*, char*, int)),
      SLOT(slotReceivedStdout(KProcess*, char*, int)));
  connect(mProcess, SIGNAL(processExited(KProcess*)), SLOT(slotProcessExited(KProcess*)));

  if(!mProcess->start(KProcess::NotifyOnExit, KProcess::All))
  {
    m_atw->printError(i18n("<qt>Failed to start shell process<br><b>%1</b></qt>").arg(shellName));
    return QString();
  }
  mProcess->writeStdin(m_input, m_input.length());
  mProcess->closeStdin();

  if (!m_blocking)
    return QString();
  else 
  {
    QWidget dummy(0, 0, WType_Dialog | WShowModal);
    dummy.setFocusPolicy(QWidget::NoFocus);
    m_loopStarted = true;
    qt_enter_modal(&dummy);
    qApp->enter_loop();
    qt_leave_modal(&dummy);
  
    if (!m_output.isEmpty() && m_output[m_output.length()-1] == '\n')
      return m_output.left(m_output.length()-1);
    else
      return m_output;
  }
}

void MyProcess::slotReceivedStdout(KProcess*, char* a_buffer, int a_len)
{
  m_output += QString::fromLocal8Bit(a_buffer, a_len);
  emit processReceivedStdout(this, a_buffer, a_len);
}

void MyProcess::slotProcessExited(KProcess* process)
{
  if (m_loopStarted)
  {
    qApp->exit_loop();
    m_loopStarted = false;
  }
  delete process;
  if (!m_blocking)
    emit processExited(this);
  mProcess = 0;
}

#include "myprocess.moc"
