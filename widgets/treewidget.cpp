/***************************************************************************
			treewidget.cpp - A tree widget
                             -------------------
		begin			: 03/08/2003
		copyright		: (C) Marc Britton
		email			: consume@optusnet.com.au
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
}

TreeWidget::~TreeWidget()
{
}

void TreeWidget::addItemFromString(const QString& s)
{
  QStringList elements = QStringList::split("/", s);
  QListViewItem* parent = 0;
  for (uint i=0; i<elements.count(); i++)
  {
//FIXME: don't find first only     
     QListViewItem* current = findItem(elements[i], 0);
     if (!current || current->depth() != (int)i)
       current  = itemFromString(parent, elements[i]);
     parent = current;
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



QString TreeWidget::currentState() const
{
  return QString("default");
}

bool TreeWidget::isKommanderWidget() const
{
  return TRUE;
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
  QString txt = KommanderWidget::evalAssociatedText( populationText() );
//FIXME: implement me
}

void TreeWidget::setWidgetText(const QString &a_text)
{
//FIXME: implement  
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
    case DCOP::selection:
      if (currentItem())
        return currentItem()->text(0);
      break;
    case DCOP::setSelection:
    {
      QListViewItem* item = findItem(args[0], 0);
      if (item)
        setCurrentItem(item);
      break;
    }
    case DCOP::clear:
      clear();
      break;
    default:
      break;
  }
  return QString::null;
}

#include "treewidget.moc"
