/***************************************************************************
                          main.cpp  -  Kommander plugin manager interface
                             -------------------
    begin                : Tue Aug 13 09:31:50 EST 2002
    copyright            : (C) 2004 by Marc Britton
    email                : consume@optushome.com.au
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
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kurl.h>
#include <qapplication.h>
#include <qobject.h>
#include <kapplication.h>

/* QT INCLUDES */
#include <qptrlist.h>
#include <qfile.h>

/* OTHER INCLUDES */
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "mainwindow.h"


using std::cout;
using std::endl;
using std::cerr;


static const char *description =
	I18N_NOOP("kmdr-plugins is a component of the Kommander dialog system that manages installed plugins."); 
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE


#define VERSION "1.0alpha6"

static KCmdLineOptions options[] =
{
  KCmdLineLastOption
};

int main(int argc, char *argv[])
{
  KLocale::setMainCatalogue("kommander");
  KAboutData aboutData( "kmdr-plugins", I18N_NOOP("Kommander Plugin Manager"),
    VERSION, description, KAboutData::License_GPL,
    "(c) 2004, Marc Britton", 0, 0, "consume@optushome.com.au");
  aboutData.addAuthor("Marc Britton",0, "consume@optushome.com.au");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  KApplication app;
  MainWindow *mw = new MainWindow();
  app.setMainWidget( mw );
  mw->show();
  return app.exec();
}
