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
#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kurl.h>

/* QT INCLUDES */
#include <qapplication.h>
#include <qfile.h>
#include <qobject.h>
#include <qptrlist.h>
#include <qstring.h>

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


#define VERSION "1.0alpha6"

static KCmdLineOptions options[] =
{
  { "!stdin", I18N_NOOP("Read dialog from standard input"), 0},
  { "+[file]", I18N_NOOP("Dialog to open"), 0 },
  KCmdLineLastOption
};

int main(int argc, char *argv[])
{
  KLocale::setMainCatalogue("kommander");
  KAboutData aboutData( "kmdr-executor", I18N_NOOP("Kommander Executor"),
    VERSION, description, KAboutData::License_GPL,
    "(c) 2002, Marc Britton", 0, 0, "consume@optushome.com.au");
  aboutData.addAuthor("Marc Britton",0, "consume@optushome.com.au");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  KApplication app;
  
  QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
  Instance* instance = 0;
  QFile inputFile;
  
  if (args->isSet("stdin"))
  {
    inputFile.open(IO_ReadOnly, stdin);
    instance = new Instance;
  }
  else if (!args->count())
  {
    qFatal("Error: no dialog given. Use --stdin option to read dialog from standard input.\n");
    return -1;
  }
  else
  {
    KURL url = args->url(0);
    instance = new Instance(url, 0);
  }
  
  // Read command-line variables
  for (int i=!args->isSet("stdin"); i<args->count(); i++)
    instance->addArgument(args->arg(i));
  
  if (args->isSet("stdin"))
    instance->run(&inputFile);
  else
    instance->run();
  return 0;
}
