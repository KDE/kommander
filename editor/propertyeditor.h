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

#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <qfeatures.h>
#include <qvariant.h>
#include <q3listview.h>
#include <q3ptrlist.h>
#include <qpointer.h>
#include <qtabwidget.h>
#include <qmodules.h>
//Added by qt3to4:
#include <QDragMoveEvent>
#include <Q3CString>
#include <QCloseEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <Q3Frame>
#include <QLabel>
#include <QEvent>
#include <QDragEnterEvent>
#include "hierarchyview.h"

class PropertyList;
class PropertyEditor;
class QPainter;
class QColorGroup;
class QComboBox;
class QLineEdit;
class QPushButton;
class Q3HBox;
class QSpinBox;
class QLabel;
class FormWindow;
class QCloseEvent;
class QResizeEvent;
class PropertyWhatsThis;
class Q3DateEdit;
class Q3TimeEdit;
class Q3DateTimeEdit;

class PropertyItem : public Q3ListViewItem
{
public:
    PropertyItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName );
    ~PropertyItem();

    void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align );
    void paintBranches( QPainter * p, const QColorGroup & cg,
			int w, int y, int h );
    void paintFocus( QPainter *p, const QColorGroup &cg, const QRect &r );

    virtual bool hasSubItems() const;
    virtual void createChildren();
    virtual void initChildren();

    bool isOpen() const;
    void setOpen( bool b );

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    QVariant value() const;
    QString name() const;
    virtual void notifyValueChange();

    virtual void setChanged( bool b, bool updateDb = TRUE );
    bool isChanged() const;

    virtual void placeEditor( QWidget *w );

    virtual PropertyItem *propertyParent() const;
    virtual void childValueChanged( PropertyItem *child );

    void addChild( PropertyItem *i );
    int childCount() const;
    PropertyItem *child( int i ) const;

    virtual bool hasCustomContents() const;
    virtual void drawCustomContents( QPainter *p, const QRect &r );

    void updateBackColor();

    void setup() { Q3ListViewItem::setup(); setHeight( Q3ListViewItem::height() + 2 ); }

    virtual QString currentItem() const;
    virtual int currentIntItem() const;
    virtual void setCurrentItem( const QString &s );
    virtual void setCurrentItem( int i );
    virtual int currentIntItemFromObject() const;
    virtual QString currentItemFromObject() const;

    void setFocus( QWidget *w );

    virtual void toggle();
    void setText( int col, const QString &txt );

protected:
    PropertyList *listview;
    QVariant val;

private:
    QColor backgroundColor();
    void createResetButton();
    void updateResetButtonState();

private:
    bool open, changed;
    PropertyItem *property;
    QString propertyName;
    Q3PtrList<PropertyItem> children;
    QColor backColor;
    QPushButton *resetButton;

};

class PropertyTextItem : public QObject,
			 public PropertyItem
{
    Q_OBJECT

public:
    PropertyTextItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		      const QString &propName, bool comment, bool multiLine, bool ascii = FALSE, bool a = FALSE );
    ~PropertyTextItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

    virtual void setChanged( bool b, bool updateDb = TRUE );

private slots:
    void setValue();
    void getText();

private:
    QLineEdit *lined();
    QPointer<QLineEdit> lin;
    QPointer<Q3HBox> box;
    QPushButton *button;
    bool withComment, hasMultiLines, asciiOnly, accel;

};


class PropertyBoolItem : public QObject,
			 public PropertyItem
{
    Q_OBJECT

public:
    PropertyBoolItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName );
    ~PropertyBoolItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    virtual void toggle();

private slots:
    void setValue();

private:
    QComboBox *combo();
    QPointer<QComboBox> comb;

};

class PropertyIntItem : public QObject,
			public PropertyItem
{
    Q_OBJECT

public:
    PropertyIntItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		     const QString &propName, bool s );
    ~PropertyIntItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

private slots:
    void setValue();

private:
    QSpinBox *spinBox();
    QPointer<QSpinBox> spinBx;
    bool signedValue;

};

class PropertyDoubleItem : public QObject,
			public PropertyItem
{
    Q_OBJECT

public:
    PropertyDoubleItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		     const QString &propName );
    ~PropertyDoubleItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

private slots:
    void setValue();

private:
    QLineEdit *lined();
    QPointer<QLineEdit> lin;
};

class PropertyListItem : public QObject,
			 public PropertyItem
{
    Q_OBJECT

public:
    PropertyListItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		      const QString &propName, bool editable );
    ~PropertyListItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

    QString currentItem() const;
    int currentIntItem() const;
    void setCurrentItem( const QString &s );
    void setCurrentItem( int i );
    int currentIntItemFromObject() const;
    QString currentItemFromObject() const;
    void addItem( const QString &s );

private slots:
    void setValue();

private:
    QComboBox *combo();
    QPointer<QComboBox> comb;
    int oldInt;
    bool editable;
    QString oldString;

};

class PropertyFontItem : public QObject,
			 public PropertyItem
{
    Q_OBJECT

public:
    PropertyFontItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName );
    ~PropertyFontItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

private slots:
    void getFont();

private:
    QPointer<QLineEdit> lined;
    QPointer<QPushButton> button;
    QPointer<Q3HBox> box;

};

class PropertyCoordItem : public QObject,
			  public PropertyItem
{
    Q_OBJECT

public:
    enum Type { Rect, Size, Point };

    PropertyCoordItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		       const QString &propName, Type t );
    ~PropertyCoordItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

private:
    QLineEdit *lined();
    QPointer<QLineEdit> lin;
    Type typ;

};

class PropertyColorItem : public QObject,
			  public PropertyItem
{
    Q_OBJECT

public:
    PropertyColorItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		       const QString &propName, bool children );
    ~PropertyColorItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

    virtual bool hasCustomContents() const;
    virtual void drawCustomContents( QPainter *p, const QRect &r );

private slots:
    void getColor();

private:
    QPointer<Q3HBox> box;
    QPointer<Q3Frame> colorPrev;
    QPointer<QPushButton> button;
    bool withChildren;

};

class PropertyPixmapItem : public QObject,
			   public PropertyItem
{
    Q_OBJECT

public:
    PropertyPixmapItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
			const QString &propName, bool isIconSet = FALSE );
    ~PropertyPixmapItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

    virtual bool hasCustomContents() const;
    virtual void drawCustomContents( QPainter *p, const QRect &r );

private slots:
    void getPixmap();

private:
    QPointer<Q3HBox> box;
    QPointer<QLabel> pixPrev;
    QPushButton *button;
    bool iconSet;

};


class PropertySizePolicyItem : public QObject,
			  public PropertyItem
{
    Q_OBJECT

public:
    PropertySizePolicyItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
			    const QString &propName );
    ~PropertySizePolicyItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

private:
    QLineEdit *lined();
    QPointer<QLineEdit> lin;

};

class PropertyPaletteItem : public QObject,
			    public PropertyItem
{
    Q_OBJECT

public:
    PropertyPaletteItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
			const QString &propName );
    ~PropertyPaletteItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

    virtual bool hasCustomContents() const;
    virtual void drawCustomContents( QPainter *p, const QRect &r );

private slots:
    void getPalette();

private:
    QPointer<Q3HBox> box;
    QPointer<QLabel> palettePrev;
    QPointer<QPushButton> button;

};

class PropertyCursorItem : public QObject,
			   public PropertyItem
{
    Q_OBJECT

public:
    PropertyCursorItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
			const QString &propName );
    ~PropertyCursorItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

private slots:
    void setValue();

private:
    QComboBox *combo();
    QPointer<QComboBox> comb;

};


class PropertyDatabaseItem : public QObject,
			     public PropertyItem
{
    Q_OBJECT

public:
    PropertyDatabaseItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName, bool wField );
    ~PropertyDatabaseItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

private:
    QPointer<QLineEdit> lined;
    QPointer<QPushButton> button;
    QPointer<Q3HBox> box;
    bool withField;

};

class PropertyList : public Q3ListView
{
    Q_OBJECT

public:
    PropertyList( PropertyEditor *e );

    virtual void setupProperties();

    virtual void setCurrentItem( Q3ListViewItem *i );
    virtual void valueChanged( PropertyItem *i );
    virtual void refetchData();
    virtual void setPropertyValue( PropertyItem *i );
    virtual void setCurrentProperty( const QString &n );

    PropertyEditor *propertyEditor() const;
    QString whatsThisAt( const QPoint &p );
    void showCurrentWhatsThis();

public slots:
    void updateEditorSize();
    void resetProperty();
    void toggleSort();

private slots:
    void itemPressed( Q3ListViewItem *i, const QPoint &p, int c );
    void toggleOpen( Q3ListViewItem *i );
    bool eventFilter( QObject *o, QEvent *e );

protected:
    void resizeEvent( QResizeEvent *e );
    void paintEmptyArea( QPainter *p, const QRect &r );
    bool addPropertyItem( PropertyItem *&item, const Q3CString &name, QVariant::Type t );

    void viewportDragEnterEvent( QDragEnterEvent * );
    void viewportDragMoveEvent ( QDragMoveEvent * );
    void viewportDropEvent ( QDropEvent * );

protected:
    PropertyEditor *editor;

private:
    void readPropertyDocs();
    QString whatsThisText( Q3ListViewItem *i );

private:
    PropertyListItem* pressItem;
    QPoint pressPos;
    bool mousePressed;
    bool showSorted;
    QMap<QString, QString> propertyDocs;
    PropertyWhatsThis *whatsThis;

};

class PropertyEditor : public QTabWidget
{
    Q_OBJECT

public:
    PropertyEditor( QWidget *parent );

    QObject *widget() const;

    void clear();
    void setup();

    void emitWidgetChanged();
    void refetchData();

    void closed( FormWindow *w );

    PropertyList *propertyList() const;
    FormWindow *formWindow() const;
#ifndef KOMMANDER
//    EventList *eventList() const;
#endif

    QString currentProperty() const;
    QString classOfCurrentProperty() const;
    QMetaObject* metaObjectOfCurrentProperty() const;

    void resetFocus();

signals:
    void hidden();

public slots:
    void setWidget( QObject *w, FormWindow *fw );

protected:
    void closeEvent( QCloseEvent *e );

private:
    QObject *wid;
    PropertyList *listview;
#ifndef KOMMANDER
    EventList *eList;
#endif
    FormWindow *formwindow;

};

class PropertyDateItem : public QObject,
			 public PropertyItem
{
    Q_OBJECT

public:
    PropertyDateItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName );
    ~PropertyDateItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

private slots:
    void setValue();

private:
    Q3DateEdit *lined();
    QPointer<Q3DateEdit> lin;

};

class PropertyTimeItem : public QObject,
			 public PropertyItem
{
    Q_OBJECT

public:
    PropertyTimeItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName );
    ~PropertyTimeItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

private slots:
    void setValue();

private:
    Q3TimeEdit *lined();
    QPointer<Q3TimeEdit> lin;

};

class PropertyDateTimeItem : public QObject,
			 public PropertyItem
{
    Q_OBJECT

public:
    PropertyDateTimeItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName );
    ~PropertyDateTimeItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

private slots:
    void setValue();

private:
    Q3DateTimeEdit *lined();
    QPointer<Q3DateTimeEdit> lin;

};

#endif
