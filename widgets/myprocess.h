/***************************************************************************
                          myprocess.h - Wrapper class for running shell processes 
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
#ifndef _HAVE_MYPROCESS_H_
#define _HAVE_MYPROCESS_H_

/* KDE INCLUDES */
#include <kprocess.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qobject.h>

class KommanderWidget;

class MyProcess : QObject
{
  Q_OBJECT
public:
  MyProcess(const KommanderWidget *);
  // Run given command, using a_shell as a shell (this can be overriden by shebang in the first line)
  // Process is run in blocking mode.
  QString run(const QString& a_command, const QString& a_shell = "/bin/sh");
private slots:
  void slotReceivedStdout(KProcess*, char*, int);
  void slotProcessExited(KProcess*);
protected:
  const KommanderWidget *m_atw;
  QString m_output;
  QCString m_input;
  bool m_loopStarted;
};

#endif
