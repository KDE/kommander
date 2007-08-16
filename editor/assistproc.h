/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef ASSISTPROC_H
#define ASSISTPROC_H

#include <q3process.h>
#include <qtimer.h>
#include <q3socket.h>
#include <qstring.h>

class AssistProc : public Q3Process
{
    Q_OBJECT
public:
    AssistProc( QObject *parent=0, const char * name = 0, const QString &path = QString::null );	//krazy:exclude=nullstrassign for old broken gcc
    ~AssistProc();
    bool run( const QString &path = QString::null );	//krazy:exclude=nullstrassign for old broken gcc

public slots:
    bool sendRequest( const QString &data );

private slots:
    void processOutput();
    void processErrOutput();

private:
    QString assistantPath;
};

#endif // ASSISTPROC_H
