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
    emit processExited(0, 0, QProcess::NormalExit);
    return QString();
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

  mProcess = new KProcess;
  (*mProcess) << shellName.toLatin1();

  connect(mProcess, SIGNAL(readyReadStandardOutput(KProcess*, char*, int)),
     SLOT(slotReceivedStdout()));
  connect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(slotProcessExited(int, QProcess::ExitStatus)));
  
  if (!m_blocking)
  {
    mProcess->start();
    mProcess->write(m_input.toUtf8(), m_input.length());
    return QString();
  }
  else 
  {
    int returncode;
    returncode = mProcess->execute(10000); //10 seconds timeout
    if (!m_output.isEmpty() && m_output[m_output.length()-1] == '\n')
      return m_output.left(m_output.length()-1);
    else
      return m_output;
  }
  return QString();
}

void MyProcess::slotReceivedStdout()
{
  QString data = QString::fromLocal8Bit(mProcess->readAllStandardOutput().data());
  m_output += data;
  emit processReceivedStdout(this, data);
}

void MyProcess::slotProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
{
  if (!m_blocking)
    emit processExited(this, exitCode, exitStatus);
  delete mProcess;
  mProcess = 0;
}

#include "myprocess.moc"
