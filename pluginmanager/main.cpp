/***************************************************************************
                          main.cpp  -  Kommander plugin manager interface
                             -------------------
    begin                : Tue Aug 13 09:31:50 EST 2002
    copyright            : (C) 2004 Marc Britton <consume@optushome.com.au>
                           (C) 2005 Michal Rudolf <mrudolf@kdewebdev.org>
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
#include <kommanderversion.h>


using std::cout;
using std::endl;
using std::cerr;


static const char *description =
	I18N_NOOP("kmdr-plugins is a component of the Kommander dialog system that manages installed plugins."); 
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE


static KCmdLineOptions options[] =
{
  { "a", 0, 0},
  { "add <file>", I18N_NOOP("Register given library"), 0},      
  { "r", 0, 0},
  { "remove <file>", I18N_NOOP("Remove given library"), 0},      
  { "c", 0, 0},
  { "check", I18N_NOOP("Check all intalled plugins and remove those missing"), 0},
  KCmdLineLastOption
};

int main(int argc, char *argv[])
{
  KLocale::setMainCatalogue("kommander");
  KAboutData aboutData( "kmdr-plugins", I18N_NOOP("Kommander Plugin Manager"),
    KOMMANDER_VERSION, description, KAboutData::License_GPL,
    "(C) 2004-2005 Kommander authors");
  aboutData.addAuthor("Marc Britton", "Original author", "consume@optusnet.com.au");
  aboutData.addAuthor("Eric Laffoon", "Project manager", "eric@kdewebdev.org");
  aboutData.addAuthor("Michal Rudolf", "Current maintainer", "mrudolf@kdewebdev.org");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  KApplication app;
  MainWindow *mw = new MainWindow();
  app.setMainWidget( mw );
  mw->show();
  return app.exec();
}
