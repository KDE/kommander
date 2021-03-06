/***************************************************************************
                          buttongroup.cpp - Button group widget 
                             -------------------
    copyright            : (C) 2002-2004 by Marc Britton
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
#include <kbuttongroup.h>

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <specials.h>
#include "buttongroup.h"
#include "radiobutton.h" // include a button header for the compiler with dynamic cast below

ButtonGroup::ButtonGroup(QWidget *a_parent, const char *a_name)
	: KButtonGroup(a_parent), KommanderWidget(this)
{
  this->setObjectName(a_name);
  QStringList states;
  states << "checked";
  states << "unchecked";
  setStates(states);
  setDisplayStates(states);
}

ButtonGroup::~ButtonGroup()
{
}

QString ButtonGroup::currentState() const
{
  if (!isCheckable() || isChecked())
    return "checked";
  else
    return "unchecked";
}

bool ButtonGroup::isKommanderWidget() const
{
  return true;
}

QStringList ButtonGroup::associatedText() const
{
  return KommanderWidget::associatedText();
}

void ButtonGroup::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void ButtonGroup::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText( a_text );
}

QString ButtonGroup::populationText() const
{
  return KommanderWidget::populationText();
}

void ButtonGroup::populate()
{
//FIXME: implement
}

void ButtonGroup::showEvent(QShowEvent* e)
{
  KButtonGroup::showEvent(e);
  emit widgetOpened();
}

void ButtonGroup::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();  
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

bool ButtonGroup::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::checked || f == DBUS::setChecked;
}
    

QString ButtonGroup::handleDBUS(int function, const QStringList& args) 
{
  switch (function) {
    case DBUS::text:
    {
      QString text;
      QList<QObject *> children = findChildren<QObject *>();
      foreach (QObject *child, children) 
      {
        if (dynamic_cast<KommanderWidget*>(child))
          text += (dynamic_cast<KommanderWidget*>(child))->evalAssociatedText();
      }
      return text;
    }
    case DBUS::checked:
      return QString::number(isChecked());
    case DBUS::setChecked:
      setCheckable(true);
      setChecked(args[0] != "false");
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}


#include "buttongroup.moc"
