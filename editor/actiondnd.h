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

#ifndef ACTIONDND_H
#define ACTIONDND_H

#include <qaction.h>
#include <qmap.h>
#include <qmenubar.h>
#include <qpixmap.h>
#include <q3popupmenu.h>
#include <q3ptrlist.h>
#include <q3toolbar.h>
#include <qpointer.h>
//Added by qt3to4:
#include <QContextMenuEvent>
#include <QDragMoveEvent>
#include <Q3CString>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QDragEnterEvent>
#include <Q3ActionGroup>

class QDesignerPopupMenu;
class QDesignerIndicatorWidget;
class FormWindow;

class QDesignerActionGroup : public Q3ActionGroup
{
    Q_OBJECT

public:
    QDesignerActionGroup( QObject *parent )
	: Q3ActionGroup( !parent || parent->inherits( "QActionGroup" ) ? parent : 0 ), wid( 0 ), idx( -1 ) {}

    QWidget *widget() const { return wid; }
    QWidget *widget( QAction *a ) const { return *widgets.find( a ); }
    int index() const { return idx; }

protected:
    void addedTo( QWidget *w, QWidget * ) {
	wid = w;
    }
    void addedTo( QWidget *w, QWidget *, QAction *a ) {
	widgets.insert( a, w );
    }
    void addedTo( int index, Q3PopupMenu * ) {
	idx = index;
    }

private:
    QWidget *wid;
    QMap<QAction *, QWidget *> widgets;
    int idx;

};

class QDesignerAction : public QAction
{
    Q_OBJECT

public:
    QDesignerAction( QObject *parent )
	: QAction( !parent || parent->inherits( "QActionGroup" ) ? parent : 0 ), wid( 0 ), idx( -1 ), widgetToInsert( 0 ) {}
    QDesignerAction( QWidget *w, QObject *parent )
	: QAction( !parent || parent->inherits( "QActionGroup" ) ? parent : 0 ), wid( 0 ), idx( -1 ), widgetToInsert( w ) {}

    QWidget *widget() const { return wid; }
    int index() const { return idx; }

    bool addTo( QWidget *w );
    bool removeFrom( QWidget *w );

    void remove();
    bool supportsMenu() const { return !widgetToInsert; }

protected:
    void addedTo( QWidget *w, QWidget * ) {
	wid = w;
    }
    void addedTo( int index, Q3PopupMenu * ) {
	idx = index;
    }

private:
    QWidget *wid;
    int idx;
    QWidget *widgetToInsert;

};

class QDesignerToolBarSeparator : public QWidget
{
    Q_OBJECT

public:
    QDesignerToolBarSeparator( Qt::Orientation, Q3ToolBar *parent, const char* name=0 );

    QSize sizeHint() const;
    Qt::Orientation orientation() const { return orient; }
public slots:
   void setOrientation( Qt::Orientation );
protected:
    void styleChange( QStyle& );
    void paintEvent( QPaintEvent * );
private:
    Orientation orient;
};


class QSeparatorAction : public QAction
{
    Q_OBJECT

public:
    QSeparatorAction( QObject *parent );

    bool addTo( QWidget *w );
    bool removeFrom( QWidget *w );
    QWidget *widget() const;
    int index() const;

private:
    QWidget *wid;
    int idx;

};

class QDesignerToolBar : public Q3ToolBar
{
    Q_OBJECT

public:
    QDesignerToolBar( Q3MainWindow *mw );
    QDesignerToolBar( Q3MainWindow *mw, Qt::ToolBarDock dock );
    Q3PtrList<QAction> insertedActions() const { return actionList; }
    void addAction( QAction *a );

    void clear();
    void installEventFilters( QWidget *w );
    void insertAction( QWidget *w, QAction *a ) { actionMap.insert( w, a ); }
    void insertAction( int index, QAction *a ) { actionList.insert( index, a ); }
    void appendAction( QAction *a ) { actionList.append( a ); }
    void removeAction( QAction *a ) { actionList.remove( a ); }
    void reInsert();
    void removeWidget( QWidget *w );

protected:
    bool eventFilter( QObject *, QEvent * );
    void paintEvent( QPaintEvent * );
#ifndef QT_NO_DRAGANDDROP
    void dragEnterEvent( QDragEnterEvent * );
    void dragMoveEvent( QDragMoveEvent * );
    void dragLeaveEvent( QDragLeaveEvent * );
    void dropEvent( QDropEvent * );
#endif
    void contextMenuEvent( QContextMenuEvent *e );
    void mousePressEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );

private slots:
    void actionRemoved();

private:
    void drawIndicator( const QPoint &pos );
    QPoint calcIndicatorPos( const QPoint &pos );
    void buttonContextMenuEvent( QContextMenuEvent *e, QObject *o );
    void buttonMousePressEvent( QMouseEvent *e, QObject *o );
    void buttonMouseMoveEvent( QMouseEvent *e, QObject *o );
    void buttonMouseReleaseEvent( QMouseEvent *e, QObject *o );
    void doInsertWidget( const QPoint &p );
    void findFormWindow();

private:
    QPoint lastIndicatorPos;
    QWidget *insertAnchor;
    bool afterAnchor;
    Q3PtrList<QAction> actionList;
    QMap<QWidget*, QAction*> actionMap;
    QPoint dragStartPos;
    QDesignerIndicatorWidget *indicator;
    bool widgetInserting;
    FormWindow *formWindow;

};

class QDesignerMenuBar : public QMenuBar
{
    Q_OBJECT
    friend class QDesignerPopupMenu;

    Q_PROPERTY( int itemNumber WRITE setItemNumber READ itemNumber )
    Q_PROPERTY( QString itemText WRITE setItemText READ itemText )
    Q_PROPERTY( Q3CString itemName WRITE setItemName READ itemName )

public:
    QDesignerMenuBar( QWidget *mw );

    void setItemNumber( int num );
    int itemNumber() const;
    void setItemText( const QString &s );
    QString itemText() const;
    void setItemName( const Q3CString &s );
    Q3CString itemName() const;

protected:
    void mousePressEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void contextMenuEvent( QContextMenuEvent *e );
#ifndef QT_NO_DRAGANDDROP
    void dragEnterEvent( QDragEnterEvent * );
    void dragMoveEvent( QDragMoveEvent * );
    void dragLeaveEvent( QDragLeaveEvent * );
    void dropEvent( QDropEvent * );
#endif

private:
    void drawIndicator( const QPoint &pos );
    QPoint calcIndicatorPos( const QPoint &pos );
    void findFormWindow();

private:
    int itemNum;
    QPoint dragStartPos;
    bool mousePressed;
    QPoint lastIndicatorPos;
    int insertAt;
    QDesignerIndicatorWidget *indicator;
    FormWindow *formWindow;
    int oldPos;

};

class QDesignerPopupMenu : public Q3PopupMenu
{
    Q_OBJECT

public:
    QDesignerPopupMenu( QWidget *w );
    Q3PtrList<QAction> insertedActions() const { return actionList; }
    void addAction( QAction *a );
    void reInsert();
    void insertAction( int index, QAction *a ) { actionList.insert( index, a ); }
    void removeAction( QAction *a ) { actionList.remove( a ); }

protected:
    void mousePressEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void contextMenuEvent( QContextMenuEvent *e );
    void paintEvent( QPaintEvent * );
#ifndef QT_NO_DRAGANDDROP
    void dragEnterEvent( QDragEnterEvent * );
    void dragMoveEvent( QDragMoveEvent * );
    void dragLeaveEvent( QDragLeaveEvent * );
    void dropEvent( QDropEvent * );
#endif

private slots:
    void actionRemoved();
    void createPopupMenu();

private:
    void drawIndicator( const QPoint &pos );
    QPoint calcIndicatorPos( const QPoint &pos );
    void findFormWindow();

private:
    QPoint lastIndicatorPos;
    int insertAt;
    Q3PtrList<QAction> actionList;
    QPoint dragStartPos;
    bool mousePressed;
    QDesignerIndicatorWidget *indicator;
    FormWindow *formWindow;
    QPointer<Q3PopupMenu> popupMenu;
    QPoint popupPos;
    QPoint popupLocalPos;

};

class QDesignerIndicatorWidget : public QWidget
{
    Q_OBJECT

public:
    QDesignerIndicatorWidget( QWidget *p )
	: QWidget( p, "qt_dockwidget_internal" ) {
	    setBackgroundColor( red );
    }

};

#endif
