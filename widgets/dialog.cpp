/***************************************************************************
                         dialog.cpp - Main dialog widget 
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
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qdialog.h>

/* OTHER INCLUDES */
#include "kommanderwidget.h"
#include "dialog.h"
#include "myprocess.h"

Dialog::Dialog(QWidget *a_parent, const char *a_name, bool a_modal, int a_flags)
	: QDialog(a_parent, a_name, a_modal, a_flags), KommanderWidget(this)
{
	QStringList states;
	states << "default";
	states << "initialization";
	states << "destroy";
  setStates(states);
	setDisplayStates(states);
}

Dialog::~Dialog()
{
}

QString Dialog::currentState() const
{
  return QString("default");
}

bool Dialog::isKommanderWidget() const
{
  return true;
}

QStringList Dialog::associatedText() const
{
  return KommanderWidget::associatedText();
}

void Dialog::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void Dialog::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText( a_text );
}

QString Dialog::populationText() const
{
  return KommanderWidget::populationText();
}

void Dialog::populate()
{
  QString txt = KommanderWidget::evalAssociatedText( populationText() );
  setWidgetText( txt );
}

void Dialog::initialize()
{    
  const QStringList assoc = associatedText();
  if (assoc.count() > 1 && !assoc[1].isEmpty()) 
  {
    MyProcess proc(this);
    proc.run( KommanderWidget::evalAssociatedText(assoc[1]) );
  }
}

void Dialog::destroy()
{    
  const QStringList assoc = associatedText();
  if (assoc.count() > 2 && !assoc[2].isEmpty()) 
  {
    MyProcess proc(this);
    proc.run( KommanderWidget::evalAssociatedText(assoc[2]) );
  }
}


void Dialog::setWidgetText(const QString& a_text)
{
  setCaption(a_text);
  emit widgetTextChanged(a_text);
}

QString Dialog::widgetText() const
{
  return caption();
}

void Dialog::setSelectedWidgetText(const QString &)
{
}

QString Dialog::selectedWidgetText() const
{
  return QString::null;
}

void Dialog::exec()
{
  QDialog::exec();
  emit finished();
}

void Dialog::show()
{
  QDialog::show();
  if (!inEditor)
    initialize();
}

void Dialog::done(int r)
{
  if (!inEditor)
    destroy();
  QDialog::done(r);  
}

void Dialog::showEvent( QShowEvent *e )
{
  QDialog::showEvent( e );
  emit widgetOpened();
}




#include "dialog.moc"
