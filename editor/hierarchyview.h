/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef HIRARCHYVIEW_H
#define HIRARCHYVIEW_H

#include <qvariant.h>
#include <qlistview.h>
#include <qtabwidget.h>
#include <qguardedptr.h>
#include <private/qcom_p.h>

class FormWindow;
class QCloseEvent;
class QPopupMenu;
class QKeyEvent;
class QMouseEvent;
class QWizard;
class QToolBox;

class HierarchyItem:public QListViewItem
{
public:
  enum Type
  {
    Widget,
    SlotParent,
    Public,
    Protected,
    Private,
    Slot,
    DefinitionParent,
    Definition,
    Event,
    EventFunction
  };

    HierarchyItem(Type type, QListViewItem * parent,
      const QString & txt1, const QString & txt2, const QString & txt3);
    HierarchyItem(Type type, QListView * parent,
      const QString & txt1, const QString & txt2, const QString & txt3);

  void paintCell(QPainter * p, const QColorGroup & cg, int column, int width, int align);
  void updateBackColor();

  void setWidget(QWidget * w);
  QWidget *widget() const;

  void setText(int col, const QString & txt)    {if (!txt.isEmpty()) QListViewItem::setText(col, txt);}
  int rtti() const                              { return (int) typ;}

private:
  void okRename(int col);
  void cancelRename(int col);

private:
  QColor backgroundColor();
  QColor backColor;
  QWidget *wid;
  Type typ;
};

class HierarchyList:public QListView
{
  Q_OBJECT 
public:
  HierarchyList(QWidget * parent, FormWindow * fw, bool doConnects = TRUE);

  virtual void setup();
  virtual void setCurrent(QWidget * w);
  void setOpen(QListViewItem * i, bool b);
  void changeNameOf(QWidget * w, const QString & name);
  void changeDatabaseOf(QWidget * w, const QString & info);
  void setFormWindow(FormWindow * fw)   {formWindow = fw;}
  void drawContentsOffset(QPainter * p, int ox, int oy, int cx, int cy, int cw, int ch)
        { setUpdatesEnabled(FALSE);  triggerUpdate();  setUpdatesEnabled(TRUE);
         QListView::drawContentsOffset(p, ox, oy, cx, cy, cw, ch); }
  void insertEntry(QListViewItem * i, const QPixmap & pix = QPixmap(), const QString & s =
      QString::null);

protected:
  void keyPressEvent(QKeyEvent * e);
  void keyReleaseEvent(QKeyEvent * e);
  void viewportMousePressEvent(QMouseEvent * e);
  void viewportMouseReleaseEvent(QMouseEvent * e);

public slots: 
  void addTabPage();
  void removeTabPage();

private:
  void insertObject(QObject * o, QListViewItem * parent);
  QWidget *findWidget(QListViewItem * i);
  QListViewItem *findItem(QWidget * w);
  QWidget *current() const;

  private slots: virtual void objectClicked(QListViewItem * i);
  virtual void showRMBMenu(QListViewItem *, const QPoint &);

protected:
  FormWindow * formWindow;
  QPopupMenu *normalMenu, *tabWidgetMenu;
  bool deselect;
};

class HierarchyView : public QTabWidget
{
    Q_OBJECT

public:
    HierarchyView( QWidget *parent );
    ~HierarchyView();

    void setFormWindow( FormWindow *fw, QWidget *w );
    FormWindow *formWindow() const;
    void clear();
    void widgetInserted( QWidget *w );
    void widgetRemoved( QWidget *w );
    void widgetsInserted( const QWidgetList &l );
    void widgetsRemoved( const QWidgetList &l );
    void namePropertyChanged( QWidget *w, const QVariant &old );
    void databasePropertyChanged( QWidget *w, const QStringList& info );
    void tabsChanged( QTabWidget *w );
    void tabsChanged( QToolBox *w );
    void pagesChanged( QWizard *w );
    void rebuild();
    void closed( FormWindow *fw );

protected slots:

protected:
    void closeEvent( QCloseEvent *e );

signals:
    void hidden();

private:
    FormWindow *formwindow;
    HierarchyList *listview;

};


#endif
