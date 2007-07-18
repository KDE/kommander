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

#ifndef METADATABASE_H
#define METADATABASE_H

#include <qvariant.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <q3ptrlist.h>
#include <qsizepolicy.h>
#include <qsize.h>
#include <qwidget.h>
#include <qcursor.h>
//Added by qt3to4:
#include <Q3CString>
#include <Q3ValueList>
#include <QPixmap>

#include "pixmapchooser.h"

class QObject;
class QPixmap;

class MetaDataBase
{
public:
    struct Connection
    {
	QObject *sender, *receiver;
	Q3CString signal, slot;
	bool operator==( const Connection &c ) const {
	    return sender == c.sender && receiver == c.receiver &&
		   signal == c.signal && slot == c.slot ;
	}
    };

    struct Slot
    {
	QString returnType;
	Q3CString slot;
	QString specifier;
	QString access;
	QString language;
	bool operator==( const Slot &s ) const {
	    return ( returnType == s.returnType &&
		     slot == s.slot &&
		     access == s.access &&
		     specifier == s.specifier &&
		     language == s.language
		     );
	}
    };

    struct Property
    {
	Q3CString property;
	QString type;
	bool operator==( const Property &p ) const {
	    return property == p.property &&
		 type == p.type;
	}
    };

    struct CustomWidget
    {
	CustomWidget();
	CustomWidget( const CustomWidget &w );
	~CustomWidget() { delete pixmap; } // inlined to work around 2.7.2.3 bug
	bool operator==( const CustomWidget &w ) const;
	CustomWidget &operator=( const CustomWidget &w );

	bool hasSignal( const Q3CString &signal ) const;
	bool hasSlot( const Q3CString &slot ) const;
	bool hasProperty( const Q3CString &prop ) const;

	enum IncludePolicy { Global, Local };
	QString className;
	QString includeFile;
	IncludePolicy includePolicy;
	QSize sizeHint;
	QSizePolicy sizePolicy;
	QPixmap *pixmap;
	Q3ValueList<Q3CString> lstSignals;
	Q3ValueList<Slot> lstSlots;
	Q3ValueList<Property> lstProperties;
	int id;
	bool isContainer;
    };

    struct Include
    {
	Include() : header(), location(), implDecl( "in implementation" ) {}
	QString header;
	QString location;
	bool operator==( const Include &i ) const {
	    return header == i.header && location == i.location;
	}
	QString implDecl;
    };

    struct MetaInfo
    {
      QString license;
      QString version;
      QString comment;
      QString author;
    };

    struct EventDescription
    {
	QString name;
	QStringList args;
#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
	bool operator==( const EventDescription& ) const { return FALSE; }
#endif
    };

    MetaDataBase();
    static void clearDataBase();

    static void addEntry( QObject *o );
    static void removeEntry( QObject *o );
    static void setPropertyChanged( QObject *o, const QString &property, bool changed );
    static bool isPropertyChanged( QObject *o, const QString &property );
    static void setPropertyComment( QObject *o, const QString &property, const QString &comment );
    static QString propertyComment( QObject *o, const QString &property );
    static QStringList changedProperties( QObject *o );

    static void setFakeProperty( QObject *o, const QString &property, const QVariant& value );
    static QVariant fakeProperty( QObject * o, const QString &property );
    static QMap<QString,QVariant>* fakeProperties( QObject* o );

    static void setSpacing( QObject *o, int spacing );
    static int spacing( QObject *o );
    static void setMargin( QObject *o, int margin );
    static int margin( QObject *o );

    static void addConnection( QObject *o, QObject *sender, const Q3CString &signal,
			       QObject *receiver, const Q3CString &slot );
    static void removeConnection( QObject *o, QObject *sender, const Q3CString &signal,
				  QObject *receiver, const Q3CString &slot );
    static Q3ValueList<Connection> connections( QObject *o );
    static Q3ValueList<Connection> connections( QObject *o, QObject *sender, QObject *receiver );
    static Q3ValueList<Connection> connections( QObject *o, QObject *object );
    static void doConnections( QObject *o );

    static void addSlot( QObject *o, const Q3CString &slot, const QString& specifier, const QString &access, const QString &language, const QString &returnType );
    static void removeSlot( QObject *o, const Q3CString &slot, const QString& specifier, const QString &access, const QString &language, const QString &returnType );
    static void removeSlot( QObject *o, const QString &slot );
    static Q3ValueList<Slot> slotList( QObject *o );
    static bool isSlotUsed( QObject *o, const Q3CString &slot );
    static bool hasSlot( QObject *o, const Q3CString &slot, bool onlyCustom = FALSE );
    static void changeSlot( QObject *o, const Q3CString &slot, const Q3CString &newName );
    static void changeSlotAttributes( QObject *o, const Q3CString &slot,
				      const QString& specifier, const QString &access,
				      const QString &language, const QString &returnType );
    static QString languageOfSlot( QObject *o, const Q3CString &slot );
    static void setSlotList( QObject *o, const Q3ValueList<Slot> &slotList );

    static bool addCustomWidget( CustomWidget *w );
    static void removeCustomWidget( CustomWidget *w );
    static Q3PtrList<CustomWidget> *customWidgets();
    static CustomWidget *customWidget( int id );
    static bool isWidgetNameUsed( CustomWidget *w );
    static bool hasCustomWidget( const QString &className );

    static void setTabOrder( QWidget *w, const QWidgetList &order );
    static QWidgetList tabOrder( QWidget *w );

    static void setIncludes( QObject *o, const Q3ValueList<Include> &incs );
    static Q3ValueList<Include> includes( QObject *o );

    static void setForwards( QObject *o, const QStringList &fwds );
    static QStringList forwards( QObject *o );

    static void setSignalList( QObject *o, const QStringList &sigs );
    static QStringList signalList( QObject *o );

    static void setMetaInfo( QObject *o, MetaInfo mi );
    static MetaInfo metaInfo( QObject *o );

    static void setCursor( QWidget *w, const QCursor &c );
    static QCursor cursor( QWidget *w );

    static void setPixmapArgument( QObject *o, int pixmap, const QString &arg );
    static QString pixmapArgument( QObject *o, int pixmap );
    static void clearPixmapArguments( QObject *o );

    static void setPixmapKey( QObject *o, int pixmap, const QString &arg );
    static QString pixmapKey( QObject *o, int pixmap );
    static void clearPixmapKeys( QObject *o );

    static void setColumnFields( QObject *o, const QMap<QString, QString> &columnFields );
    static QMap<QString, QString> columnFields( QObject *o );

    static QString normalizeSlot( const QString &slot );

    static void clear( QObject *o );

    static void setExportMacro( QObject *o, const QString &macro );
    static QString exportMacro( QObject *o );

    static bool hasObject( QObject *o );

};

#endif
