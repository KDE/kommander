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

