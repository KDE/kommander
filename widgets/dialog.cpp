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
#include <klocale.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qdialog.h>
#include <qpoint.h>
#include <qcursor.h>
#include <qapplication.h>

/* OTHER INCLUDES */
#include <specials.h>
#include "dialog.h"
#include <myprocess.h>
#include "kommanderplugin.h"

enum Functions {
  FirstFunction = 185,
  D_focusWidget,
  D_waitCursor,
  D_restoreCursor,
  LastFunction
};

Dialog::Dialog(QWidget *a_parent, const char *a_name, bool a_modal, int a_flags)
  : QDialog(a_parent, a_name, a_modal, a_flags), KommanderWindow(this)
{
  QStringList states;
  states << "default";
  states << "initialization";
  states << "destroy";
  setStates(states);
  setDisplayStates(states);
  m_useShebang = false;
  m_shebang = "#!/usr/bin/kmdr-executor";
  m_firstShow = true;
  KommanderPlugin::setDefaultGroup(Group::DCOP);
  KommanderPlugin::registerFunction(D_focusWidget, "focusWidget(QString widget)",  i18n("The name of the widget having focus"), 1);
  KommanderPlugin::registerFunction(D_waitCursor, "waitCursor(QString widget)",  i18n("Set a wait cursor. CAUTION: if set more than once an equal number of calls to restore must be made to clear it."), 1);
  KommanderPlugin::registerFunction(D_restoreCursor, "restoreCursor(QString widget)",  i18n("Restore normal curser. NOTE: must be called as many times as wait was."), 1);
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

void Dialog::exec()
{
  QDialog::exec();
  emit finished();
}

void Dialog::show()
{
//if the dialog is embedded in a KPart, the show can be called many times.
//to avoid re-init and sending signals we don't want, in that case call only the
//QWidget's show method to show the widgets, but don't do any QDialog specific 
//task
  if (!m_firstShow) 
  {
    QWidget::show();
  } else
  {
    QDialog::show();
    if (!inEditor)
      initialize();
    m_firstShow = false;
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
  if ( e->state() == 0 && e->key() == Key_Escape)
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
  return f == DCOP::text || f == DCOP::setText || f == DCOP::geometry || (f > FirstFunction && f < LastFunction);
}

QString Dialog::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::text:
      return caption();
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    case DCOP::geometry:
      return QString::number(this->x())+" "+QString::number(this->y())+" "+QString::number(this->width())+" "+QString::number(this->height());
      break;
    case D_focusWidget:
      return focusWidget()->name();
      break;
    case D_waitCursor:
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      break;
    case D_restoreCursor:
      QApplication::restoreOverrideCursor();
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}


#include "dialog.moc"
