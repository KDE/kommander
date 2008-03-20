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
#include <klistview.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qstringlist.h>
#include <qptrvector.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <kommander_export.h>

class QWidget;

class QShowEvent;
class KOMMANDER_EXPORT TreeWidget : public KListView, public KommanderWidget
{
  Q_OBJECT
  
  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
  Q_PROPERTY(QString pathSeparator READ pathSeparator WRITE setPathSeparator)
  
public:
  TreeWidget(QWidget *a_parent, const char *a_name);
  ~TreeWidget();
  virtual void setCurrentItem(QListViewItem* item);  
  QString pathSeparator() const;
  void setPathSeparator(const QString& a_pathSep);

  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);

  virtual QString handleDCOP(int function, const QStringList& args);
  virtual bool isFunctionSupported(int function);
public slots:
  virtual void setWidgetText(const QString&);
  virtual void populate();
signals:
  void widgetOpened();
  void widgetTextChanged(const QString&);
  void contextMenuRequested(int xpos, int ypos);
protected:
  void showEvent(QShowEvent *e);
  void contextMenuEvent( QContextMenuEvent * e );
  int itemToIndex(QListViewItem* item);
  QString itemText(QListViewItem* item) const;
  QString itemsText();
  QListViewItem* indexToItem(int index);
  QString itemPath(QListViewItem* item) const;
private:
  void addItemFromString(const QString& s);
  QListViewItem* itemFromString(QListViewItem* parent, const QString& s);
  QPtrVector<QListViewItem> m_lastPath;
  QString m_pathSeparator;
  int addColumnTree(const QString & label, int width = -1 );
  void setSortCol(int column, bool ascending=true);
  void setAltBackground(const QColor & c);
};

#endif
