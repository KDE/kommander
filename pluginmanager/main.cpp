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


int main(int argc, char *argv[])
{
  KLocale::setMainCatalogue("kommander");
  KAboutData aboutData( "kmdr-plugins", 0, ki18n("Kommander Plugin Manager"),
    KOMMANDER_VERSION, ki18n(description), KAboutData::License_GPL,
    ki18n("(C) 2004-2005 Kommander authors"));
  aboutData.addAuthor(ki18n("Marc Britton"), ki18n("Original author"), "consume@optusnet.com.au");
  aboutData.addAuthor(ki18n("Eric Laffoon"), ki18n("Project manager"), "eric@kdewebdev.org");
  aboutData.addAuthor(ki18n("Michal Rudolf"), ki18n("Current maintainer"), "mrudolf@kdewebdev.org");
  KCmdLineArgs::init(argc, argv, &aboutData);

  KCmdLineOptions options;
  options.add("a");
  options.add("add <file>", ki18n("Register given library"));
  options.add("r");
  options.add("remove <file>", ki18n("Remove given library"));
  options.add("c");
  options.add("check", ki18n("Check all installed plugins and remove those missing"));
  options.add("l");
  options.add("list", ki18n("List all installed plugins"));
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
        cerr << i18n("Error adding plugin '%1'", *it).local8Bit();
    
    items = args->getOptionList("remove");
    for (QCStringList::ConstIterator it = items.begin(); it != items.end(); ++it)
      if (!P.remove(*it))
        cerr << i18n("Error removing plugin '%1'", *it).local8Bit();
    
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
