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
#include "assistproc.h"

AssistProc::AssistProc( QObject *parent, const char *name, const QString &path )
    : QProcess( parent, name )
{
    assistantPath = path;
    connect( this, SIGNAL(readyReadStdout()), this, SLOT(processOutput()) );
    connect( this, SIGNAL(readyReadStderr()), this, SLOT(processErrOutput()) );
}

AssistProc::~AssistProc()
{
    tryTerminate();
    QTimer::singleShot( 5000, this, SLOT(kill())); // If busy, kill in 5 sec...
}

bool AssistProc::run( const QString &path )
{
    if ( !path.isEmpty() )
	assistantPath = path;

    clearArguments();
    addArgument( assistantPath );
    addArgument( "-stdin" );

    if ( !start() ) {
	qWarning( QString("Assistant [exe]: Couldn't start \'%1\', please check path.").arg(assistantPath) );
	return FALSE;
    }
    return TRUE;
}

bool AssistProc::sendRequest( const QString &data )
{
    if ( isRunning() ) {
	writeToStdin( data );
    } else {
	if ( run() )              // If not running, try to start
	    writeToStdin( data ); // and display requested page.
	else
	    return FALSE;
    }
    return TRUE;
}

void AssistProc::processOutput()
{
    qWarning( QString("Assistant [out]: %1").arg(QString(readStdout())) );
}

void AssistProc::processErrOutput()
{
    qWarning( QString("Assistant [err]: %1").arg(QString(readStderr())) );
}
