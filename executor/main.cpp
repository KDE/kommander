/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Tue Aug 13 09:31:50 EST 2002
    copyright            : (C) 2002 by Marc Britton
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
#include <kapp.h>

/* QT INCLUDES */
#include <qptrlist.h>
#include <qfile.h>

/* OTHER INCLUDES */
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "instance.h"

using std::cout;
using std::endl;
using std::cerr;


static const char *description =
	I18N_NOOP("Executor is a component of the Kommander dialog system that executes .kmdr files given as arguments or via stdin");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE


#define VERSION "beta3"

static KCmdLineOptions options[] =
{
  { "+[file]", I18N_NOOP("Dialog to open"), 0 },
  { 0, 0, 0 }
};

int main(int argc, char *argv[])
{
  KAboutData aboutData( "kmdr-executor", I18N_NOOP("Kommander Executor"),
    VERSION, description, KAboutData::License_GPL,
    "(c) 2002, Marc Britton", 0, 0, "consume@optushome.com.au");
  aboutData.addAuthor("Marc Britton",0, "consume@optushome.com.au");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  KApplication app;

  QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
  if(args->count() == 0) // read from stdin
  {
	 QFile inputFile;
	 inputFile.open(IO_ReadOnly, stdin);

	 Instance *instance = new Instance;
	 instance->run(&inputFile);
  }
  else
  {
	 // load dialog specified on the command line
	  QString dlgFileName;
	  for(int i = 0;i < args->count();++i)
	  {
		KURL url = args->url(i);
		if(url.isLocalFile())                 // FIXME : Pointless repitition
		{
		  Instance *instance = new Instance(url.path(), 0);
		  instance->run();
		}
	  }
  }
  return 0;
}
