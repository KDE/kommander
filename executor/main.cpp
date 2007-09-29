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
#include <kglobal.h>
#include <klocale.h>
#include <kurl.h>
#include <kmessagebox.h>

/* QT INCLUDES */
#include <qapplication.h>
#include <qobject.h>
#include <qstring.h>

/* OTHER INCLUDES */
#include <cstdio>
#include <cstdlib>
#include "instance.h"
#include <iostream>
#include <kommanderversion.h>

using std::cout;
using std::endl;
using std::cerr;


static const char *description =
	I18N_NOOP("Executor is a component of the Kommander dialog system that executes .kmdr files given as arguments or via stdin");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE

static KCmdLineOptions options[] =
{
  { "!stdin", I18N_NOOP("Read dialog from standard input"), 0},
  { "c <catalog>", I18N_NOOP("Use given catalog for translation"), 0},      
  { "+[file]", I18N_NOOP("Dialog to open"), 0 },
  KCmdLineLastOption
};

int main(int argc, char *argv[])
{
  KAboutData aboutData( "kmdr-executor", I18N_NOOP("Kommander Executor"),
    KOMMANDER_VERSION, description, KAboutData::License_GPL,
    "(c) 2002, Marc Britton", 0, 0, "consume@optushome.com.au");
  aboutData.addAuthor("Marc Britton", 0, "consume@optushome.com.au");
  aboutData.addAuthor("Michal Rudolf", 0, "mrudolf@kdewebdev.org");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  if (args->isSet("c"))
    KLocale::setMainCatalogue(args->getOption("c"));
  else if (args->count())
  {
    char buf[200];
    QString baseFile = args->url(0).fileName();
    int ext = baseFile.findRev('.');
    if (ext != -1)
      baseFile = baseFile.left(ext);
    strcpy(buf, baseFile.latin1());
    KLocale::setMainCatalogue(buf);
  }
  else
    KLocale::setMainCatalogue("Kommander");
  KApplication app;

  QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
  if (!args->count())
  {
    KMessageBox::sorry(0, i18n("Error: no dialog given. Use --stdin option to read dialog from standard input.\n"));
    return -1;
  }
  Instance instance;
  if (!instance.build(args->isSet("stdin") ? QString() : args->url(0)))
    return -1;

  // Read command-line variables
  QStringList cmdargs;
  for (int i = !args->isSet("stdin"); i<args->count(); i++)
    cmdargs.append(args->arg(i));
  instance.addCmdlineArguments(cmdargs);

  instance.run();
  return 0;
}
