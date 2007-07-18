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
/* Modifications by Marc Britton (c) 2002-2003 under GNU GPL, terms as above */

#ifndef _HAVE_KOMMANDERFACTORY_H_
#define _HAVE_KOMMANDERFACTORY_H_

#include <qstring.h>
#include <q3ptrlist.h>
#include <qimage.h>
#include <qpixmap.h>
#include <q3valuelist.h>
#include <qmap.h>
#include <qaction.h>
#include "kommander_export.h"

class QWidget;
class QLayout;
class QDomElement;
class Q3ListViewItem;
class Q3Table;
class KommanderPlugin;

struct KommanderWidgetInfo
{
    KommanderWidgetInfo() {}
    KommanderWidgetInfo( const QString &g, const QString &t, const QString &w = QString::null, bool c = FALSE )
	: group( g ), toolTip( t ), whatsThis( w ), isContainer( c )
    {
    }
    QString group;
    QString toolTip;
    QString whatsThis;
    bool isContainer;
};
typedef QMap<QString, KommanderWidgetInfo> FeatureList;

class KOMMANDER_EXPORT KommanderFactory
{
public:
    KommanderFactory();
    virtual ~KommanderFactory();

    static QWidget *create( const QString &uiFile, QObject *connector = 0, QWidget *parent = 0, const char *name = 0 );
    static QWidget *create( QIODevice *dev, QObject *connector = 0, QWidget *parent = 0, const char *name = 0 );
    static int loadPlugins( bool force = FALSE );
    static void addPlugin( KommanderPlugin *plugin );
    static void loadImages( const QString &dir );

    static QWidget *createWidget( const QString &className, QWidget *parent, const char *name );

    static FeatureList featureList();
private:
    enum LayoutType { HBox, VBox, Grid, NoLayout };
    void loadImageCollection( const QDomElement &e );
    void loadConnections( const QDomElement &e, QObject *connector );
    void loadTabOrder( const QDomElement &e );
    QWidget *createWidgetInternal( const QDomElement &e, QWidget *parent, QLayout* layout, const QString &classNameArg );
    QLayout *createLayout( QWidget *widget, QLayout*  layout, LayoutType type );
    LayoutType layoutType( QLayout *l ) const;
    void setProperty( QObject* widget, const QString &prop, const QDomElement &e );
    void createSpacer( const QDomElement &e, QLayout *layout );
    QImage loadFromCollection( const QString &name );
    QPixmap loadPixmap( const QDomElement &e );
    QColorGroup loadColorGroup( const QDomElement &e );
    void createColumn( const QDomElement &e, QWidget *widget );
    void loadItem( const QDomElement &e, QPixmap &pix, QString &txt, bool &hasPixmap );
    void createItem( const QDomElement &e, QWidget *widget, Q3ListViewItem *i = 0 );
    void loadChildAction( QObject *parent, const QDomElement &e );
    void loadActions( const QDomElement &e );
    void loadToolBars( const QDomElement &e );
    void loadMenuBar( const QDomElement &e );
    QAction *findAction( const QString &name );
    QString translate( const QString& sourceText, const QString& comment = "" );

private:
    struct Image {
	QImage img;
	QString name;
	bool operator==(  const Image &i ) const {
	    return ( i.name == name &&
		     i.img == img );
	}
    };

    struct Field
    {
	Field() {}
	Field( const QString &s1, const QPixmap &p, const QString &s2 ) : name( s1 ), pix( p ), field( s2 ) {}
	QString name;
	QPixmap pix;
	QString field;
#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
	bool operator==( const Field& ) const { return FALSE; }
#endif
    };

    struct EventFunction
    {
	EventFunction() {}
	EventFunction( const QString &e, const QStringList &f )
	    : events( e ) { functions.append( f ); }
	QStringList events;
	Q3ValueList<QStringList> functions;
    };

    struct SqlWidgetConnection
    {
	SqlWidgetConnection() {}
	SqlWidgetConnection( const QString &c, const QString &t )
	    : conn( c ), table( t ), dbControls( new QMap<QString, QString>() ) {}
	QString conn;
	QString table;
	QMap<QString, QString> *dbControls;
    };

    struct Functions
    {
	QString functions;
    };

    Q3ValueList<Image> images;
    QWidget *toplevel;
    Q3ListViewItem *lastItem;
    QMap<QString, QString> *dbControls;
    QMap<QString, QStringList> dbTables;
    QMap<QWidget*, SqlWidgetConnection> sqlWidgetConnections;
    QMap<QString, QString> buddies;
    QMap<Q3Table*, Q3ValueList<Field> > fieldMaps;
    Q3PtrList<QAction> actionList;
   QMap<QObject *, EventFunction> eventMap;
    QMap<QString, QString> languageSlots;
    QMap<QString, Functions*> languageFunctions;
    QStringList variables;
    QStringList noDatabaseWidgets;
    bool usePixmapCollection;
    int defMargin, defSpacing;

};

#endif
