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
#include <qlistbox.h>

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
  return TRUE;
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

QString ListBox::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    case DCOP::selection:
      return currentText();
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
    case DCOP::currentItem:
      return QString::number(currentItem());
    case DCOP::setCurrentItem:
      if (args[0].toUInt() < count())
        setCurrentItem(args[0].toUInt());
      break;
    case DCOP::item:
      return (args[0].toUInt() < count()) ? item(args[0].toInt())->text() : QString::null;
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
      break;
    }
    case DCOP::setPixmap:
    {
      QPixmap pixmap = KGlobal::iconLoader()->loadIcon(args[0], KIcon::Small);
      uint index = args[1].toUInt();
      if (index == -1)
      {
        for (int i=0; i<count(); i++)
          changeItem(pixmap, text(i), i);
      }
      else if (index < count())
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
      break;
  }
  return QString::null;
}

#include "listbox.moc"
