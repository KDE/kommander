/***************************************************************************
                          main.cpp - initialization
                             -------------------
    copyright            : (C) 2004    Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// KDE includes
#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <klocale.h>
#include <ksplashscreen.h>

// Other includes
#include "mainwindow.h"
#include "kommanderwidget.h"
#include "kommanderversion.h"

static const char *description =
  I18N_NOOP("Kommander is a graphical editor of scripted dialogs.");
static const char *text =
  I18N_NOOP("Based on Qt Designer, (C) 2000 Trolltech AS.");

int main( int argc, char *argv[] )
{
  KAboutData aboutData( "kmdr-editor", 0, ki18n("Kommander"),
                        KOMMANDER_VERSION, ki18n(description), KAboutData::License_GPL,
                        ki18n("(C) 2002-2005 Kommander authors"), ki18n(text));
  aboutData.addAuthor(ki18n("Marc Britton"), ki18n("Original author"), "consume@optusnet.com.au");
  aboutData.addAuthor(ki18n("Eric Laffoon"), ki18n("Project manager"), "eric@kdewebdev.org");
  aboutData.addAuthor(ki18n("Michal Rudolf"), ki18n("Current maintainer"), "mrudolf@kdewebdev.org");
  aboutData.setTranslator (ki18nc("NAME OF TRANSLATORS", "Your names"),
                           ki18nc("EMAIL OF TRANSLATORS", "Your emails"));
  KCmdLineArgs::init(argc, argv, &aboutData);

  KCmdLineOptions options;
  options.add("+file", ki18n("Dialog to open"));
  KCmdLineArgs::addCmdLineOptions(options);

  KommanderWidget::inEditor = true;

  KLocale::setMainCatalogue("kommander");
  KApplication a(true, true);
  
  KSharedConfig::Ptr config = KGlobal::config();
  config->setGroup("General");
  bool splashScreen = config->readBoolEntry("SplashScreen", true);
  KSplashScreen* splash = 0;
  if (splashScreen) {
     splash = new KSplashScreen(UserIcon("kommandersplash"));
    splash->show();
  }
  
  MainWindow *mw = new MainWindow(false);
  a.setMainWidget(mw);
  mw->setCaption(i18n("Kommander Dialog Editor"));
  mw->show();
  
  if (splashScreen) {
    splash->finish(mw);
    delete splash;
  }

  return a.exec();
}

