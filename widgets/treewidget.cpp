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
#include "kommanderplugin.h"
#include "specials.h"

enum Functions {
  FirstFunction = 189,
  SelectedIndexes,
  LastFunction
};

TreeWidget::TreeWidget(QWidget *a_parent, const char *a_name)
  : KListView(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  setPathSeparator("/");
  KommanderPlugin::setDefaultGroup(Group::DCOP);
  KommanderPlugin::registerFunction(SelectedIndexes, "selectedIndexes(QString widget)",  "", 1);
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
  QListViewItem* parent = 0;
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
      QListViewItem* item = (i>0) ? parent->firstChild() : firstChild();
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

QListViewItem* TreeWidget::itemFromString(QListViewItem* parent, const QString& s)
{
  QStringList elements;
  if (s.contains("\t"))
    elements = QStringList::split("\t", s, true);
  else
    elements = QStringList::split("\\t", s, true);
  int cols = elements.count();
  if (cols >= columns())
    cols = columns();
  QListViewItem* item;
  if (parent)
    item = new QListViewItem(parent);
  else
    item = new QListViewItem(this);
  int i = 0;
  for (QStringList::ConstIterator it = elements.constBegin(); it != elements.constEnd(); ++it) 
    item->setText(i++, *it);
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

QString TreeWidget::itemText(QListViewItem* item) const
{
  if (!item)
    return QString();
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

QString TreeWidget::itemPath(QListViewItem* item) const
{
  if (!item)
    return QString();
  item = item->parent();
  if (!item)
    return QString();
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

void TreeWidget::showEvent(QShowEvent* e)
{
    QListView::showEvent( e );
    emit widgetOpened();
}

void TreeWidget::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

bool TreeWidget::isFunctionSupported(int f)
{
  return f == DCOP::insertItem || f == DCOP::text || f == DCOP::setText || f == DCOP::insertItems ||
    f == DCOP::selection || f == DCOP::setSelection || f == DCOP::clear || f == DCOP::removeItem || 
    f == DCOP::currentItem || f == DCOP::setCurrentItem || f == DCOP::findItem || f == DCOP::item || 
      f == DCOP::itemPath || f == DCOP::itemDepth || f == DCOP::setPixmap || f == DCOP::setColumnCaption || (f > FirstFunction && f < LastFunction);
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
      m_lastPath.clear();
    case DCOP::insertItems:
    {
      QStringList items(QStringList::split("\n", args[0], true));
      for (QStringList::ConstIterator it = items.constBegin(); it != items.constEnd(); ++it) 
        addItemFromString(*it);
      break;
    }
    case SelectedIndexes:
    {
      QString selection = "";
      QListViewItemIterator it(this);
      while (it.current()) 
      {
        if (it.current()->isSelected())
        {        
          selection.append(QString("%1\n").arg(itemToIndex(it.current())));
        }
        ++it;
      }
      if (!selection.isEmpty())
        selection = selection.left(selection.length() - 1);
      return selection;
      break;
    }
    case DCOP::selection:
    {
      QString selection = "";
      QListViewItemIterator it(this);
      while (it.current()) 
      {
        if (it.current()->isSelected())
            selection.append(itemText(it.current()) + "\n");
        ++it;
      }
      if (!selection.isEmpty())
        selection = selection.left(selection.length() - 1);
      return selection;
      break;
    }
    case DCOP::setSelection:
      if (selectionModeExt() == Single || selectionModeExt() == NoSelection)
        setCurrentItem(findItem(args[0], 0));
      else
      {
        clearSelection();
        QStringList items(QStringList::split("\n", args[0]));
        for (QStringList::ConstIterator it = items.begin(); it != items.end(); ++it)
        {
          QListViewItem* item = findItem(*it, 0);
          if (item)
          {
            item->setSelected(true);
            ensureItemVisible(item);
          }
        }
      }
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
    case DCOP::setColumnCaption:
      setColumnText(args[0].toInt(), args[1]);
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}

#include "treewidget.moc"
