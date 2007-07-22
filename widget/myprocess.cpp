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
#include <k3process.h>
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
  QString at = a_command.trimmed();
  if (at.isEmpty())
  {
    emit processExited(0);
    return QString::null;
  }
  
  QString shellName = a_shell;
  if (shellName.isEmpty())
    shellName = "/bin/sh";

  // Look for shell
  if (at.startsWith("#!")) {
    int eol = at.indexOf("\n");
    if (eol == -1)
      eol = at.length();
    shellName = at.mid(2, eol-1).trimmed();
    at = at.mid(eol+1);
  }
  m_input = at.toLocal8Bit();

  mProcess = new K3Process;
  (*mProcess) << shellName.toLatin1();

  connect(mProcess, SIGNAL(receivedStdout(K3Process*, char*, int)),
      SLOT(slotReceivedStdout(K3Process*, char*, int)));
  connect(mProcess, SIGNAL(processExited(K3Process*)), SLOT(slotProcessExited(K3Process*)));

  if(!mProcess->start(K3Process::NotifyOnExit, K3Process::All))
  {
    m_atw->printError(i18n("<qt>Failed to start shell process<br><b>%1</b></qt>", shellName));
    return QString::null;
  }
  mProcess->writeStdin(m_input, m_input.length());
  mProcess->closeStdin();

  if (!m_blocking)
    return QString::null;
  else 
  {
    //FIXME KProcess with blocking feature
/*    QWidget dummy(0, 0);
    dummy.setFocusPolicy(QWidget::NoFocus);
    m_loopStarted = true;
    qt_enter_modal(&dummy);
    qApp->enter_loop();
    qt_leave_modal(&dummy);
*/  
    if (!m_output.isEmpty() && m_output[m_output.length()-1] == '\n')
      return m_output.left(m_output.length()-1);
    else
      return m_output;
  }
}

void MyProcess::slotReceivedStdout(K3Process*, char* a_buffer, int a_len)
{
  m_output += QString::fromLocal8Bit(a_buffer, a_len);
  emit processReceivedStdout(this, a_buffer, a_len);
}

void MyProcess::slotProcessExited(K3Process* process)
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

//#include "myprocess.moc"
