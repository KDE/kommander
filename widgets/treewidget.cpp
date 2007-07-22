/***************************************************************************
                  treewidget.cpp - Tree/detailed list widget
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
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <q3listview.h>
//Added by qt3to4:
#include <QPixmap>
#include <QShowEvent>

/* OTHER INCLUDES */
#include <specials.h>
#include "treewidget.h"


TreeWidget::TreeWidget(QWidget *a_parent, const char *a_name)
  : K3ListView(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  setPathSeparator("/");
}

TreeWidget::~TreeWidget()
{
}

QString TreeWidget::pathSeparator() const
{
  return m_pathSeparator;  
}

void TreeWidget::setPathSeparator(const QString& a_pathSep)
{
  m_pathSeparator = a_pathSep;
}
  
void TreeWidget::addItemFromString(const QString& s)
{
  QStringList elements = QStringList::split(m_pathSeparator, s, true);
  if (elements.count() > 1)
    setRootIsDecorated(true);
  Q3ListViewItem* parent = 0;
  if (m_lastPath.size() < elements.count())
    m_lastPath.resize(elements.count());
  uint i = 0;
  for (QStringList::ConstIterator it = elements.begin(); it != elements.end(); ++it) 
  {
    if (m_lastPath[i] && m_lastPath[i]->text(0) == elements[i])
    {
      parent = m_lastPath[i];
      i++;
      continue;
    }
    else 
    {
      Q3ListViewItem* item = (i>0) ? parent->firstChild() : firstChild();
      while (item)
      {
        if (item->text(0) == *it)
          break;
        item = item->nextSibling(); 
      }
      if (item)
        parent = item;
      else 
        parent = itemFromString(parent, *it);
      m_lastPath.insert(i, parent); 
      i++;
    }
  }
}

Q3ListViewItem* TreeWidget::itemFromString(Q3ListViewItem* parent, const QString& s) 
{
  QStringList elements;
  if (s.contains("\t"))
    elements = QStringList::split("\t", s, true);
  else
    elements = QStringList::split("\\t", s, true);
  int cols = elements.count();
  if (cols >= columns())
    cols = columns();
  Q3ListViewItem* item;
  if (parent)
    item = new Q3ListViewItem(parent);
  else
    item = new Q3ListViewItem(this);
  
  int i = 0;
  for (QStringList::ConstIterator it = elements.constBegin(); it != elements.constEnd(); ++it) 
    item->setText(i++, *it);
  return item;
}

int TreeWidget::itemToIndex(Q3ListViewItem* item) 
{
  Q3ListViewItemIterator it(this);
  int index = 0;
  while (it.current()) {
    if (it.current() == item)
      return index;
    ++it;
    ++index;
  }
  return -1;
}

Q3ListViewItem* TreeWidget::indexToItem(int item) 
{
  Q3ListViewItemIterator it(this);
  int index = 0;
  while (it.current()) {
    if (index == item)
      return it.current();
    ++it;
    ++index;
  }
  return 0;
}
  
QString TreeWidget::itemText(Q3ListViewItem* item)
{
  if (!item)
    return QString::null;
  QStringList items;
  for (int i=0; i<columns(); i++)
    items.append(item->text(i));
  return items.join("\t");
}
  
QString TreeWidget::itemsText() 
{
  QStringList items;
  Q3ListViewItemIterator it(this);
  while (it.current()) 
  {
    QString path = itemPath(it.current());
    if (path.isEmpty())
      items.append(itemText(it.current()));
    else 
      items.append(QString("%1%2%3").arg(path).arg(m_pathSeparator)
        .arg(itemText(it.current())));
    ++it;
  }
  return items.join("\n");
}

QString TreeWidget::itemPath(Q3ListViewItem* item)
{
  if (!item) 
    return QString::null;
  item = item->parent();
  QStringList path;
  while (item) 
  {
    path.prepend(item->text(0));
    item = item->parent();
  } 
  return path.join(m_pathSeparator);
}

QString TreeWidget::currentState() const
{
  return QString("default");
}

bool TreeWidget::isKommanderWidget() const
{
  return true;
}

void TreeWidget::setCurrentItem(Q3ListViewItem* item)
{
  K3ListView::setCurrentItem(item);
  setSelected(item, true);
  ensureItemVisible(item);
}

QStringList TreeWidget::associatedText() const
{
  return KommanderWidget::associatedText();
}

void TreeWidget::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void TreeWidget::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText( a_text );
}

QString TreeWidget::populationText() const
{
  return KommanderWidget::populationText();
}

void TreeWidget::populate()
{
  setWidgetText(KommanderWidget::evalAssociatedText( populationText()));
}

void TreeWidget::setWidgetText(const QString &a_text)
{
  handleDCOP(DBUS::setText, a_text);
  emit widgetTextChanged(a_text);
}

void TreeWidget::showEvent( QShowEvent *e )
{
    Q3ListView::showEvent( e );
    emit widgetOpened();
}

bool TreeWidget::isFunctionSupported(int f)
{
  return f == DBUS::insertItem || f == DBUS::text || f == DBUS::setText || f == DBUS::insertItems ||
    f == DBUS::selection || f == DBUS::setSelection || f == DBUS::clear || f == DBUS::removeItem || 
    f == DBUS::currentItem || f == DBUS::setCurrentItem || f == DBUS::findItem || f == DBUS::item || 
      f == DBUS::itemPath || f == DBUS::itemDepth || f == DBUS::setPixmap || f == DBUS::setColumnCaption;
}

QString TreeWidget::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::insertItem:
      addItemFromString(args[0]);
      break;
    case DBUS::text:
      return itemsText();
    case DBUS::setText:
      clear();       /* break omitted: setText is equivalent to clear and insertItems */
      m_lastPath.clear();
    case DBUS::insertItems:
    {
      QStringList items(QStringList::split("\n", args[0], true));
      for (QStringList::ConstIterator it = items.constBegin(); it != items.constEnd(); ++it) 
        addItemFromString(*it);
      break;
    }
    case DBUS::selection:
      return itemText(currentItem());
    case DBUS::setSelection:
      setCurrentItem(findItem(args[0], 0));
      break;
    case DBUS::clear:
      clear();
      m_lastPath.clear();
      break;
    case DBUS::removeItem:
      delete indexToItem(args[0].toInt());
      m_lastPath.clear();
      break;
    case DBUS::currentItem:
      return QString::number(itemToIndex(currentItem()));
    case DBUS::setCurrentItem:
      setCurrentItem(indexToItem(args[0].toInt()));
      break;
    case DBUS::findItem:
      return QString::number(itemToIndex(findItem(args[0], 0)));
    case DBUS::item:
      return itemText(indexToItem(args[0].toInt()));
    case DBUS::itemPath:
      return itemPath(indexToItem(args[0].toInt()));
    case DBUS::itemDepth:
    {
      Q3ListViewItem* item = indexToItem(args[0].toInt());
      return (item) ? QString::number(item->depth()) : QString::number(-1);
    }
    case DBUS::setPixmap:
    {
      QPixmap pixmap = KIconLoader::global()->loadIcon(args[0], KIcon::Small);
      if (args[1].toInt() == -1)
        for (Q3ListViewItemIterator it(this); it.current(); ++it)
          it.current()->setPixmap(0, pixmap);
      else 
      { 
        Q3ListViewItem* item = indexToItem(args[1].toInt());
        if (item)
          item->setPixmap(0, pixmap);
      }
      break;
    }
    case DBUS::setColumnCaption:
      setColumnText(args[0].toInt(), args[1]);
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString::null;
}

#include "treewidget.moc"
