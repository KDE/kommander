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
#include <klocale.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kapplication.h>

// Other includes
#include "mainwindow.h"
#include "kommanderwidget.h"

#define VERSION "1.1dev3"

static const char *description =
  I18N_NOOP("Kommander is a graphical editor of scripted dialogs.");
static const char *text =
  I18N_NOOP("Based on Qt Designer, (C) 2000 Trolltech AS.");

static KCmdLineOptions options[] =
{
  { "+file", I18N_NOOP("Dialog to open"), 0 },
  { 0, 0, 0 }
};

int main( int argc, char *argv[] )
{
  KAboutData aboutData( "Kommander", I18N_NOOP("Kommander"),
                        VERSION, description, KAboutData::License_GPL,
                        "(C) 2002-2004 Kommander authors", text);
  aboutData.addAuthor("Marc Britton", "Original author", "consume@optusnet.com.au");
  aboutData.addAuthor("Eric Laffoon", "Project manager", "eric@kdewebdev.org");
  aboutData.addAuthor("Michal Rudolf", "Current maintainer", "mrudolf@kdewebdev.org");
  aboutData.setTranslator (I18N_NOOP("_: NAME OF TRANSLATORS\nYour names"),
                           I18N_NOOP("_: EMAIL OF TRANSLATORS\nYour emails"));
  KCmdLineArgs::init(argc, argv, &aboutData);
  KCmdLineArgs::addCmdLineOptions(options);

  KommanderWidget::inEditor = true;

  KLocale::setMainCatalogue("kommander");
  KApplication a(true, true);
  MainWindow *mw = new MainWindow(false);
  a.setMainWidget(mw);
  mw->setCaption(i18n("Kommander Dialog Editor"));
  mw->show();

  return a.exec();
}

