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

#ifndef KOMMANDER
#include "designerappiface.h"
#endif
#include "designerapp.h"
#include "mainwindow.h"
#include "formwindow.h"

#include <qfile.h>
#include <qdir.h>
#include <qsettings.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpixmap.h>

#ifdef Q_WS_WIN
#include <qt_windows.h>
#include <process.h>
#endif

#ifdef HAVE_KDE
#include <kapplication.h>
#include <klocale.h>
#endif

#include <cstdlib>

QPixmap splashScreen();

static QLabel *splash = 0;

void set_splash_status( const QString &txt )
{
    if ( !splash )
	return;
    splash->repaint( FALSE );
    QPainter p( splash );
    p.setPen( Qt::black );
    p.drawText( 5, splash->fontMetrics().height(), txt );
    QString s;//= "Licensed to " + QString::fromLatin1( QT_PRODUCT_LICENSEE );
    //p.drawText( 5, 2 * splash->fontMetrics().height() + 1, s );
    QApplication::flush();
}

DesignerApplication::DesignerApplication( int &argc, char **argv, const QCString &rAppName )
    : KApplication( argc, argv, rAppName )//, appIface( 0 )
{
  QStringList libraryPaths;
  setLibraryPaths(libraryPaths);
}


QLabel *DesignerApplication::showSplash()
{
  QRect screen = QApplication::desktop()->screenGeometry();
  QSettings config;
  config.insertSearchPath(QSettings::Windows, "/Trolltech");

  QRect mainRect;
  QString keybase = settingsKey();
  bool show = config.readBoolEntry(keybase + "SplashScreen", TRUE);
  mainRect.setX(config.readNumEntry(keybase + "Geometries/MainwindowX", 0));
  mainRect.setY(config.readNumEntry(keybase + "Geometries/MainwindowY", 0));
  mainRect.setWidth(config.readNumEntry(keybase + "Geometries/MainwindowWidth", 500));
  mainRect.setHeight(config.readNumEntry(keybase + "Geometries/MainwindowHeight", 500));
  screen =
      QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(mainRect.
          center()));

  if (show)
  {
    splash =
        new QLabel(0, "splash",
        WDestructiveClose | WStyle_Customize | WStyle_NoBorder | WX11BypassWM | WStyle_StaysOnTop);
    splash->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
    splash->setPixmap(splashScreen());
    splash->adjustSize();
    splash->setCaption(i18n("Kommander Editor"));
    splash->move(screen.center() - QPoint(splash->width() / 2, splash->height() / 2));
    splash->show();
    splash->repaint(FALSE);
    QApplication::flush();
  }

  return splash;
}

void DesignerApplication::closeSplash()
{
    splash->hide();
}

