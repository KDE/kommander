/***************************************************************************
                         listbox.cpp - Listbox widget 
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
#include <kglobal.h>
#include <kiconloader.h>
#include <kicon.h>

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
//Added by qt3to4:
#include <QPixmap>
#include <QShowEvent>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "listbox.h"

ListBox::ListBox(QWidget *a_parent, const char *a_name)
  : Q3ListBox(a_parent), KommanderWidget(this)
{
  setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
}

ListBox::~ListBox()
{
}

QString ListBox::currentState() const
{
  return QString("default");
}

bool ListBox::isKommanderWidget() const
{
  return true;
}

QStringList ListBox::associatedText() const
{
  return KommanderWidget::associatedText();
}

void ListBox::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void ListBox::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString ListBox::populationText() const
{
  return KommanderWidget::populationText();
}

void ListBox::populate()
{
  QString txt = KommanderWidget::evalAssociatedText(populationText());
  setWidgetText(txt);
}

void ListBox::setWidgetText(const QString& a_text)
{
  clear();
  insertStringList( a_text.split('\n'));
  emit widgetTextChanged(a_text);
}

void ListBox::showEvent(QShowEvent *e)
{
  Q3ListBox::showEvent(e);
  emit widgetOpened();
}

bool ListBox::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::setText || f == DBUS::selection || f == DBUS::setSelection ||
      f == DBUS::insertItems || f == DBUS::insertItem || f == DBUS::removeItem || f == DBUS::clear ||
      f == DBUS::currentItem || f == DBUS::setCurrentItem || f == DBUS::item || f == DBUS::addUniqueItem ||
      f == DBUS::findItem || f == DBUS::setPixmap || f == DBUS::count;
}

void ListBox::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}


QString ListBox::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    case DBUS::selection:
    {
      if (selectionMode() == Single)
        return currentText();
      QString value;
      for (uint i=0; i<count(); i++)
        if (isSelected(i)) 
          value += (value.length() ? "\n" : "") + item(i)->text();
      return value;
    }
    case DBUS::setSelection:
    {
      Q3ListBoxItem* found = findItem(args[0]);
      if (found)
        setCurrentItem(index(found));
      break;
    }
    case DBUS::insertItems:
      insertStringList(args[0].split('\n'), args[1].toInt());
      break;
    case DBUS::insertItem:
      insertItem(args[0], args[1].toInt());
      break;
    case DBUS::removeItem:
      removeItem(args[0].toInt());
      break;
    case DBUS::clear:
      clear();
      break;
    case DBUS::count:
      return QString::number(count());
    case DBUS::currentItem:
      return QString::number(currentItem());
    case DBUS::setCurrentItem:
    {
      int index = args[0].toInt();
      if (index < (int)count())
        setCurrentItem(index);
      break;
    }
    case DBUS::item:
    {
      int index = args[0].toInt();
      if (index >= 0 && index < (int)count())
        return item(index)->text();
      else
        return QString();
    }
    case DBUS::addUniqueItem:
      if (!findItem(args[0]))
        insertItem(args[0]);
      break;
    case DBUS::findItem:
    {
      Q3ListBoxItem * found = findItem(args[0]); // exact match
      if (!found) found = findItem(args[0]); // startswith FIXME
      if (!found) found = findItem(args[0]); //contains FIXME
      if (found)
        return QString::number(index(found));
      else return QString::number(-1);
      break;
    }
    case DBUS::setPixmap:
    {
      /*
      QPixmap pixmap = KIconLoader::global()->loadIcon(args[0]); // FIXME, KIcon::Small);
      if (pixmap.isNull())
        pixmap.load(args[0]);
      int index = args[1].toInt();
      if (index == -1)
      {
        for (int i=0; i<count(); i++)
          changeItem(pixmap, text(i), i);
      }
      else if (index < (int)count())
        changeItem(pixmap, text(index), index);*/
      break;
    }
    case DBUS::text:
    {
      QStringList strings;
      for(uint i=0; i < count() ; ++i)
        strings += item(i)->text();
      return strings.join("\n");
    }
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}

#include "listbox.moc"
