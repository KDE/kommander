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

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "listbox.h"

ListBox::ListBox(QWidget *a_parent, const char *a_name)
  : KListBox(a_parent, a_name), KommanderWidget(this)
{
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
  insertStringList(QStringList::split("\n", a_text));
  emit widgetTextChanged(a_text);
}

void ListBox::showEvent(QShowEvent *e)
{
  QListBox::showEvent(e);
  emit widgetOpened();
}

bool ListBox::isFunctionSupported(int f)
{
  return f == DCOP::text || f == DCOP::setText || f == DCOP::selection || f == DCOP::setSelection ||
    f == DCOP::insertItems || f == DCOP::insertItem || f == DCOP::removeItem || f == DCOP::clear ||
    f == DCOP::currentItem || f == DCOP::setCurrentItem || f == DCOP::item || f == DCOP::addUniqueItem ||
      f == DCOP::findItem || f == DCOP::setPixmap || f == DCOP::count;
}


QString ListBox::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    case DCOP::selection:
    {
      if (selectionMode() == Single)
        return currentText();
      QString value;
      for (uint i=0; i<count(); i++)
        if (isSelected(i)) 
          value += (value.length() ? "\n" : "") + item(i)->text();
      return value;
    }
    case DCOP::setSelection:
    {
      QListBoxItem* found = findItem(args[0], Qt::ExactMatch);
      if (found)
        setCurrentItem(index(found));
      break;
    }
    case DCOP::insertItems:
      insertStringList(QStringList::split("\n", args[0]), args[1].toInt());
      break;
    case DCOP::insertItem:
      insertItem(args[0], args[1].toInt());
      break;
    case DCOP::removeItem:
      removeItem(args[0].toInt());
      break;
    case DCOP::clear:
      clear();
      break;
    case DCOP::count:
      return QString::number(count());
    case DCOP::currentItem:
      return QString::number(currentItem());
    case DCOP::setCurrentItem:
    {
      int index = args[0].toInt();
      if (index < (int)count())
        setCurrentItem(index);
      break;
    }
    case DCOP::item:
    {
      int index = args[0].toInt();
      if (index >= 0 && index < (int)count())
        return item(index)->text();
      else
        return QString::null;
    }
    case DCOP::addUniqueItem:
      if (!findItem(args[0], Qt::ExactMatch))
        insertItem(args[0]);
      break;
    case DCOP::findItem:
    {
      QListBoxItem* found = findItem(args[0], Qt::ExactMatch);
      if (!found) found = findItem(args[0], Qt::BeginsWith);
      if (!found) found = findItem(args[0], Qt::Contains);
      if (found)
        return QString::number(index(found));
      else return QString::number(-1);
      break;
    }
    case DCOP::setPixmap:
    {
      QPixmap pixmap = KGlobal::iconLoader()->loadIcon(args[0], KIcon::Small);
      if (pixmap.isNull())
        pixmap.load(args[0]);
      int index = args[1].toInt();
      if (index == -1)
      {
        for (uint i=0; i<count(); i++)
          changeItem(pixmap, text(i), i);
      }
      else if (index < (int)count())
        changeItem(pixmap, text(index), index);
      break;
    }
    case DCOP::text:
    {
      QStringList strings;
      for(uint i=0; i < count() ; ++i)
        strings += item(i)->text();
      return strings.join("\n");
    }
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString::null;
}

#include "listbox.moc"
