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
#include <qptrlist.h>
#include <qsizepolicy.h>
#include <qsize.h>
#include <qwidgetlist.h>
#include <qcursor.h>

#include "pixmapchooser.h"

class QObject;
class QPixmap;
struct LanguageInterface;

class MetaDataBase
{
public:
    struct Connection
    {
	QObject *sender, *receiver;
	QCString signal, slot;
	bool operator==( const Connection &c ) const {
	    return sender == c.sender && receiver == c.receiver &&
		   signal == c.signal && slot == c.slot ;
	}
    };

    struct Slot
    {
	QString returnType;
	QCString slot;
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
	QCString property;
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

	bool hasSignal( const QCString &signal ) const;
	bool hasSlot( const QCString &slot ) const;
	bool hasProperty( const QCString &prop ) const;

	enum IncludePolicy { Global, Local };
	QString className;
	QString includeFile;
	IncludePolicy includePolicy;
	QSize sizeHint;
	QSizePolicy sizePolicy;
	QPixmap *pixmap;
	QValueList<QCString> lstSignals;
	QValueList<Slot> lstSlots;
	QValueList<Property> lstProperties;
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
	QString className;
	bool classNameChanged;
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

    static void addConnection( QObject *o, QObject *sender, const QCString &signal,
			       QObject *receiver, const QCString &slot );
    static void removeConnection( QObject *o, QObject *sender, const QCString &signal,
				  QObject *receiver, const QCString &slot );
    static QValueList<Connection> connections( QObject *o );
    static QValueList<Connection> connections( QObject *o, QObject *sender, QObject *receiver );
    static QValueList<Connection> connections( QObject *o, QObject *object );
    static void doConnections( QObject *o );

    static void addSlot( QObject *o, const QCString &slot, const QString& specifier, const QString &access, const QString &language, const QString &returnType );
    static void removeSlot( QObject *o, const QCString &slot, const QString& specifier, const QString &access, const QString &language, const QString &returnType );
    static void removeSlot( QObject *o, const QString &slot );
    static QValueList<Slot> slotList( QObject *o );
    static bool isSlotUsed( QObject *o, const QCString &slot );
    static bool hasSlot( QObject *o, const QCString &slot, bool onlyCustom = FALSE );
    static void changeSlot( QObject *o, const QCString &slot, const QCString &newName );
    static void changeSlotAttributes( QObject *o, const QCString &slot,
				      const QString& specifier, const QString &access,
				      const QString &language, const QString &returnType );
    static QString languageOfSlot( QObject *o, const QCString &slot );
    static void setSlotList( QObject *o, const QValueList<Slot> &slotList );

    static bool addCustomWidget( CustomWidget *w );
    static void removeCustomWidget( CustomWidget *w );
    static QPtrList<CustomWidget> *customWidgets();
    static CustomWidget *customWidget( int id );
    static bool isWidgetNameUsed( CustomWidget *w );
    static bool hasCustomWidget( const QString &className );

    static void setTabOrder( QWidget *w, const QWidgetList &order );
    static QWidgetList tabOrder( QWidget *w );

    static void setIncludes( QObject *o, const QValueList<Include> &incs );
    static QValueList<Include> includes( QObject *o );

    static void setForwards( QObject *o, const QStringList &fwds );
    static QStringList forwards( QObject *o );

    static void setVariables( QObject *o, const QStringList &vars );
    static QStringList variables( QObject *o );

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

    static bool hasEvents( const QString &lang );

    static void setEditor( const QStringList &langs );
    static bool hasEditor( const QString &lang );

    static QValueList<EventDescription> events( QObject *o, const QString &lang );
    static bool setEventFunctions( QObject *o, QObject *form, const QString &lang,
				   const QString &event, const QStringList &functions, bool = TRUE );
    static QStringList eventFunctions( QObject *o, const QString &event, const QString &lang );
    static bool hasEventFunctions( QObject *o );
    static QMap<QString, QStringList> eventFunctions( QObject *o );
    static void setFunctionBodies( QObject *o, const QMap<QString, QString> &bodies, const QString &lang, const QString &returnType );
    static void addFunctionBody( QObject *o, const QString &func, const QString &body );
    static void setFunctionComments( QObject *o, const QString &func, const QString &comments );
    static QMap<QString, QString> functionBodies( QObject *o );
    static QString functionComments( QObject *o, const QString &func );

    static void setupInterfaceManagers();
    static QStringList languages();

    static LanguageInterface *languageInterface( const QString &lang );

    static QString normalizeSlot( const QString &slot );

    static void clear( QObject *o );

    static void setBreakPoints( QObject *o, const QValueList<int> &l );
    static QValueList<int> breakPoints( QObject *o );

    static void setExportMacro( QObject *o, const QString &macro );
    static QString exportMacro( QObject *o );

    static bool hasObject( QObject *o );

    static void functionNameChanged( QObject *o, const QString &oldName, const QString &newName );

};

#endif
