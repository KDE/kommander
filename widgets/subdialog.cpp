/***************************************************************************
                          subdialog.cpp - Subdialog widget 
                             -------------------
    copyright            : (C) 2002 by Marc Britton
    email                : consume@optusnet.com.au
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

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qpushbutton.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <kommanderfactory.h>
#include <kommanderwidget.h>
#include <specials.h>
#include "subdialog.h"

SubDialog::SubDialog(QWidget *a_parent, const char *a_name)
  : QPushButton(a_parent, a_name), KommanderWidget(this), m_dialog(0)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);

  connect(this, SIGNAL(clicked()), this, SLOT(showDialog()));
  
}

SubDialog::~SubDialog()
{
}

QString SubDialog::currentState() const
{
  return QString("default");
}

bool SubDialog::isKommanderWidget() const
{
  return true;
}

QStringList SubDialog::associatedText() const
{
  return KommanderWidget::associatedText();
}

void SubDialog::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void SubDialog::setPopulationText(const QString& a_text)
{
    KommanderWidget::setPopulationText( a_text );
}

QString SubDialog::populationText() const
{
    return KommanderWidget::populationText();
}

void SubDialog::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
//FIXME    
}


void SubDialog::setKmdrFile(QString a_kmdrFile)
{
  m_kmdrFile = a_kmdrFile;
}

QString SubDialog::kmdrFile() const
{
  return m_kmdrFile;
}

void SubDialog::showDialog()
{
  delete m_dialog;

  KommanderFactory::loadPlugins();
  m_dialog = (QDialog *)KommanderFactory::create(kmdrFile());
  if(!m_dialog)
    qWarning("Creation of sub dialog failed ..");
  connect(m_dialog, SIGNAL(finished()), this, SLOT(slotFinished()));

  m_dialog->exec();
}

void SubDialog::slotFinished()
{
  if(m_dialog)
  {
    KommanderWidget *atw = dynamic_cast<KommanderWidget *>(m_dialog);
    if(atw)
      emit widgetTextChanged(atw->evalAssociatedText());
  }
}

void SubDialog::showEvent( QShowEvent *e )
{
    QPushButton::showEvent( e );
    emit widgetOpened();
}

QString SubDialog::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
    {
      if (!m_dialog)
        return QString();
      KommanderWidget *atw = dynamic_cast<KommanderWidget *>(m_dialog);
      if(atw)
        return atw->evalAssociatedText();
    }
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}


#include "subdialog.moc"
