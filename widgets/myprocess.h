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

class AssocTextWidget;

class MyProcess : QObject
{
	Q_OBJECT
public:
	MyProcess(const AssocTextWidget *);
public slots:
	QString run(QString);
	void receivedStdout(KProcess *, char *, int);
protected:
	const AssocTextWidget *m_atw;
	QString m_output;
};

#endif
