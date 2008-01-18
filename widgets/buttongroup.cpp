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

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qbuttongroup.h>
#include <qevent.h>

/* OTHER INCLUDES */
#include <specials.h>
#include "buttongroup.h"

#include "radiobutton.h" // include a button header for the compiler with dynamic cast below

ButtonGroup::ButtonGroup(QWidget *a_parent, const char *a_name)
	: QButtonGroup(a_parent, a_name), KommanderWidget(this)
{
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
  QButtonGroup::showEvent(e);
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
  return f == DCOP::text || f == DCOP::checked || f == DCOP::setChecked;
}
    

QString ButtonGroup::handleDCOP(int function, const QStringList& args) 
{
  switch (function) {
    case DCOP::text:
    {
      QString text;
      for (int i = 0; i < count(); i++)
        if (dynamic_cast<KommanderWidget*>(find(i)))
          text += (dynamic_cast<KommanderWidget*>(find(i)))->evalAssociatedText();
      return text;
    }
    case DCOP::checked:
      return QString::number(isChecked());
    case DCOP::setChecked:
      setCheckable(true);
      setChecked(args[0] != "false");
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}


#include "buttongroup.moc"
