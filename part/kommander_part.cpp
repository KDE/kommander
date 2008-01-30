/***************************************************************************
    begin                : Wed Jan 30 2008
    copyright            : (C) 2008, Andras Mantia <amantia@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kommander_part.h"

#include <kdebug.h>

#include <kinstance.h>
#include <kparts/genericfactory.h>

#include <qpoint.h>
#include <qlayout.h>
#include <qtimer.h>

#include "instance.h"
#include "kommanderversion.h"

static const char *description =
    I18N_NOOP("Executor Part is a component of the Kommander dialog system that executes .kmdr files inside a KDE KPart");

typedef KParts::GenericFactory<KommanderPart> KommanderPartFactory;

K_EXPORT_COMPONENT_FACTORY( libkommander_part, KommanderPartFactory )

KommanderPart::KommanderPart( QWidget *parentWidget, const char * /*widgetName*/,
                              QObject *parent, const char *name, const QStringList & /*args*/ )
    : KParts::ReadOnlyPart(parent, name)
{
  setInstance( KommanderPartFactory::instance() );
  m_instance = 0L; 
  m_widget = new QWidget(parentWidget);
  setWidget(m_widget);
  m_layout = new QGridLayout(m_widget);
}

KommanderPart::~KommanderPart()
{
  delete m_instance;
}

KAboutData* KommanderPart::createAboutData()
{
  KAboutData * aboutData = new KAboutData("kommander_part", I18N_NOOP("Kommander Executor Part"),
    KOMMANDER_VERSION, description, KAboutData::License_GPL,
    "(c) 2008 Andras Mantia", I18N_NOOP("Part of the KDEWebDev module."),  "http://www.kdewebdev.org");
  
  aboutData->addAuthor("Andras Mantia", I18N_NOOP("Current maintainer"), "amantia@kdewebdev.org");
  aboutData->addAuthor("Michal Rudolf", I18N_NOOP("Previous maintainer"), "mrudolf@kdewebdev.org");
  aboutData->addAuthor("Marc Britton", I18N_NOOP("Original author"), "consume@optusnet.com.au");
  return aboutData;
}

bool KommanderPart::openFile()
{ 
  delete m_instance;
  m_instance = new Instance(0L);
  m_instance->build(m_url);
  QTimer::singleShot(0, this, SLOT(slotRun()));
  
  emit setStatusBarText( m_url.prettyURL() );
  return true;
}

void KommanderPart::slotRun()
{
  QWidget *w = m_instance->widget();
  if (w)
  {
    w->reparent(m_widget, 0, QPoint(0,0));
    m_layout->addWidget(w, 0, 0);
    w->show(); //show, not execute, so it doesn't block the parent
  }
}

#include "kommander_part.moc"
