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

// KDE includes
#include <klocale.h>

// Qt includes
#include <qlabel.h>
#include <qobjectlist.h>
#include <qsettings.h>
#include <qtextstream.h>

// Other includes
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include "mainwindow.h"
#include "formwindow.h"
#include "designerapp.h"
#include "kommanderwidget.h"

#if defined(Q_WS_X11)
extern void qt_wait_for_window_manager( QWidget* );
#endif



#if defined(Q_OS_IRIX) && defined(Q_CC_GNU)
static void signalHandler()
#else
static void signalHandler( int )
#endif
{
  QFile f(QDir::homeDirPath() + "/.designerargs");
  f.open(IO_ReadOnly);
  QString args;
  f.readLine(args, f.size());
  QStringList lst = QStringList::split(" ", args);
  for (QStringList::Iterator it = lst.begin(); it != lst.end(); ++it)
  {
    QString arg = (*it).stripWhiteSpace();
    if (arg[0] != '-')
    {
      QObjectList *l = MainWindow::self->queryList("FormWindow");
      FormWindow *fw = (FormWindow *) l->first();
      bool haveit = FALSE;
      while (fw)
      {
        haveit = haveit || fw->fileName() == arg;
        fw = (FormWindow *) l->next();
      }
      if (!haveit)
      {
        MainWindow::self->openFormWindow(arg);
      }
    }
  }
  MainWindow::self->raise();
  MainWindow::self->setActiveWindow();
}



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



int main( int argc, char *argv[] )
{
  KommanderWidget::inEditor = true;
  QApplication::setColorSpec(QApplication::ManyColor);
  
  KLocale::setMainCatalogue("kommander");
  DesignerApplication a(argc, argv, "Qt Designer");

  DesignerApplication::setOverrideCursor(Qt::WaitCursor);

  bool creatPid = FALSE;
  if (a.argc() > 1)
  {
    QString arg1 = a.argv()[1];
    if (arg1 == "-client")
    {
      QFile pf(QDir::homeDirPath() + "/.designerpid");
      if (pf.exists() && pf.open(IO_ReadOnly))
      {
        QString pidStr;
        pf.readLine(pidStr, pf.size());
        QFile f(QDir::homeDirPath() + "/.designerargs");
        f.open(IO_WriteOnly);
        QTextStream ts(&f);
        for (int i = 1; i < a.argc(); ++i)
          ts << a.argv()[i] << " ";
        ts << endl;
        f.close();
        if (kill(pidStr.toInt(), SIGUSR1) == -1)
          creatPid = TRUE;
        else
          return 0;
      } else
      {
        creatPid = TRUE;
      }
    }
  }

  if (creatPid)
  {
    QFile pf(QDir::homeDirPath() + "/.designerpid");
    pf.open(IO_WriteOnly);
    QTextStream ts(&pf);
    signal(SIGUSR1, signalHandler);
    ts << getpid() << endl;

    pf.close();
    signal(SIGABRT, exitHandler);
    signal(SIGFPE, exitHandler);
    signal(SIGILL, exitHandler);
    signal(SIGINT, exitHandler);
    signal(SIGSEGV, exitHandler);
    signal(SIGTERM, exitHandler);
  }

  QLabel *splash = a.showSplash();

  MainWindow *mw = new MainWindow(creatPid);
  a.setMainWidget(mw);
  mw->setCaption("Kommander Dialog Editor");

  QSettings config;
  config.insertSearchPath(QSettings::Windows, "/Trolltech");

  if (config.readBoolEntry(DesignerApplication::settingsKey() + "Geometries/MainwindowMaximized",
          FALSE))
  {
    int x = config.readNumEntry(DesignerApplication::settingsKey() + "Geometries/MainwindowX", 0);
    int y = config.readNumEntry(DesignerApplication::settingsKey() + "Geometries/MainwindowY", 0);
    mw->move(x, y);
    mw->showMaximized();
  } else
  {
    mw->show();
  }
#if defined(Q_WS_X11)
  qt_wait_for_window_manager(mw);
#endif
  delete splash;

  QApplication::restoreOverrideCursor();
  for (int i = 1; i < a.argc(); ++i)
  {
    QString arg = a.argv()[i];
    if (arg[0] != '-')
      mw->fileOpen("", arg);
  }

  return a.exec();
}
