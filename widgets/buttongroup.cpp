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
#include <klocale.h>

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qbuttongroup.h>
#include <qevent.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "kommanderplugin.h"
#include <specials.h>
#include "buttongroup.h"

#include "radiobutton.h" // include a button header for the compiler with dynamic cast below

enum Functions {
  FirstFunction = 720,
  BG_selectedId,
  LastFunction
};

ButtonGroup::ButtonGroup(QWidget *a_parent, const char *a_name)
	: QButtonGroup(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "checked";
  states << "unchecked";
  setStates(states);
  setDisplayStates(states);
  
  KommanderPlugin::setDefaultGroup(Group::DCOP);
  KommanderPlugin::registerFunction(BG_selectedId, "selectedId(QString widget)", i18n("Returns the ID of the selected button."), 1);

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
  return f == DCOP::text || f == DCOP::checked || f == DCOP::setChecked || f == DCOP::geometry || f == DCOP::getBackgroundColor || f == DCOP::setBackgroundColor || (f >= FirstFunction && f <= LastFunction);
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
    case BG_selectedId:
      return QString::number(this->selectedId() );
      break;
    case DCOP::geometry:
    {
      QString geo = QString::number(this->x())+" "+QString::number(this->y())+" "+QString::number(this->width())+" "+QString::number(this->height());
      return geo;
      break;
    }
    case DCOP::getBackgroundColor:
      return this->paletteBackgroundColor().name();
      break;
    case DCOP::setBackgroundColor:
    {
      QColor color;
      color.setNamedColor(args[0]);
      this->setPaletteBackgroundColor(color);
      break;
    }
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}


#include "buttongroup.moc"
