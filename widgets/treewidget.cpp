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
#include <qlistview.h>

/* OTHER INCLUDES */
#include <specials.h>
#include "treewidget.h"


TreeWidget::TreeWidget(QWidget *a_parent, const char *a_name)
  : KListView(a_parent, a_name), KommanderWidget(this)
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
  QStringList elements = QStringList::split(m_pathSeparator, s);
  if (elements.count() > 1)
    setRootIsDecorated(true);
  QListViewItem* parent = 0;
  if (m_lastPath.size() < elements.count())
    m_lastPath.resize(elements.count());
  for (uint i=0; i<elements.count(); i++)
  {
    if (m_lastPath[i] && m_lastPath[i]->text(0) == elements[i])
    {
      parent = m_lastPath[i];
      continue;
    }
    else 
    {
      QListViewItem* item = (i>0) ? parent->firstChild() : firstChild();
      while (item)
      {
        if (item->text(0) == elements[i])
          break;
        item = item->nextSibling(); 
      }
      if (item)
        parent = item;
      else 
        parent = itemFromString(parent, elements[i]);
      m_lastPath.insert(i, parent); 
    }
    
  }
}

QListViewItem* TreeWidget::itemFromString(QListViewItem* parent, const QString& s) 
{
  QStringList elements;
  if (s.contains("\t"))
    elements = QStringList::split("\t", s);
  else
    elements = QStringList::split("\\t", s);
  int cols = elements.count();
  if (cols >= columns())
    cols = columns();
  QListViewItem* item;
  if (parent)
    item = new QListViewItem(parent);
  else
    item = new QListViewItem(this);
  for (int i=0; i<cols; i++)
    item->setText(i, elements[i]);
  return item;
}

int TreeWidget::itemToIndex(QListViewItem* item) 
{
  QListViewItemIterator it(this);
  int index = 0;
  while (it.current()) {
    if (it.current() == item)
      return index;
    ++it;
    ++index;
  }
  return -1;
}

QListViewItem* TreeWidget::indexToItem(int item) 
{
  QListViewItemIterator it(this);
  int index = 0;
  while (it.current()) {
    if (index == item)
      return it.current();
    ++it;
    ++index;
  }
  return 0;
}
  
QString TreeWidget::itemText(QListViewItem* item)
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
  QListViewItemIterator it(this);
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

QString TreeWidget::itemPath(QListViewItem* item)
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
  return TRUE;
}

void TreeWidget::setCurrentItem(QListViewItem* item)
{
  KListView::setCurrentItem(item);
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
  handleDCOP(DCOP::setText, a_text);
  emit widgetTextChanged(a_text);
}

void TreeWidget::showEvent( QShowEvent *e )
{
    QListView::showEvent( e );
    emit widgetOpened();
}

QString TreeWidget::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::insertItem:
      addItemFromString(args[0]);
      break;
    case DCOP::text:
      return itemsText();
    case DCOP::setText:
      clear();       /* break omitted: setText is equivalent to clear and insertItems */
    case DCOP::insertItems:
    {
      QStringList items(QStringList::split("\n", args[0]));
      for (uint i=0; i<items.count(); i++)
        addItemFromString(items[i]);
      break;
    }
    case DCOP::selection:
      return itemText(currentItem());
    case DCOP::setSelection:
      setCurrentItem(findItem(args[0], 0));
      break;
    case DCOP::clear:
      clear();
      m_lastPath.clear();
      break;
    case DCOP::removeItem:
      delete indexToItem(args[0].toInt());
      m_lastPath.clear();
      break;
    case DCOP::currentItem:
      return QString::number(itemToIndex(currentItem()));
    case DCOP::setCurrentItem:
      setCurrentItem(indexToItem(args[0].toInt()));
      break;
    case DCOP::findItem:
      return QString::number(itemToIndex(findItem(args[0], 0)));
    case DCOP::item:
      return itemText(indexToItem(args[0].toInt()));
    case DCOP::itemPath:
      return itemPath(indexToItem(args[0].toInt()));
    case DCOP::itemDepth:
    {
      QListViewItem* item = indexToItem(args[0].toInt());
      return (item) ? QString::number(item->depth()) : QString::number(-1);
    }
    case DCOP::setPixmap:
    {
      QPixmap pixmap = KGlobal::iconLoader()->loadIcon(args[0], KIcon::Small);
      if (args[1].toInt() == -1)
        for (QListViewItemIterator it(this); it.current(); ++it)
          it.current()->setPixmap(0, pixmap);
      else 
      { 
        QListViewItem* item = indexToItem(args[1].toInt());
        if (item)
          item->setPixmap(0, pixmap);
      }
      break;
    }
    default:
      break;
  }
  return QString::null;
}

#include "treewidget.moc"
