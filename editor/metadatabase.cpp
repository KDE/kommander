/**********************************************************************
 This file is based on Qt Designer, Copyright (C) 2000 Trolltech AS. All rights reserved.

 This file may be distributed and/or modified under the terms of the
 GNU General Public License version 2 as published by the Free Software
 Foundation and appearing in the file LICENSE.GPL included in the
 packaging of this file.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

 See http://www.trolltech.com/gpl/ for GPL licensing information.

 Modified for Kommander:
  (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
  (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>

**********************************************************************/

#include "metadatabase.h"
#include "widgetfactory.h"
#include "formwindow.h"
#include "parser.h"
#include "widgetdatabase.h"
#include "formfile.h"

#include <qapplication.h>
#include <qobject.h>
#include <qlayout.h>
#include <q3ptrdict.h>
#include <qobject.h>
#include <q3strlist.h>
#include <qmetaobject.h>
#include <qwidget.h>
#include <q3mainwindow.h>
#include <qregexp.h>
//Added by qt3to4:
#include <Q3CString>
#include <Q3ValueList>
#include <QPixmap>
#include <Q3PtrList>
#include <private/qpluginmanager_p.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <q3textstream.h>

#include <stdlib.h>


class MetaDataBaseRecord
{
public:
    QObject *object;
    QStringList changedProperties;
    QMap<QString,QVariant> fakeProperties;
    QMap<QString, QString> propertyComments;
    int spacing, margin;
    Q3ValueList<MetaDataBase::Connection> connections;
    Q3ValueList<MetaDataBase::Slot> slotList;
    Q3ValueList<MetaDataBase::Include> includes;
    QStringList forwards, variables, sigs;
    QWidgetList tabOrder;
    MetaDataBase::MetaInfo metaInfo;
    QCursor cursor;
    QMap<int, QString> pixmapArguments;
    QMap<int, QString> pixmapKeys;
    QMap<QString, QString> columnFields;
    QMap<QString, QStringList> eventFunctions;
    QMap<QString, QString> functionBodies;
    QMap<QString, QString> functionComments;
    Q3ValueList<int> breakPoints;
    QString exportMacro;
};

static Q3PtrDict<MetaDataBaseRecord> *db = 0;
static Q3PtrList<MetaDataBase::CustomWidget> *cWidgets = 0;
static bool doUpdate = true;
static QStringList langList;
static QStringList editorLangList;

/*!
  \class MetaDataBase metadatabase.h
  \brief Database which stores meta data of widgets

  The MetaDataBase stores meta information of widgets, which are not
  stored directly in widgets (properties). This is e.g. the
  information which properties have been modified.
*/

MetaDataBase::MetaDataBase()
{
}

inline void setupDataBase()
{
    if ( !db || !cWidgets ) {
        db = new Q3PtrDict<MetaDataBaseRecord>( 1481 );
        db->setAutoDelete( true );
        cWidgets = new Q3PtrList<MetaDataBase::CustomWidget>;
        cWidgets->setAutoDelete( true );
    }
}

void MetaDataBase::clearDataBase()
{
    delete db;
    db = 0;
    delete cWidgets;
    cWidgets = 0;
}

void MetaDataBase::addEntry( QObject *o )
{
    if ( !o )
        return;
    setupDataBase();
    if ( db->find( o ) )
        return;
    MetaDataBaseRecord *r = new MetaDataBaseRecord;
    r->object = o;
    r->spacing = r->margin = -1;
    db->insert( (void*)o, r );

    WidgetFactory::initChangedProperties( o );
}

void MetaDataBase::removeEntry( QObject *o )
{
    setupDataBase();
    db->remove( o );
}

void MetaDataBase::setPropertyChanged( QObject *o, const QString &property, bool changed )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    if ( changed ) {
        if ( r->changedProperties.findIndex( property ) == -1 )
            r->changedProperties.append( property );
    } else {
        if ( r->changedProperties.findIndex( property ) != -1 )
            r->changedProperties.remove( property );
    }

    if ( doUpdate &&
         ( property == "hAlign" || property == "vAlign" || property == "wordwrap" ) ) {
        doUpdate = false;
        setPropertyChanged( o, "alignment", changed ||
                            isPropertyChanged( o, "hAlign" ) ||
                            isPropertyChanged( o, "vAlign" ) ||
                            isPropertyChanged( o, "wordwrap" ) );
        doUpdate = true;
    }

    if ( doUpdate && property == "alignment" ) {
        doUpdate = false;
        setPropertyChanged( o, "hAlign", changed );
        setPropertyChanged( o, "vAlign", changed );
        setPropertyChanged( o, "wordwrap", changed );
        doUpdate = true;
    }
}

bool MetaDataBase::isPropertyChanged( QObject *o, const QString &property )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return false;
    }

    return r->changedProperties.findIndex( property ) != -1;
}

QStringList MetaDataBase::changedProperties( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return QStringList();
    }

    QStringList lst( r->changedProperties );
    return lst;
}

void MetaDataBase::setPropertyComment( QObject *o, const QString &property, const QString &comment )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    r->propertyComments.insert( property, comment );
}

QString MetaDataBase::propertyComment( QObject *o, const QString &property )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return QString();
    }

    return *r->propertyComments.find( property );
}

void MetaDataBase::setFakeProperty( QObject *o, const QString &property, const QVariant& value )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }
    r->fakeProperties[property] = value;
}

QVariant MetaDataBase::fakeProperty( QObject * o, const QString &property)
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return QVariant();
    }
    QMap<QString, QVariant>::Iterator it = r->fakeProperties.find( property );
    if ( it != r->fakeProperties.end() )
        return r->fakeProperties[property];
    return WidgetFactory::defaultValue( o, property );

}

QMap<QString,QVariant>* MetaDataBase::fakeProperties( QObject* o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return 0;
    }
    return &r->fakeProperties;
}

void MetaDataBase::setSpacing( QObject *o, int spacing )
{
    if ( !o )
        return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    r->spacing = spacing;
    QLayout * layout = 0;
    WidgetFactory::layoutType( (QWidget*)o, layout );
    if ( layout )
        layout->setSpacing( spacing );
}

int MetaDataBase::spacing( QObject *o )
{
    if ( !o )
        return -1;
    setupDataBase();
    if ( o->inherits( "QMainWindow" ) )
        o = ( (Q3MainWindow*)o )->centralWidget();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return -1;
    }

    return r->spacing;
}

void MetaDataBase::setMargin( QObject *o, int margin )
{
    if ( !o )
        return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    r->margin = margin;
    QLayout * layout = 0;
    WidgetFactory::layoutType( (QWidget*)o, layout );
    if ( margin < 1 )
        margin = 1;
    if ( layout )
        layout->setMargin( margin );
}

int MetaDataBase::margin( QObject *o )
{
    if ( !o )
        return -1;
    setupDataBase();
    if ( o->inherits( "QMainWindow" ) )
        o = ( (Q3MainWindow*)o )->centralWidget();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return -1;
    }

    return r->margin;
}

void MetaDataBase::addConnection( QObject *o, QObject *sender, const Q3CString &signal,
                                  QObject *receiver, const Q3CString &slot )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }
    Connection conn;
    conn.sender = sender;
    conn.signal = signal;
    conn.receiver = receiver;
    conn.slot = slot;
    r->connections.append( conn );
}

void MetaDataBase::removeConnection( QObject *o, QObject *sender, const Q3CString &signal,
                                     QObject *receiver, const Q3CString &slot )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    for ( Q3ValueList<Connection>::Iterator it = r->connections.begin(); it != r->connections.end(); ++it ) {
        Connection conn = *it;
        if ( conn.sender == sender &&
             conn.signal == signal &&
             conn.receiver == receiver &&
             conn.slot == slot ) {
            r->connections.remove( it );
            break;
        }
    }
}

Q3ValueList<MetaDataBase::Connection> MetaDataBase::connections( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return Q3ValueList<Connection>();
    }
    return r->connections;
}

Q3ValueList<MetaDataBase::Connection> MetaDataBase::connections( QObject *o, QObject *sender,
                                                                QObject *receiver )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return Q3ValueList<Connection>();
    }
    Q3ValueList<Connection>::Iterator it = r->connections.begin();
    Q3ValueList<Connection> ret;
    Q3ValueList<Connection>::Iterator conn;
    while ( ( conn = it ) != r->connections.end() ) {
        ++it;
        if ( (*conn).sender == sender &&
             (*conn).receiver == receiver )
            ret << *conn;
    }

    return ret;
}

Q3ValueList<MetaDataBase::Connection> MetaDataBase::connections( QObject *o, QObject *object )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return Q3ValueList<Connection>();
    }
    Q3ValueList<Connection>::Iterator it = r->connections.begin();
    Q3ValueList<Connection> ret;
    Q3ValueList<Connection>::Iterator conn;
    while ( ( conn = it ) != r->connections.end() ) {
        ++it;
        if ( (*conn).sender == object ||
             (*conn).receiver == object )
            ret << *conn;
    }
    return ret;
}

void MetaDataBase::doConnections( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    QObject *sender = 0, *receiver = 0;
    QObjectList *l = 0;
    Q3ValueList<Connection>::Iterator it = r->connections.begin();
    for ( ; it != r->connections.end(); ++it ) {
        Connection conn = *it;
        if ( qstrcmp( conn.sender->name(), o->name() ) == 0 ) {
            sender = o;
        } else {
            l = o->queryList( 0, conn.sender->name(), false );
            if ( !l || !l->first() ) {
                delete l;
                continue;
            }
            sender = l->first();
            delete l;
        }
        if ( qstrcmp( conn.receiver->name(), o->name() ) == 0 ) {
            receiver = o;
        } else {
            l = o->queryList( 0, conn.receiver->name(), false );
            if ( !l || !l->first() ) {
                delete l;
                continue;
            }
            receiver = l->first();
            delete l;
        }
        QString s = "2""%1";
        s = s.arg( conn.signal );
        QString s2 = "1""%1";
        s2 = s2.arg( conn.slot );

        Q3StrList signalList = sender->metaObject()->signalNames( true );
        Q3StrList slotList = receiver->metaObject()->slotNames( true );

        // avoid warnings
        if ( signalList.find( conn.signal ) == -1 ||
             slotList.find( conn.slot ) == -1 )
            continue;

        QObject::connect( sender, s, receiver, s2 );
    }
}

void MetaDataBase::addSlot( QObject *o, const Q3CString &slot, const QString& specifier,
                            const QString &access, const QString &language, const QString &returnType )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    Slot s;
    s.slot = slot;
    s.specifier = specifier;
    s.access = access;
    s.language = language;
    s.returnType = returnType;
    Q3ValueList<MetaDataBase::Slot>::Iterator it = r->slotList.find( s );
    if ( it != r->slotList.end() )
        r->slotList.remove( it );
    r->slotList.append( s );
#ifndef KOMMANDER
    ( (FormWindow*)o )->formFile()->addSlotCode( s );
#endif
}

void MetaDataBase::setSlotList( QObject *o, const Q3ValueList<Slot> &slotList )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }
    r->slotList = slotList;
}

void MetaDataBase::removeSlot( QObject *o, const Q3CString &slot, const QString& specifier,
                               const QString &access, const QString &language, const QString &returnType )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    for ( Q3ValueList<Slot>::Iterator it = r->slotList.begin(); it != r->slotList.end(); ++it ) {
        Slot s = *it;
        if ( s.slot == slot &&
             s.specifier == specifier &&
             s.access == access &&
             ( language.isEmpty() || s.language == language ) &&
               ( returnType.isEmpty() || s.returnType == returnType ) ) {
            r->slotList.remove( it );
            break;
        }
    }
}

void MetaDataBase::removeSlot( QObject *o, const QString &slot )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    for ( Q3ValueList<Slot>::Iterator it = r->slotList.begin(); it != r->slotList.end(); ++it ) {
        Slot s = *it;
        if ( normalizeSlot( s.slot ) == normalizeSlot( slot ) ) {
            r->slotList.remove( it );
            break;
        }
    }
}

Q3ValueList<MetaDataBase::Slot> MetaDataBase::slotList( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return Q3ValueList<Slot>();
    }

    return r->slotList;
}

bool MetaDataBase::isSlotUsed( QObject *o, const Q3CString &slot )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return false;
    }

    Q3ValueList<Connection> conns = connections( o );
    for ( Q3ValueList<Connection>::Iterator it = conns.begin(); it != conns.end(); ++it ) {
        if ( (*it).slot == slot )
            return true;
    }
    return false;
}

void MetaDataBase::changeSlot( QObject *o, const Q3CString &slot, const Q3CString &newName )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    for ( Q3ValueList<Slot>::Iterator it = r->slotList.begin(); it != r->slotList.end(); ++it ) {
        Slot s = *it;
        if ( normalizeSlot( s.slot ) == normalizeSlot( slot ) ) {
            (*it).slot = newName;
            return;
        }
    }
}

void MetaDataBase::changeSlotAttributes( QObject *o, const Q3CString &slot,
                                         const QString& specifier, const QString &access,
                                         const QString &language, const QString &returnType )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    for ( Q3ValueList<Slot>::Iterator it = r->slotList.begin(); it != r->slotList.end(); ++it ) {
        Slot s = *it;
        if ( normalizeSlot( s.slot ) == normalizeSlot( slot ) ) {
            (*it).specifier = specifier;
            (*it).access = access;
            (*it).language = language;
            (*it).returnType = returnType;
            return;
        }
    }
}

bool MetaDataBase::hasSlot( QObject *o, const Q3CString &slot, bool onlyCustom )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return false;
    }

    if ( !onlyCustom ) {
        Q3StrList slotList = o->metaObject()->slotNames( true );
        if ( slotList.find( slot ) != -1 )
            return true;

        if ( o->inherits( "FormWindow" ) ) {
            o = ( (FormWindow*)o )->mainContainer();
            slotList = o->metaObject()->slotNames( true );
            if ( slotList.find( slot ) != -1 )
                return true;
        }

        if ( o->inherits( "CustomWidget" ) ) {
            MetaDataBase::CustomWidget *w = ( (::CustomWidget*)o )->customWidget();
            for ( Q3ValueList<Slot>::Iterator it = w->lstSlots.begin(); it != w->lstSlots.end(); ++it ) {
                Q3CString s = (*it).slot;
                if ( !s.data() )
                    continue;
                if ( s == slot )
                    return true;
            }
        }
    }

    for ( Q3ValueList<Slot>::Iterator it = r->slotList.begin(); it != r->slotList.end(); ++it ) {
        Slot s = *it;
        if ( normalizeSlot( s.slot ) == normalizeSlot( slot ) )
            return true;
    }

    return false;
}

QString MetaDataBase::languageOfSlot( QObject *o, const Q3CString &slot )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return QString();
    }

    QString sl = slot;
    sl = normalizeSlot( sl );
    for ( Q3ValueList<Slot>::Iterator it = r->slotList.begin(); it != r->slotList.end(); ++it ) {
        Slot s = *it;
        QString sl2 = s.slot;
        sl2 = normalizeSlot( sl2 );
        if ( sl == sl2 )
            return s.language;
    }
    return QString();
}

bool MetaDataBase::addCustomWidget( CustomWidget *wid )
{
    setupDataBase();

    for ( CustomWidget *w = cWidgets->first(); w; w = cWidgets->next() ) {
        if ( *wid == *w ) {
            for ( Q3ValueList<Q3CString>::ConstIterator it = wid->lstSignals.begin(); it != wid->lstSignals.end(); ++it ) {
                if ( !w->hasSignal( *it ) )
                    w->lstSignals.append( *it );
            }
            for ( Q3ValueList<Slot>::ConstIterator it2 = wid->lstSlots.begin(); it2 != wid->lstSlots.end(); ++it2 ) {
                if ( !w->hasSlot( MetaDataBase::normalizeSlot( (*it2).slot ).toLatin1() ) )
                    w->lstSlots.append( *it2 );
            }
            for ( Q3ValueList<Property>::ConstIterator it3 = wid->lstProperties.begin(); it3 != wid->lstProperties.end(); ++it3 ) {
                if ( !w->hasProperty( (*it3).property ) )
                    w->lstProperties.append( *it3 );
            }
            delete wid;
            return false;
        }
    }


    WidgetDatabaseRecord *r = new WidgetDatabaseRecord;
    r->name = wid->className;
    r->group = WidgetDatabase::widgetGroup( "Custom" );
    r->toolTip = wid->className;
    r->icon = new QIcon( *wid->pixmap, *wid->pixmap );
    r->isContainer = wid->isContainer;
    wid->id = WidgetDatabase::addCustomWidget( r );
    cWidgets->append( wid );
    return true;
}

void MetaDataBase::removeCustomWidget( CustomWidget *w )
{
    cWidgets->removeRef( w );
}

Q3PtrList<MetaDataBase::CustomWidget> *MetaDataBase::customWidgets()
{
    setupDataBase();
    return cWidgets;
}

MetaDataBase::CustomWidget *MetaDataBase::customWidget( int id )
{
    for ( CustomWidget *w = cWidgets->first(); w; w = cWidgets->next() ) {
        if ( id == w->id )
            return w;
    }
    return 0;
}

bool MetaDataBase::isWidgetNameUsed( CustomWidget *wid )
{
    for ( CustomWidget *w = cWidgets->first(); w; w = cWidgets->next() ) {
        if ( w == wid )
            continue;
        if ( wid->className == w->className )
            return true;
    }
    return false;
}

bool MetaDataBase::hasCustomWidget( const QString &className )
{
    for ( CustomWidget *w = cWidgets->first(); w; w = cWidgets->next() ) {
        if ( w->className == className )
            return true;
    }
    return false;
}

void MetaDataBase::setTabOrder( QWidget *w, const QWidgetList &order )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*) w );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  w, w->name(), w->className() );
        return;
    }

    r->tabOrder = order;
}

QWidgetList MetaDataBase::tabOrder( QWidget *w )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*) w );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  w, w->name(), w->className() );
        return QWidgetList();
    }

    return r->tabOrder;
}

void MetaDataBase::setIncludes( QObject *o, const Q3ValueList<Include> &incs )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    r->includes = incs;
}

Q3ValueList<MetaDataBase::Include> MetaDataBase::includes( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return Q3ValueList<Include>();
    }

    return r->includes;
}

void MetaDataBase::setForwards( QObject *o, const QStringList &fwds )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    r->forwards = fwds;
}

QStringList MetaDataBase::forwards( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return QStringList();
    }

    return r->forwards;
}

void MetaDataBase::setSignalList( QObject *o, const QStringList &sigs )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    r->sigs.clear();

    for ( QStringList::ConstIterator it = sigs.begin(); it != sigs.end(); ++it ) {
        QString s = (*it).simplified();
        bool hasSemicolon = s.endsWith( ";" );
        if ( hasSemicolon )
            s = s.left( s.length() - 1 );
        int p = s.find( '(' );
        if ( p < 0 )
            p = s.length();
        int sp = s.find( ' ' );
        if ( sp >= 0 && sp < p ) {
            s = s.mid( sp+1 );
            p -= sp + 1;
        }
        if ( p == (int) s.length() )
            s += "()";
        if ( hasSemicolon )
            s += ";";
        r->sigs << s;
    }
}

QStringList MetaDataBase::signalList( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return QStringList();
    }

    return r->sigs;
}

void MetaDataBase::setMetaInfo( QObject *o, MetaInfo mi )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    r->metaInfo = mi;
}

MetaDataBase::MetaInfo MetaDataBase::metaInfo( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return MetaInfo();
    }

    return r->metaInfo;
}




MetaDataBase::CustomWidget::CustomWidget()
{
    className = "MyCustomWidget";
    includeFile = "mywidget.h";
    includePolicy = Local;
    sizeHint = QSize( -1, -1 );
    pixmap = new QPixmap( PixmapChooser::loadPixmap( "customwidget.xpm" ) );
    id = -1;
    sizePolicy = QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    isContainer = false;
}

MetaDataBase::CustomWidget::CustomWidget( const CustomWidget &w )
{
    className = w.className;
    includeFile = w.includeFile;
    includePolicy = w.includePolicy;
    sizeHint = w.sizeHint;
    if ( w.pixmap )
        pixmap = new QPixmap( *w.pixmap );
    else
        pixmap = 0;
    id = w.id;
    isContainer = w.isContainer;
}

void MetaDataBase::setCursor( QWidget *w, const QCursor &c )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)w );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  w, w->name(), w->className() );
        return;
    }

    r->cursor = c;
}

QCursor MetaDataBase::cursor( QWidget *w )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)w );
    if ( !r ) {
        w->unsetCursor();
        return w->cursor();
    }

    return r->cursor;
}

bool MetaDataBase::CustomWidget::operator==( const CustomWidget &w ) const
{
    return className == w.className;
}

MetaDataBase::CustomWidget &MetaDataBase::CustomWidget::operator=( const CustomWidget &w )
{
    delete pixmap;
    className = w.className;
    includeFile = w.includeFile;
    includePolicy = w.includePolicy;
    sizeHint = w.sizeHint;
    if ( w.pixmap )
        pixmap = new QPixmap( *w.pixmap );
    else
        pixmap = 0;
    lstSignals = w.lstSignals;
    lstSlots = w.lstSlots;
    lstProperties = w.lstProperties;
    id = w.id;
    isContainer = w.isContainer;
    return *this;
}

bool MetaDataBase::CustomWidget::hasSignal( const Q3CString &signal ) const
{
    Q3StrList sigList = QWidget::staticMetaObject()->signalNames( true );
    if ( sigList.find( signal ) != -1 )
        return true;
    for ( Q3ValueList<Q3CString>::ConstIterator it = lstSignals.begin(); it != lstSignals.end(); ++it ) {
        if ( normalizeSlot( *it ) == normalizeSlot( signal ) )
            return true;
    }
    return false;
}

bool MetaDataBase::CustomWidget::hasSlot( const Q3CString &slot ) const
{
    Q3StrList slotList = QWidget::staticMetaObject()->slotNames( true );
    if ( slotList.find( normalizeSlot( slot ) ) != -1 )
        return true;

    for ( Q3ValueList<MetaDataBase::Slot>::ConstIterator it = lstSlots.begin(); it != lstSlots.end(); ++it ) {
        if ( normalizeSlot( (*it).slot ) == normalizeSlot( slot ) )
            return true;
    }
    return false;
}

bool MetaDataBase::CustomWidget::hasProperty( const Q3CString &prop ) const
{
    Q3StrList propList = QWidget::staticMetaObject()->propertyNames( true );
    if ( propList.find( prop ) != -1 )
        return true;

    for ( Q3ValueList<MetaDataBase::Property>::ConstIterator it = lstProperties.begin(); it != lstProperties.end(); ++it ) {
        if ( (*it).property == prop )
            return true;
    }
    return false;
}

void MetaDataBase::setPixmapArgument( QObject *o, int pixmap, const QString &arg )
{
    if ( !o )
        return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    r->pixmapArguments.remove( pixmap );
    r->pixmapArguments.insert( pixmap, arg );
}

QString MetaDataBase::pixmapArgument( QObject *o, int pixmap )
{
    if ( !o )
        return QString();
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return QString();
    }

    return *r->pixmapArguments.find( pixmap );
}

void MetaDataBase::clearPixmapArguments( QObject *o )
{
    if ( !o )
        return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    r->pixmapArguments.clear();
}


void MetaDataBase::setPixmapKey( QObject *o, int pixmap, const QString &arg )
{
    if ( !o )
        return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    r->pixmapKeys.remove( pixmap );
    r->pixmapKeys.insert( pixmap, arg );
}

QString MetaDataBase::pixmapKey( QObject *o, int pixmap )
{
    if ( !o )
        return QString();
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return QString();
    }

    return *r->pixmapKeys.find( pixmap );
}

void MetaDataBase::clearPixmapKeys( QObject *o )
{
    if ( !o )
        return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    r->pixmapKeys.clear();
}



void MetaDataBase::setColumnFields( QObject *o, const QMap<QString, QString> &columnFields )
{
    if ( !o )
        return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    r->columnFields = columnFields;
}

QMap<QString, QString> MetaDataBase::columnFields( QObject *o )
{
    if ( !o )
        return QMap<QString, QString>();
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return QMap<QString, QString>();
    }

    return r->columnFields;
}

QString MetaDataBase::normalizeSlot( const QString &s )
{
    return Parser::cleanArgs( s );
}

void MetaDataBase::clear( QObject *o )
{
    if ( !o )
        return;
    setupDataBase();
    db->remove( (void*)o );
    for ( Q3PtrDictIterator<QWidget> it( *( (FormWindow*)o )->widgets() ); it.current(); ++it )
        db->remove( (void*)it.current() );
}

void MetaDataBase::setExportMacro( QObject *o, const QString &macro )
{
    if ( !o )
        return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return;
    }

    r->exportMacro = macro;
}

QString MetaDataBase::exportMacro( QObject *o )
{
    if ( !o )
        return "";
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
        qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
                  o, o->name(), o->className() );
        return "";
    }

    return r->exportMacro;
}

bool MetaDataBase::hasObject( QObject *o )
{
    return !!db->find( o );
}

