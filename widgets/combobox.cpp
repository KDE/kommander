/***************************************************************************
                         combobox.cpp - Combobox widget 
                             -------------------
    copyright            : (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
                           (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
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
//Added by qt3to4:
#include <QShowEvent>

#include <klocale.h>

/* OTHER INCLUDES */
#include "kommanderplugin.h"
#include "specials.h"
#include "combobox.h"

enum Functions {
  FirstFunction = 355, //CHANGE THIS NUMBER TO AN UNIQUE ONE!!!
  popupList,
  LastFunction
};


ComboBox::ComboBox(QWidget *a_parent, const char *a_name)
  : KComboBox( a_parent ), KommanderWidget(this)
{
  this->setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);

  connect(this, SIGNAL(activated(int)), this, SLOT(emitWidgetTextChanged(int)));

  KommanderPlugin::setDefaultGroup(Group::DBUS);
  KommanderPlugin::registerFunction(popupList, "popupList(QString widget)",  i18n("Make the ComboBox expose it's list without mousing around."), 1);

}

ComboBox::~ComboBox()
{
}

QString ComboBox::currentState() const
{
  return QString("default");
}

bool ComboBox::isKommanderWidget() const
{
  return true;
}

QStringList ComboBox::associatedText() const
{
  return KommanderWidget::associatedText();
}

void ComboBox::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void ComboBox::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString ComboBox::populationText() const
{
  return KommanderWidget::populationText();
}

void ComboBox::populate()
{
  setWidgetText(KommanderWidget::evalAssociatedText( populationText()));
}

void ComboBox::setWidgetText(const QString& a_text)
{
  clear();
  insertItems(-1, a_text.split('\n'));
  emit widgetTextChanged(a_text);
}

void ComboBox::emitWidgetTextChanged(int a_index)
{
  emit widgetTextChanged(itemText(a_index));
}

void ComboBox::showEvent(QShowEvent *e)
{
    QComboBox::showEvent( e );
    emit widgetOpened();
}

void ComboBox::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();  
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}


bool ComboBox::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::selection || f == DBUS::setSelection ||
      f == DBUS::currentItem || f == DBUS::setCurrentItem || f == DBUS::item || 
      f == DBUS::removeItem || f == DBUS::insertItem || f == DBUS::insertItems ||
      f == DBUS::addUniqueItem || f == DBUS::clear || f == DBUS::count || f == DBUS::setEditable ||
      f == DBUS::geometry || f == DBUS::hasFocus || (f >= FirstFunction && f <= LastFunction);
}

QString ComboBox::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
      return currentText();
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    case DBUS::selection:
      return currentText();
    case DBUS::currentItem:
      return QString::number(currentIndex());
    case DBUS::setCurrentItem:
      setCurrentItem(args[0]);
      break;
    case DBUS::item:
    {
      int i = args[0].toInt();
      if (i >= 0 && i < count()) 
        return itemText(i);
      break;
    }
    case DBUS::removeItem:
      removeItem(args[0].toInt());
      break;
    case DBUS::insertItem:
      insertItem(args[1].toInt(), args[0]);
      break;
    case DBUS::insertItems:
      insertItems(args[1].toInt(), args[0].split('\n'));
      break;
    case DBUS::addUniqueItem:
      for (int i=0; i<count(); i++)
        if (itemText(i) == args[0])
          return QString();
      insertItem(-1, args[0]);
      break;
    case DBUS::clear:
      clear();
      break;
    case DBUS::count:
      return QString::number(count());
    case DBUS::setSelection:
    {
      for (int i = 0; i < count(); i++)
        if (itemText(i) == args[0])
        {
          setCurrentItem(QString(i));
          break;
        }
      break;
    }
    case DBUS::setEditable:
      setEditable(args[0] != "false" && args[0] != "0");
      break;
    case popupList:
      QComboBox::popup();
      break;
    case DBUS::geometry:
      return QString::number(this->x())+" "+QString::number(this->y())+" "+QString::number(this->width())+" "+QString::number(this->height());
      break;
    case DBUS::hasFocus:
      return QString::number(this->hasFocus());
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}

#include "combobox.moc"
