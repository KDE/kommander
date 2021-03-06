/***************************************************************************
                         groupbox.cpp - Groupbox widget 
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
#include <qobject.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <specials.h>
#include "groupbox.h"

GroupBox::GroupBox(QWidget *a_parent, const char *a_name)
  : QWidget(a_parent), KommanderWidget(this)
{
  setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
}

GroupBox::~GroupBox()
{
}

QString GroupBox::currentState() const
{
	return QString("default");
}

bool GroupBox::isKommanderWidget() const
{
	return true;
}

QStringList GroupBox::associatedText() const
{
	return KommanderWidget::associatedText();
}

void GroupBox::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void GroupBox::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString GroupBox::populationText() const
{
    return KommanderWidget::populationText();
}

void GroupBox::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
//FIXME:    setWidgetText( txt );
}

/* We need to keep track of all widgets created as a child of a groupbox, in order of creation. */
void GroupBox::insertChild(QObject *a_child)
{
  m_childList.append(a_child);
  a_child->setParent(this);
}
 
void GroupBox::removeChild(QObject *a_child)
{
  m_childList.removeAll(a_child);
  a_child->setParent(0L);
}

void GroupBox::showEvent( QShowEvent *e )
{
  QWidget::showEvent(e);
  emit widgetOpened();
}

void GroupBox::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

bool GroupBox::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::setText;
}

QString GroupBox::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
    {
      QString text;
      QListIterator<QObject *> it(m_childList);
      while( it.hasNext())
      {
        QObject *a = it.next(); 
        if (dynamic_cast<KommanderWidget*>(a))
          text += (dynamic_cast<KommanderWidget*>(a))->evalAssociatedText();
      }
      return text;
    }
    case DBUS::setText:
      //setTitle(args[0]);
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
  
}

#include "groupbox.moc"
