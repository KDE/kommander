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

#ifndef RESOURCE_H
#define RESOURCE_H

#include <qstring.h>
#include <qtextstream.h>
#include <qvariant.h>
#include <qvaluelist.h>
#include <qimage.h>
#include "actiondnd.h"

#include "metadatabase.h"

class QWidget;
class QObject;
class QLayout;
class QStyle;
class QPalette;
class FormWindow;
class MainWindow;
class QDomElement;
class QDesignerGridLayout;
class QListViewItem;
class QMainWindow;
#ifndef KOMMANDER
struct LanguageInterface;
#endif
class FormFile;

class Resource
{
public:
    struct Image {
	QImage img;
	QString name;
	bool operator==(  const Image &i ) const {
	    return ( i.name == name &&
		     i.img == img );
	}
    };

    Resource();
    Resource( MainWindow* mw );
    ~Resource();

    void setWidget( FormWindow *w );
    QWidget *widget() const;

    bool load( FormFile *ff );
    bool load( FormFile *ff, QIODevice* );
    QString copy();

    bool save( const QString& filename, bool formCodeOnly = FALSE);
    bool save( QIODevice* );
    void paste( const QString &cb, QWidget *parent );

    static void saveImageData( const QImage &img, QTextStream &ts, int indent );
    static void loadCustomWidgets( const QDomElement &e, Resource *r );

private:
    void saveObject( QObject *obj, QDesignerGridLayout* grid, QTextStream &ts, int indent );
    void saveChildrenOf( QObject* obj, QTextStream &ts, int indent );
    void saveObjectProperties( QObject *w, QTextStream &ts, int indent );
    void saveSetProperty( QObject *w, const QString &name, QVariant::Type t, QTextStream &ts, int indent );
    void saveEnumProperty( QObject *w, const QString &name, QVariant::Type t, QTextStream &ts, int indent );
    void saveProperty( QObject *w, const QString &name, const QVariant &value, QVariant::Type t, QTextStream &ts, int indent );
    void saveProperty( const QVariant &value, QTextStream &ts, int indent );
    void saveItems( QObject *obj, QTextStream &ts, int indent );
    void saveItem( const QStringList &text, const QPtrList<QPixmap> &pixmaps, QTextStream &ts, int indent );
    void saveItem( QListViewItem *i, QTextStream &ts, int indent );
    void saveConnections( QTextStream &ts, int indent );
    void saveCustomWidgets( QTextStream &ts, int indent );
    void saveTabOrder( QTextStream &ts, int indent );
    void saveColorGroup( QTextStream &ts, int indent, const QColorGroup &cg );
    void saveColor( QTextStream &ts, int indent, const QColor &c );
    void saveMetaInfoBefore( QTextStream &ts, int indent );
    void saveMetaInfoAfter( QTextStream &ts, int indent );
    void savePixmap( const QPixmap &p, QTextStream &ts, int indent, const QString &tagname = "pixmap" );
    void saveActions( const QPtrList<QAction> &actions, QTextStream &ts, int indent );
    void saveChildActions( QAction *a, QTextStream &ts, int indent );
    void saveToolBars( QMainWindow *mw, QTextStream &ts, int indent );
    void saveMenuBar( QMainWindow *mw, QTextStream &ts, int indent );
#ifndef KOMMANDER
//    void saveFormCode();
#endif

    QObject *createObject( const QDomElement &e, QWidget *parent, QLayout* layout = 0 );
    QWidget *createSpacer( const QDomElement &e, QWidget *parent, QLayout *layout, Qt::Orientation o );
    void createItem( const QDomElement &e, QWidget *widget, QListViewItem *i = 0 );
    void createColumn( const QDomElement &e, QWidget *widget );
    void setObjectProperty( QObject* widget, const QString &prop, const QDomElement &e);
    QString saveInCollection( const QImage &img );
    QString saveInCollection( const QPixmap &pix ) { return saveInCollection( pix.convertToImage() ); }
    QImage loadFromCollection( const QString &name );
    void saveImageCollection( QTextStream &ts, int indent );
    void loadImageCollection( const QDomElement &e );
    void loadConnections( const QDomElement &e );
    void loadTabOrder( const QDomElement &e );
    void loadItem( const QDomElement &n, QPixmap &pix, QString &txt, bool &hasPixmap );
    void loadActions( const QDomElement &n );
    void loadChildAction( QObject *parent, const QDomElement &e );
    void loadToolBars( const QDomElement &n );
    void loadMenuBar( const QDomElement &n );
    QColorGroup loadColorGroup( const QDomElement &e );
    QPixmap loadPixmap( const QDomElement &e, const QString &tagname = "pixmap" );
#ifndef KOMMANDER
//    void loadFunctions( const QDomElement &e ); // compatibility with early 3.0 betas
//    void loadExtraSource();
#endif

private:
    MainWindow *mainwindow;
    FormWindow *formwindow;
    QWidget* toplevel;
    QValueList<Image> images;
    bool copying, pasting;
    bool mainContainerSet;
    QStringList knownNames;
    QStringList usedCustomWidgets;
    QListViewItem *lastItem;

    QValueList<MetaDataBase::Include> metaIncludes;
    QStringList metaForwards;
    QStringList metaVariables;
    QStringList metaSignals;
    MetaDataBase::MetaInfo metaInfo;
    QMap<QString, QString> dbControls;
    QMap<QString, QStringList> dbTables;
    QString exportMacro;
    bool hadGeometry;
    QMap<QString, QValueList<MetaDataBase::Connection> > langConnections;
    QString currFileName;
#ifndef KOMMANDER
    LanguageInterface *langIface;
#endif
    bool hasFunctions;

};

#endif
