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
#include <qstringlist.h>

/* OTHER INCLUDES */
#include "pluginmanager.h"
#include "mainwindow.h"
#include "kommanderversion.h"

#include <iostream>
using namespace std;

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
  { "check", I18N_NOOP("Check all installed plugins and remove those missing"), 0},
  { "l", 0, 0},
  { "list", I18N_NOOP("List all installed plugins"), 0},
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
  KCmdLineArgs::init(argc, argv, &aboutData);
  KCmdLineArgs::addCmdLineOptions(options); // Add our own options.

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  KApplication app;
  
  if (!args->getOption("add").isNull() || !args->getOption("remove").isNull() || args->isSet("check")  || args->isSet("list"))
  {
    PluginManager P;
    if (args->isSet("check"))
      P.verify();
    
    QCStringList items = args->getOptionList("add");
    for (QCStringList::ConstIterator it = items.begin(); it != items.end(); ++it)
      if (!P.add(*it))
        cerr << i18n("Error adding plugin '%1'").arg(*it).local8Bit();
    
    items = args->getOptionList("remove");
    for (QCStringList::ConstIterator it = items.begin(); it != items.end(); ++it)
      if (!P.remove(*it))
        cerr << i18n("Error removing plugin '%1'").arg(*it).local8Bit();
    
    if (args->isSet("list"))
    {
      QStringList plugins = P.items();
      for (QStringList::ConstIterator it = plugins.begin(); it != plugins.end(); ++it)
        cout << (*it).local8Bit() << "\n";
    }
  }
  else 
  {
    MainWindow *mw = new MainWindow();
    app.setMainWidget(mw);
    mw->show();
    return app.exec();
  }
}
