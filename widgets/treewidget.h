/***************************************************************************
                  treewidget.h - Tree/detailed list widget
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

#ifndef _HAVE_TREEWIDGET_H_
#define _HAVE_TREEWIDGET_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qstring.h>
#include <qstringlist.h>
#include <qvector.h>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <Q3ListView>

//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "kommanderwidgets_export.h"

class QWidget;

class QShowEvent;
class KOMMANDERWIDGETS_EXPORT TreeWidget : public Q3ListView, public KommanderWidget
{
  Q_OBJECT
  
  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
  Q_PROPERTY(QString pathSeparator READ pathSeparator WRITE setPathSeparator)
  
public:
  TreeWidget(QWidget *a_parent, const char *a_name);
  ~TreeWidget();
  virtual void setCurrentItem(Q3ListViewItem* item);  
  using Q3ListView::setCurrentItem;
  QString pathSeparator() const;
  void setPathSeparator(const QString& a_pathSep);
  
  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  
  virtual QString handleDBUS(int function, const QStringList& args);
  using KommanderWidget::handleDBUS;
  virtual bool isFunctionSupported(int function);
public slots:
  virtual void setWidgetText(const QString&);
  virtual void populate();
signals:
  void widgetOpened();
  void widgetTextChanged(const QString&);
protected:
  void showEvent(QShowEvent *e);
  int itemToIndex(Q3ListViewItem* item);
  QString itemText(Q3ListViewItem* item);
  QString itemsText();
  Q3ListViewItem* indexToItem(int index);
  QString itemPath(Q3ListViewItem* item);
private:
  void addItemFromString(const QString& s);
  Q3ListViewItem* itemFromString(Q3ListViewItem* parent, const QString& s);
  QVector<Q3ListViewItem*> m_lastPath;
  QString m_pathSeparator;
};

#endif
