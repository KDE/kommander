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
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include "specials.h"
#include "dialog.h"
#include "myprocess.h"

Dialog::Dialog(QWidget *a_parent, const char *a_name, bool a_modal, int a_flags)
  : QDialog(a_parent), KommanderWindow(this)
{
  Q_UNUSED(a_flags);
  setObjectName(a_name);
  setModal(a_modal);
  QStringList states;
  states << "default";
  states << "initialization";
  states << "destroy";
  setStates(states);
  setDisplayStates(states);
  m_useShebang = false;
  m_shebang = "#!/usr/bin/kmdr-executor";
  m_firstShow = true;
}

Dialog::~Dialog()
{
  if (!inEditor)
    destroy();
}

QString Dialog::currentState() const
{
  return QString("default");
}

bool Dialog::isKommanderWidget() const
{
  return true;
}

bool Dialog::useInternalParser() const
{
  return m_useInternalParser; 
}

void Dialog::setUseInternalParser(bool b) 
{
  KommanderWidget::useInternalParser = b; 
  m_useInternalParser = b;
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
  setWidgetText(KommanderWidget::evalAssociatedText(populationText()));
}

void Dialog::initialize()
{    
  const QStringList assoc = associatedText();
  if (assoc.count() > 1 && !assoc[1].isEmpty()) 
  {
    QString evalText = KommanderWidget::evalAssociatedText(assoc[1]);
    if (!evalText.isEmpty())
    {
      MyProcess proc(this);
      proc.run(evalText);
    }
  }
}

void Dialog::destroy()
{    
  const QStringList assoc = associatedText();
  if (assoc.count() > 2 && !assoc[2].isEmpty()) 
  {
    QString evalText = KommanderWidget::evalAssociatedText(assoc[2]);
    if (!evalText.isEmpty())
    {
      MyProcess proc(this);
      proc.run(evalText);
    }
  }
}


void Dialog::setWidgetText(const QString& a_text)
{
  setWindowTitle(a_text);
  emit widgetTextChanged(a_text);
}

void Dialog::exec()
{
  QDialog::exec();
  emit finished();
}

void Dialog::setVisible(bool visible)
{
  //avoid initialization on every show/hide
  if (!m_firstShow) 
  {
    QDialog::setVisible(visible);
  } else
  {
    m_firstShow = false;
    QDialog::setVisible(visible);
    if (!inEditor)
      initialize();
  }
}

void Dialog::done(int r)
{
/*  if (!inEditor)
    destroy();*/
  QDialog::done(r);  
}

void Dialog::showEvent(QShowEvent *e)
{
  QDialog::showEvent( e );
  emit widgetOpened();
}

void Dialog::keyPressEvent( QKeyEvent *e )
{
  if ( e->state() == 0 && e->key() == Qt::Key_Escape)
	return;
  else
   QDialog::keyPressEvent(e);
  
}

void Dialog::contextMenuEvent( QContextMenuEvent * e )
{
  QDialog::contextMenuEvent( e );
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

bool Dialog::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::setText;
}

QString Dialog::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
      return windowTitle();
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}


#include "dialog.moc"
