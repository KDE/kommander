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

#include "mainwindow.h"
#include "formwindow.h"

#include "designerapp.h"

#include <qtextstream.h>
#include <qobjectlist.h>
#include <qlabel.h>
#include <qsettings.h>

#include <stdlib.h>
#include <signal.h>

#if defined(Q_OS_SCO) && defined(raise)
#  undef raise // fix for broken headers on SCO OpenServer 5.0.5
#endif

#if defined(Q_WS_WIN)
#include <qt_windows.h>
#include <process.h>
#endif

#if defined(Q_WS_X11)
extern void qt_wait_for_window_manager( QWidget* );
#endif

#if defined(Q_OS_UNIX)
#include <sys/types.h>
#include <unistd.h>

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

#if defined(Q_OS_IRIX) && defined(Q_CC_GNU)
static void signalHandler()
#else
static void signalHandler( int )
#endif
{
    QFile f( QDir::homeDirPath() + "/.designerargs" );
    f.open( IO_ReadOnly );
    QString args;
    f.readLine( args, f.size() );
    QStringList lst = QStringList::split( " ", args );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
	QString arg = (*it).stripWhiteSpace();
	if ( arg[0] != '-' ) {
	    QObjectList* l = MainWindow::self->queryList( "FormWindow" );
	    FormWindow* fw = (FormWindow*) l->first();
#if 0 // ### what's this dead code for?
	    FormWindow* totop;
#endif
	    bool haveit = FALSE;
	    while ( fw ) {
		haveit = haveit || fw->fileName() == arg;
#if 0 // ### what's this dead code for?
		if ( haveit )
		    totop = fw;
#endif
		fw = (FormWindow*) l->next();
	    }
	    if ( !haveit )
	    {
		MainWindow::self->openFormWindow( arg );
	    }
	}
    }
    MainWindow::self->raise();
    MainWindow::self->setActiveWindow();
}

#if defined(Q_C_CALLBACKS)
}
#endif

#endif

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

#if defined(Q_OS_IRIX) && defined(Q_CC_GNU)
static void exitHandler()
#else
static void exitHandler( int )
#endif
{
    QDir d( QDir::homeDirPath() );
    d.remove( ".designerpid" );
    exit( -1 );
}

#if 0
#if defined(QT_NO_DEBUG)
#if defined(Q_OS_IRIX) && defined(Q_CC_GNU)
static void crashHandler()
#else
static void crashHandler( int )
#endif
{
    if ( MainWindow::self )
	MainWindow::self->saveAllTemp();
    ::exit( -1 );
}
#endif
#endif

#if defined(Q_C_CALLBACKS)
}
#endif

#ifdef Q_OS_MACX
#include <stdlib.h>
#include <qdir.h>
#endif

int main( int argc, char *argv[] )
{
#if defined(QT_NO_DEBUG)
#if 0
    signal( SIGSEGV, crashHandler );
#endif
#endif

    QApplication::setColorSpec( QApplication::ManyColor );

#if defined(HAVE_KDE)
    KLocale::setMainCatalogue("kcommander");
    DesignerApplication a( argc, argv, "Qt Designer" );
#else
    DesignerApplication a( argc, argv );
#endif

#ifdef Q_OS_MACX
    QString qdir = QDir::cleanDirPath(QDir::currentDirPath() + QDir::separator() + "..");
    setenv("QTDIR", qdir, 0);
    setenv("PATH",  qdir + QDir::separator() + "bin" + ":" + getenv("PATH"), 0);
#endif

#if defined (QT_REMOTE_CONTROL)
    // {9A6A9A56-011B-4FEE-9B83-5BAD85071729}
    static const QUuid id = QUuid( 0x9a6a9a56, 0x011b, 0x4fee, 0x9b, 0x83, 0x5b, 0xad, 0x85, 0x07, 0x17, 0x29 );
    a.setEnableRemoteControl(TRUE,id);
#endif

    DesignerApplication::setOverrideCursor( Qt::WaitCursor );

    bool creatPid = FALSE;
    if ( a.argc() > 1 ) {
	QString arg1 = a.argv()[ 1 ];
	if ( arg1 == "-client" ) {
	    QFile pf( QDir::homeDirPath() + "/.designerpid" );
	    if ( pf.exists() && pf.open( IO_ReadOnly ) ) {
		QString pidStr;
		pf.readLine( pidStr, pf.size() );
		QFile f( QDir::homeDirPath() + "/.designerargs" );
		f.open( IO_WriteOnly );
		QTextStream ts( &f );
		for ( int i = 1; i < a.argc(); ++i )
		    ts << a.argv()[ i ] << " ";
		ts << endl;
		f.close();
#if defined(Q_OS_UNIX)
		if ( kill( pidStr.toInt(), SIGUSR1 ) == -1 )
		    creatPid = TRUE;
		else
		    return 0;
#elif defined(Q_OS_WIN32)
		if ( !GetProcessVersion( pidStr.toUInt() ) ) {
		    creatPid = TRUE;
		} else {
		    if ( a.winVersion() & Qt::WV_NT_based )
			    SendMessage( HWND_BROADCAST, RegisterWindowMessage((TCHAR*)"QT_DESIGNER_OPEN_FILE"), 0, 0 );
		    else
			    SendMessage( HWND_BROADCAST, RegisterWindowMessageA("QT_DESIGNER_OPEN_FILE"), 0, 0 );
		    return 0;
		}
#endif
	    } else {
		creatPid = TRUE;
	    }
	} else if(arg1 == "-debug_stderr") {
	    extern bool debugToStderr; //outputwindow.cpp
	    debugToStderr = TRUE;
	}
    }

    if ( creatPid ) {
	QFile pf( QDir::homeDirPath() + "/.designerpid" );
	pf.open( IO_WriteOnly );
	QTextStream ts( &pf );
#if defined(Q_OS_UNIX)
	signal( SIGUSR1, signalHandler );
#endif
	ts << getpid() << endl;

	pf.close();
	signal( SIGABRT, exitHandler );
	signal( SIGFPE, exitHandler );
	signal( SIGILL, exitHandler );
	signal( SIGINT, exitHandler );
	signal( SIGSEGV, exitHandler );
	signal( SIGTERM, exitHandler );
    }

    QLabel *splash = a.showSplash();

    MainWindow *mw = new MainWindow( creatPid );
    a.setMainWidget( mw );
#ifndef KOMMANDER
#if defined(QT_NON_COMMERCIAL)
    mw->setCaption( "Qt Designer by Trolltech for non-commercial use" );
#else
    mw->setCaption( "Qt Designer by Trolltech" );
#endif
#else
    mw->setCaption("Kommander Editor");
#endif

    QSettings config;
    config.insertSearchPath( QSettings::Windows, "/Trolltech" );

    if ( config.readBoolEntry( DesignerApplication::settingsKey() + "Geometries/MainwindowMaximized", FALSE ) ) {
	int x = config.readNumEntry( DesignerApplication::settingsKey() + "Geometries/MainwindowX", 0 );
	int y = config.readNumEntry( DesignerApplication::settingsKey() + "Geometries/MainwindowY", 0 );
	mw->move( x, y );
	mw->showMaximized();
    } else {
	mw->show();
    }
#if defined(Q_WS_X11)
    qt_wait_for_window_manager( mw );
#endif
    delete splash;

    QApplication::restoreOverrideCursor();
    for ( int i = 1; i < a.argc(); ++i ) {
	QString arg = a.argv()[ i ];
	if ( arg[0] != '-' )
#ifndef KOMMANDER
	    mw->fileOpen( "", "", arg );
#else
	    mw->fileOpen("", arg);
#endif
    }

    return a.exec();
}
