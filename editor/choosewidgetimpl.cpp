 /***************************************************************************
                          choosewidgetimpl.cpp  - dialog to choose widget
                             -------------------
    begin                :  Thu 13 Apr 2004
    copyright            : (C) 2000 Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

 /** KDE INCLUDES */ 
#include <klistview.h>
#include <klocale.h>

/** QT INCLUDES */
#include <qptrstack.h>
#include <qobject.h>
#include <qobjectlist.h>
#include <qlineedit.h>
#include "qmetaobject.h"

/** OTHER INCLUDES */
#include "choosewidgetimpl.h"
#include "choosewidgetimpl.moc"
 
ChooseWidget::ChooseWidget(QWidget* a_parent, const char* a_name, bool a_modal)
   : ChooseWidgetBase(a_parent, a_name, a_modal)
{
  connect( nameEdit, SIGNAL(textChanged(const QString&)), SLOT(textChanged(const QString&)) );
  connect( widgetView, SIGNAL(executed(QListViewItem*)), SLOT(selectedItem(QListViewItem*)));
  widgetView->setFullWidth(true);
  widgetView->addColumn(i18n("Widgets"));
  widgetView->setRootIsDecorated(true);
  nameEdit->setFocus();
}

ChooseWidget::~ChooseWidget()
{
  
}

void ChooseWidget::setWidget(QWidget* w)
{
  widgetView->clear();
  if (!w)
    return;
  
  QListViewItem* item;
  QPtrStack<QWidget> p_widgets;
  QPtrStack<QListViewItem> p_items;
  
  item = new QListViewItem(widgetView, QString("%1 (%2)").arg(w->name()).arg(w->className()));
  item->setOpen(true);
  
  p_widgets.push(w);
  p_items.push(item);
  
  while (!p_widgets.isEmpty()) {
    w = p_widgets.pop();
    item = p_items.pop();
    QObjectList *objectList = w->queryList(0, 0, true, false);
    for (QObjectListIt it(*objectList); it.current(); ++it) {
      QListViewItem* newItem = item;
      if (isKommanderWidget(*it)) 
        newItem = new QListViewItem(item, QString("%1 (%2)").arg((*it)->name()).arg((*it)->className()));
      if ((*it)->children()) {
          p_widgets.push((QWidget*)(*it));
          p_items.push(newItem);
      }
    }
    delete objectList;
  }
  if (widgetView->childCount()) {
    widgetView->setCurrentItem(widgetView->firstChild());
    widgetView->firstChild()->setSelected(true);
  }
}


QString ChooseWidget::selection()
{
   if (widgetView->currentItem())
     return widgetView->currentItem()->text(0);
   else
     return QString::null;
}

void ChooseWidget::textChanged(const QString& text)
{
  QListViewItem* item = widgetView->findItem(text, 0, Qt::BeginsWith);
  if (item) {
    widgetView->setCurrentItem(item);
    widgetView->ensureItemVisible(item);
  }
}

bool ChooseWidget::isKommanderWidget(QObject* w)
{
  bool pExists = false;
  QMetaObject *metaObj = w->metaObject();
  if (metaObj)
  {
    int id = metaObj->findProperty("KommanderWidget", true);
    const QMetaProperty *metaProp = metaObj->property(id, true);
    if (metaProp && metaProp->isValid())
      pExists = true;
  }
  if (pExists)
  {
    QVariant flag = (w)->property("KommanderWidget");
    if(flag.isValid() && !(QString(w->name()).startsWith("qt_"))) 
      return true;
  }
  return false;
}

void ChooseWidget::selectedItem(QListViewItem* item)
{
  if (item)
    accept();
}

