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
//Added by qt3to4:
#include "kommander_export.h"

class KommanderWidget;

class KOMMANDER_EXPORT MyProcess : public QObject
{
  Q_OBJECT
public:
  MyProcess(const KommanderWidget *);
  // Run given command, using a_shell as a shell (this can be overriden by shebang in the first line)
  // Process is run in blocking mode.
  QString run(const QString& a_command, const QString& a_shell = "/bin/sh");
  // Kill running process
  void cancel();
  void setBlocking(bool blocking);
  bool isBlocking() const;
  QString output() const;
signals:
  void processExited(MyProcess*);
  void processReceivedStdout(MyProcess*, char*, int);
private slots:
  void slotReceivedStdout(KProcess*, char*, int);
  void slotProcessExited(KProcess*);
protected:
  const KommanderWidget *m_atw;
  QString m_output;
  QString m_input;
  bool m_loopStarted;
  bool m_blocking;
  bool m_handleOutput;
  KProcess* mProcess;
};

#endif
