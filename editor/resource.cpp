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

#include <qmenudata.h>
#include "resource.h"
#include "defs.h"
#include "metadatabase.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "qdom.h"
#include <widgetdatabase.h>
#include "widgetfactory.h"
#include "layout.h"
#include <domtool.h>
#include "command.h"
#include "pixmapchooser.h"
#ifndef QT_NO_SQL
#include "database.h"
#endif
#include "actiondnd.h"
#ifndef KOMMANDER
#include "project.h"
#include "pixmapcollection.h"
#endif
#include "formfile.h"

#include <qfeatures.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qobject.h>
#include <qwidget.h>
#include <qobjectlist.h>
#include <qmetaobject.h>
#include <qworkspace.h>
#include <qtabwidget.h>
#include <qapplication.h>
#include <qbuffer.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qtabwidget.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qwidgetstack.h>
#include <qtabbar.h>
#include <qheader.h>
#include <qlistview.h>
#include <qiconview.h>
#include <qlabel.h>
#include <qwizard.h>
#include <qtextcodec.h>
#include <qregexp.h>
#include <zlib.h>
#include <qdatetime.h>
#ifndef QT_NO_TABLE
#include <qtable.h>
#endif

static QString makeIndent( int indent )
{
    QString s;
    s.fill( ' ', indent * 4 );
    return s;
}

static QString entitize( const QString &s, bool attribute = FALSE )
{
    QString s2 = s;
    s2 = s2.replace( QRegExp( "&" ), "&amp;" );
    s2 = s2.replace( QRegExp( ">" ), "&gt;" );
    s2 = s2.replace( QRegExp( "<" ), "&lt;" );
    if ( attribute ) {
	s2 = s2.replace( QRegExp( "\"" ), "&quot;" );
	s2 = s2.replace( QRegExp( "'" ), "&apos;" );
    }
    return s2;
}

static QString mkBool( bool b )
{
    return b? "true" : "false";
}

/*!
  \class Resource resource.h
  \brief Class for saving/loading, etc. forms

  This class is used for saving and loading forms, code generation,
  transferring data of widgets over the clipboard, etc..

*/


Resource::Resource()
{
    mainwindow = 0;
    formwindow = 0;
    toplevel = 0;
    copying = FALSE;
    pasting = FALSE;
    hadGeometry = FALSE;
    langIface = 0;
    hasFunctions = FALSE;
}

Resource::Resource( MainWindow* mw )
    : mainwindow( mw )
{
    formwindow = 0;
    toplevel = 0;
    copying = FALSE;
    pasting = FALSE;
    hadGeometry = FALSE;
    langIface = 0;
    hasFunctions = FALSE;
}

Resource::~Resource()
{
    if ( langIface )
	langIface->release();
}

void Resource::setWidget( FormWindow *w )
{
    formwindow = w;
    toplevel = w;
}

QWidget *Resource::widget() const
{
    return toplevel;
}

bool Resource::load( FormFile *ff )
{
    if ( !ff || ff->absFileName().isEmpty() )
	return FALSE;
    currFileName = ff->absFileName();
    mainContainerSet = FALSE;

    QFile f( ff->absFileName() );
    f.open( IO_ReadOnly );

    bool b = load( ff, &f );
    f.close();

    return b;
}

#undef signals
#undef slots

bool Resource::load( FormFile *ff, QIODevice* dev )
{
    QDomDocument doc;
    QString errMsg;
    int errLine;
    if ( !doc.setContent( dev, &errMsg, &errLine ) ) {
	qDebug( QString("Parse error: ") + errMsg + QString(" in line %d"), errLine );
	return FALSE;
    }

    DomTool::fixDocument( doc );

    toplevel = formwindow = new FormWindow( ff, mainwindow->qWorkspace(), 0 );
#ifndef KOMMANDER
    formwindow->setProject( MainWindow::self->currProject() );
#endif
    formwindow->setMainWindow( mainwindow );
    MetaDataBase::addEntry( formwindow );

    if (!langIface) {
#ifndef KOMMANDER
	langIface = MetaDataBase::languageInterface( mainwindow->currProject()->language() );
#else
	langIface = MetaDataBase::languageInterface( "C++" );
#endif
	if ( langIface )
	    langIface->addRef();
    }

    QDomElement e = doc.firstChild().toElement().firstChild().toElement();

    QDomElement forwards = e;
    while ( forwards.tagName() != "forwards" && !forwards.isNull() )
	forwards = forwards.nextSibling().toElement();

    QDomElement includes = e;
    while ( includes.tagName() != "includes" && !includes.isNull() )
	includes = includes.nextSibling().toElement();

    QDomElement variables = e;
    while ( variables.tagName() != "variables" && !variables.isNull() )
	variables = variables.nextSibling().toElement();

    QDomElement signals = e;
    while ( signals.tagName() != "signals" && !signals.isNull() )
	signals = signals.nextSibling().toElement();

    QDomElement slots = e;
    while ( slots.tagName() != "slots" && !slots.isNull() )
	slots = slots.nextSibling().toElement();

    QDomElement connections = e;
    while ( connections.tagName() != "connections" && !connections.isNull() )
	connections = connections.nextSibling().toElement();

    QDomElement imageCollection = e;
    images.clear();
    while ( imageCollection.tagName() != "images" && !imageCollection.isNull() )
	imageCollection = imageCollection.nextSibling().toElement();

    QDomElement customWidgets = e;
    while ( customWidgets.tagName() != "customwidgets" && !customWidgets.isNull() )
	customWidgets = customWidgets.nextSibling().toElement();

    QDomElement tabOrder = e;
    while ( tabOrder.tagName() != "tabstops" && !tabOrder.isNull() )
	tabOrder = tabOrder.nextSibling().toElement();

    QDomElement actions = e;
    while ( actions.tagName() != "actions" && !actions.isNull() )
	actions = actions.nextSibling().toElement();

    QDomElement toolbars = e;
    while ( toolbars.tagName() != "toolbars" && !toolbars.isNull() )
	toolbars = toolbars.nextSibling().toElement();

    QDomElement menubar = e;
    while ( menubar.tagName() != "menubar" && !menubar.isNull() )
	menubar = menubar.nextSibling().toElement();

    QDomElement functions = e;
    while ( functions.tagName() != "functions" && !functions.isNull() )
	functions = functions.nextSibling().toElement();


    QDomElement widget;
    while ( !e.isNull() ) {
	if ( e.tagName() == "widget" ) {
	    widget = e;
	} else if ( e.tagName() == "include" ) { // compatibility with 2.x
	    MetaDataBase::Include inc;
	    inc.location = "global";
	    if ( e.attribute( "location" ) == "local" )
		inc.location = "local";
	    inc.implDecl = "in declaration";
	    if ( e.attribute( "impldecl" ) == "in implementation" )
		inc.implDecl = "in implementation";
	    inc.header = e.firstChild().toText().data();
	    if ( inc.header.right( 5 ) != ".ui.h" )
		metaIncludes.append( inc );
	} else if ( e.tagName() == "comment" ) {
	    metaInfo.comment = e.firstChild().toText().data();
	} else if ( e.tagName() == "forward" ) { // compatibility with old betas
	    metaForwards << e.firstChild().toText().data();
	} else if ( e.tagName() == "variable" ) { // compatibility with old betas
	    metaVariables << e.firstChild().toText().data();
	} else if ( e.tagName() == "author" ) {
	    metaInfo.author = e.firstChild().toText().data();
	} else if ( e.tagName() == "class" ) {
	    metaInfo.className = e.firstChild().toText().data();
	} else if ( e.tagName() == "pixmapfunction" ) {
	    if ( formwindow ) {
		formwindow->setSavePixmapInline( FALSE );
#ifndef KOMMANDER
		formwindow->setSavePixmapInProject( FALSE );
#endif
		formwindow->setPixmapLoaderFunction( e.firstChild().toText().data() );
	    }
	} else if ( e.tagName() == "pixmapinproject" ) {
	    if ( formwindow ) {
		formwindow->setSavePixmapInline( FALSE );
#ifndef KOMMANDER
		formwindow->setSavePixmapInProject( TRUE );
#endif
	    }
	} else if ( e.tagName() == "exportmacro" ) {
	    exportMacro = e.firstChild().toText().data();
	} else if ( e.tagName() == "layoutdefaults" ) {
	    formwindow->setLayoutDefaultSpacing( e.attribute( "spacing", QString::number( formwindow->layoutDefaultSpacing() ) ).toInt() );
	    formwindow->setLayoutDefaultMargin( e.attribute( "margin", QString::number( formwindow->layoutDefaultMargin() ) ).toInt() );
	}

	e = e.nextSibling().toElement();
    }

    if ( !imageCollection.isNull() )
	loadImageCollection( imageCollection );
    if ( !customWidgets.isNull() )
	loadCustomWidgets( customWidgets, this );

#if defined (QT_NON_COMMERCIAL)
    bool previewMode = MainWindow::self->isPreviewing();
    QWidget *w = (QWidget*)createObject( widget, !previewMode ? (QWidget*)formwindow : MainWindow::self);
    if ( !w )
	return FALSE;
    if ( previewMode )
	w->reparent( MainWindow::self, Qt::WType_TopLevel,  w->pos(), TRUE );	
#else
    if ( !createObject( widget, formwindow) )
	return FALSE;
#endif

    if ( !forwards.isNull() ) {
	for ( QDomElement n = forwards.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
	    if ( n.tagName() == "forward" )
		metaForwards << n.firstChild().toText().data();
    }

    if ( !includes.isNull() ) {
	for ( QDomElement n = includes.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
	    if ( n.tagName() == "include" ) {
		if ( n.tagName() == "include" ) {
		    MetaDataBase::Include inc;
		    inc.location = "global";
		    if ( n.attribute( "location" ) == "local" )
			inc.location = "local";
		    inc.implDecl = "in declaration";
		    if ( n.attribute( "impldecl" ) == "in implementation" )
			inc.implDecl = "in implementation";
		    inc.header = n.firstChild().toText().data();
		    if ( inc.header.right( 5 ) != ".ui.h" )
			metaIncludes.append( inc );
		}
	    }
    }

    if ( !variables.isNull() ) {
	for ( QDomElement n = variables.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
	    if ( n.tagName() == "variable" )
		metaVariables << n.firstChild().toText().data();
    }
    if ( !signals.isNull() ) {
	for ( QDomElement n = signals.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
	    if ( n.tagName() == "signal" )
		metaSignals << n.firstChild().toText().data();
    }
    if ( !slots.isNull() ) {
	for ( QDomElement n = slots.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
	    if ( n.tagName() == "slot" ) {
		MetaDataBase::Slot slot;
		slot.specifier = n.attribute( "specifier", "virtual" );
		if ( slot.specifier.isEmpty() )
		    slot.specifier = "virtual";
		slot.access = n.attribute( "access", "public" );
		if ( slot.access.isEmpty() )
		    slot.access = "public";
		slot.language = n.attribute( "language", "C++" );
		slot.returnType = n.attribute( "returnType", "void" );
		if ( slot.returnType.isEmpty() )
		    slot.returnType = "void";
		slot.slot = n.firstChild().toText().data();
		if ( !MetaDataBase::hasSlot( formwindow, slot.slot, TRUE ) )
		    MetaDataBase::addSlot( formwindow, slot.slot, slot.specifier,
					   slot.access, slot.language, slot.returnType );
		else
		    MetaDataBase::changeSlotAttributes( formwindow, slot.slot,
							slot.specifier, slot.access,
							slot.language, slot.returnType );
	    }
    }
    if ( !actions.isNull() )
	loadActions( actions );
    if ( !toolbars.isNull() )
	loadToolBars( toolbars );
    if ( !menubar.isNull() )
	loadMenuBar( menubar );

    if ( !connections.isNull() )
	loadConnections( connections );
    if ( !functions.isNull() )
	loadFunctions( functions );
    if ( !tabOrder.isNull() )
	loadTabOrder( tabOrder );

    if ( formwindow ) {
	MetaDataBase::setIncludes( formwindow, metaIncludes );
	MetaDataBase::setForwards( formwindow, metaForwards );
	MetaDataBase::setVariables( formwindow, metaVariables );
	MetaDataBase::setSignalList( formwindow, metaSignals );
	metaInfo.classNameChanged = metaInfo.className != QString( formwindow->name() );
	MetaDataBase::setMetaInfo( formwindow, metaInfo );
	MetaDataBase::setExportMacro( formwindow->mainContainer(), exportMacro );
    }

    loadExtraSource();

    if ( mainwindow && formwindow )
    {
	mainwindow->insertFormWindow( formwindow );
    }

    if ( formwindow ) {
	formwindow->killAccels( formwindow );
	if ( formwindow->layout() )
	    formwindow->layout()->activate();
	if ( hadGeometry )
	    formwindow->resize( formwindow->size().expandedTo( formwindow->minimumSize().
							       expandedTo( formwindow->minimumSizeHint() ) ) );
	else
	    formwindow->resize( formwindow->size().expandedTo( formwindow->sizeHint() ) );
    }

    return TRUE;
}

bool Resource::save( const QString& filename, bool formCodeOnly )
{
    if ( !formwindow || filename.isEmpty() )
	return FALSE;
    if (!langIface) {
#ifndef KOMMANDER
	    langIface = MetaDataBase::languageInterface( mainwindow->self->currProject()->language() );
#else
	    langIface = MetaDataBase::languageInterface( "C++" );
#endif
	if ( langIface )
	    langIface->addRef();
    }
    if ( formCodeOnly && langIface && langIface->supports( LanguageInterface::StoreFormCodeSeperate ) ) {
	saveFormCode();
	return TRUE; // missing error checking in saveFormCode ?
    }
    currFileName = filename;

    QFile f( filename );
    if ( !f.open( IO_WriteOnly | IO_Translate ) )
	return FALSE;
    bool b = save( &f );
    f.close();
    return b;
}

bool Resource::save( QIODevice* dev )
{
    if ( !formwindow )
	return FALSE;

    if (!langIface) {
#ifndef KOMMANDER
	    langIface = MetaDataBase::languageInterface( mainwindow->self->currProject()->language() );
#else
	    langIface = MetaDataBase::languageInterface( "C++" );
#endif
	if ( langIface )
	    langIface->addRef();
    }

    QTextStream ts( dev );
    ts.setCodec( QTextCodec::codecForName( "UTF-8" ) );

    ts << "<!DOCTYPE UI><UI version=\"3.0\" stdsetdef=\"1\">" << endl;
    saveMetaInfoBefore( ts, 0 );
    saveObject( formwindow->mainContainer(), 0, ts, 0 );
    if ( formwindow->mainContainer()->inherits( "QMainWindow" ) ) {
	saveMenuBar( (QMainWindow*)formwindow->mainContainer(), ts, 0 );
	saveToolBars( (QMainWindow*)formwindow->mainContainer(), ts, 0 );
    }
    if ( !MetaDataBase::customWidgets()->isEmpty() && !usedCustomWidgets.isEmpty() )
	saveCustomWidgets( ts, 0 );
    if ( formwindow->mainContainer()->inherits( "QMainWindow" ) )
	saveActions( formwindow->actionList(), ts, 0 );
    if ( !images.isEmpty() )
	saveImageCollection( ts, 0 );
    if ( !MetaDataBase::connections( formwindow ).isEmpty() || !MetaDataBase::slotList( formwindow ).isEmpty() )
	saveConnections( ts, 0 );
    saveTabOrder( ts, 0 );
    saveMetaInfoAfter( ts, 0 );
    ts << "</UI>" << endl;
    saveFormCode();
    images.clear();

    return TRUE;
}

QString Resource::copy()
{
    if ( !formwindow )
	return QString::null;

    copying = TRUE;
    QString s;
    QTextOStream ts( &s );

    ts << "<!DOCTYPE UI-SELECTION><UI-SELECTION>" << endl;
    QWidgetList widgets = formwindow->selectedWidgets();
    QWidgetList tmp( widgets );
    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	QWidget *p = w->parentWidget();
	bool save = TRUE;
	while ( p ) {
	    if ( tmp.findRef( p ) != -1 ) {
		save = FALSE;
		break;
	    }
	    p = p->parentWidget();
	}
	if ( save )
	    saveObject( w, 0, ts, 0 );
    }
    if ( !MetaDataBase::customWidgets()->isEmpty() && !usedCustomWidgets.isEmpty() )
	saveCustomWidgets( ts, 0 );
    if ( !images.isEmpty() )
	saveImageCollection( ts, 0 );
    ts << "</UI-SELECTION>" << endl;

    return s;
}


void Resource::paste( const QString &cb, QWidget *parent )
{
    if ( !formwindow )
	return;
    mainContainerSet = TRUE;

    pasting = TRUE;
    QBuffer buf( QCString( cb.utf8() ) );
    buf.open( IO_ReadOnly );
    QDomDocument doc;
    QString errMsg;
    int errLine;
    if ( !doc.setContent( &buf, &errMsg, &errLine ) ) {
	qDebug( QString("Parse error: ") + errMsg + QString(" in line %d"), errLine );
    }

    QDomElement firstWidget = doc.firstChild().toElement().firstChild().toElement();

    QDomElement imageCollection = firstWidget;
    images.clear();
    while ( imageCollection.tagName() != "images" && !imageCollection.isNull() )
	imageCollection = imageCollection.nextSibling().toElement();

    QDomElement customWidgets = firstWidget;
    while ( customWidgets.tagName() != "customwidgets" && !customWidgets.isNull() )
	customWidgets = customWidgets.nextSibling().toElement();

    if ( !imageCollection.isNull() )
	loadImageCollection( imageCollection );
    if ( !customWidgets.isNull() )
	loadCustomWidgets( customWidgets, this );

    QWidgetList widgets;
    formwindow->clearSelection( FALSE );
    formwindow->setPropertyShowingBlocked( TRUE );
    formwindow->clearSelection( FALSE );
    while ( !firstWidget.isNull() ) {
	if ( firstWidget.tagName() == "widget" ) {
	    QWidget *w = (QWidget*)createObject( firstWidget, parent, 0 );
	    if ( !w )
		continue;
	    widgets.append( w );
	    int x = w->x() + formwindow->grid().x();
	    int y = w->y() + formwindow->grid().y();
	    if ( w->x() + w->width() > parent->width() )
		x = QMAX( 0, parent->width() - w->width() );
	    if ( w->y() + w->height() > parent->height() )
		y = QMAX( 0, parent->height() - w->height() );
	    if ( x != w->x() || y != w->y() )
		w->move( x, y );
	    formwindow->selectWidget( w );
	} else if ( firstWidget.tagName() == "spacer" ) {
	    QWidget *w = createSpacer( firstWidget, parent, 0, firstWidget.tagName() == "vspacer" ? Qt::Vertical : Qt::Horizontal );
	    if ( !w )
		continue;
	    widgets.append( w );
	    int x = w->x() + formwindow->grid().x();
	    int y = w->y() + formwindow->grid().y();
	    if ( w->x() + w->width() > parent->width() )
		x = QMAX( 0, parent->width() - w->width() );
	    if ( w->y() + w->height() > parent->height() )
		y = QMAX( 0, parent->height() - w->height() );
	    if ( x != w->x() || y != w->y() )
		w->move( x, y );
	    formwindow->selectWidget( w );
	}
	firstWidget = firstWidget.nextSibling().toElement();
    }
    formwindow->setPropertyShowingBlocked( FALSE );
    formwindow->emitShowProperties();
    buf.close();

    PasteCommand *cmd = new PasteCommand( FormWindow::tr( "Paste" ), formwindow, widgets );
    formwindow->commandHistory()->addCommand( cmd );
}

void Resource::saveObject( QObject *obj, QDesignerGridLayout* grid, QTextStream &ts, int indent )
{
    if ( obj && obj->isWidgetType() && ( (QWidget*)obj )->isHidden() )
	return;
    QString closeTag;
    if ( obj->isWidgetType() ) {
	const char* className = WidgetFactory::classNameOf( obj );
	if ( obj->isA( "CustomWidget" ) )
	    usedCustomWidgets << QString( className );
	if ( obj != formwindow && !formwindow->widgets()->find( (QWidget*)obj ) )
	    return; // we don't know anything about this thing

	QString attributes;
	if ( grid ) {
	    QDesignerGridLayout::Item item = grid->items[ (QWidget*)obj ];
	    attributes += QString(" row=\"") + QString::number(item.row) + "\"";
	    attributes += QString(" column=\"") + QString::number(item.column) + "\"";
	    if ( item.rowspan * item.colspan != 1 ) {
		attributes += QString(" rowspan=\"") + QString::number(item.rowspan) + "\"";
		attributes += QString(" colspan=\"") + QString::number(item.colspan) + "\"";
	    }
	}

	if ( qstrcmp( className, "Spacer" ) == 0 ) {
	    closeTag = makeIndent( indent ) + "</spacer>\n";
	    ts << makeIndent( indent ) << "<spacer" << attributes << ">" << endl;
	    ++indent;
	} else {
	    closeTag = makeIndent( indent ) + "</widget>\n";
	    ts << makeIndent( indent ) << "<widget class=\"" << className << "\"" << attributes << ">" << endl;
	    ++indent;
	}
	if ( WidgetFactory::hasItems( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( obj ) ) ) )
	    saveItems( obj, ts, indent );
	saveObjectProperties( obj, ts, indent );
    } else {
	// test for other objects we created. Nothing so far.
	return;
    }

    if ( obj->inherits( "QTabWidget" ) ) {
	QTabWidget* tw = (QTabWidget*) obj;
	QObjectList* tmpl = tw->queryList( "QWidgetStack" );
	QWidgetStack *ws = (QWidgetStack*)tmpl->first();
	QTabBar *tb = ( (QDesignerTabWidget*)obj )->tabBar();
	for ( int i = 0; i < tb->count(); ++i ) {
	    QTab *t = tb->tabAt( i );
	    if ( !t )
		continue;
	    QWidget *w = ws->widget( t->identifier() );
	    if ( !w )
		continue;
	    if ( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) == -1 )
		continue; // we don't know this widget
	    ts << makeIndent( indent ) << "<widget class=\"QWidget\">" << endl;
	    ++indent;
	    ts << makeIndent( indent ) << "<property name=\"name\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<cstring>" << entitize( w->name() ) << "</cstring>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;

	    ts << makeIndent( indent ) << "<attribute name=\"title\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<string>" << entitize( t->text() ) << "</string>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</attribute>" << endl;
	    saveChildrenOf( w, ts, indent );
	    --indent;
	    ts << makeIndent( indent ) << "</widget>" << endl;
	}
	delete tmpl;
    } else if ( obj->inherits( "QWizard" ) ) {
	QWizard* wiz = (QWizard*)obj; for ( int i = 0; i < wiz->pageCount(); ++i ) {
	    QWidget *w = wiz->page( i );
	    if ( !w )
		continue;
	    if ( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) == -1 )
		continue; // we don't know this widget
	    ts << makeIndent( indent ) << "<widget class=\"QWidget\">" << endl;
	    ++indent;
	    ts << makeIndent( indent ) << "<property name=\"name\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<cstring>" << entitize( w->name() ) << "</cstring>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;

	    ts << makeIndent( indent ) << "<attribute name=\"title\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<string>" << entitize( wiz->title( w ) ) << "</string>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</attribute>" << endl;
	    saveChildrenOf( w, ts, indent );
	    --indent;
	    ts << makeIndent( indent ) << "</widget>" << endl;
	}
    } else if ( obj->inherits( "QMainWindow" ) ) {
	saveChildrenOf( ( (QMainWindow*)obj )->centralWidget(), ts, indent );
    } else {
	saveChildrenOf( obj, ts, indent );
    }

    indent--;
    ts << closeTag;
}

void Resource::saveItems( QObject *obj, QTextStream &ts, int indent )
{
    if ( obj->inherits( "QListBox" ) || obj->inherits( "QComboBox" ) ) {
	QListBox *lb = 0;
	if ( obj->inherits( "QListBox" ) )
	    lb = (QListBox*)obj;
	else
	    lb = ( (QComboBox*)obj )->listBox();

	QListBoxItem *i = lb->firstItem();
	for ( ; i; i = i->next() ) {
	    ts << makeIndent( indent ) << "<item>" << endl;
	    indent++;
	    QStringList text;
	    text << i->text();
	    QPtrList<QPixmap> pixmaps;
	    if ( i->pixmap() )
		pixmaps.append( i->pixmap() );
	    saveItem( text, pixmaps, ts, indent );
	    indent--;
	    ts << makeIndent( indent ) << "</item>" << endl;
	}
    } else if ( obj->inherits( "QIconView" ) ) {
	QIconView *iv = (QIconView*)obj;

	QIconViewItem *i = iv->firstItem();
	for ( ; i; i = i->nextItem() ) {
	    ts << makeIndent( indent ) << "<item>" << endl;
	    indent++;
	    QStringList text;
	    text << i->text();
	    QPtrList<QPixmap> pixmaps;
	    if ( i->pixmap() )
		pixmaps.append( i->pixmap() );
	    saveItem( text, pixmaps, ts, indent );
	    indent--;
	    ts << makeIndent( indent ) << "</item>" << endl;
	}
    } else if ( obj->inherits( "QListView" ) ) {
	QListView *lv = (QListView*)obj;
	int i;
	for ( i = 0; i < lv->header()->count(); ++i ) {
	    ts << makeIndent( indent ) << "<column>" << endl;
	    indent++;
	    QStringList l;
	    l << lv->header()->label( i );
	    QPtrList<QPixmap> pix;
	    pix.setAutoDelete( TRUE );
	    if ( lv->header()->iconSet( i ) )
		pix.append( new QPixmap( lv->header()->iconSet( i )->pixmap() ) );
	    saveItem( l, pix, ts, indent );
	    ts << makeIndent( indent ) << "<property name=\"clickable\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<bool>" << mkBool( lv->header()->isClickEnabled( i ) )<< "</bool>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;
	    ts << makeIndent( indent ) << "<property name=\"resizeable\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<bool>" << mkBool( lv->header()->isResizeEnabled( i ) ) << "</bool>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</column>" << endl;
	}
	saveItem( lv->firstChild(), ts, indent - 1 );
    }
#ifndef QT_NO_TABLE
    else if ( obj->inherits( "QTable" ) ) {
	QTable *table = (QTable*)obj;
	int i;
	QMap<QString, QString> columnFields = MetaDataBase::columnFields( table );
	bool isDataTable = table->inherits( "QDataTable" );
	for ( i = 0; i < table->horizontalHeader()->count(); ++i ) {
	    if ( !table->horizontalHeader()->label( i ).isNull() &&
		 table->horizontalHeader()->label( i ).toInt() != i + 1 ||
		 table->horizontalHeader()->iconSet( i ) ||
		  isDataTable ) {
		ts << makeIndent( indent ) << "<column>" << endl;
		indent++;
		QStringList l;
		l << table->horizontalHeader()->label( i );
		QPtrList<QPixmap> pix;
		pix.setAutoDelete( TRUE );
		if ( table->horizontalHeader()->iconSet( i ) )
		    pix.append( new QPixmap( table->horizontalHeader()->iconSet( i )->pixmap() ) );
		saveItem( l, pix, ts, indent );
		if ( table->inherits( "QDataTable" ) && !columnFields.isEmpty() ) {
		    ts << makeIndent( indent ) << "<property name=\"field\">" << endl;
		    indent++;
		    ts << makeIndent( indent ) << "<string>" << entitize( *columnFields.find( l[ 0 ] ) ) << "</string>" << endl;
		    indent--;
		    ts << makeIndent( indent ) << "</property>" << endl;
		}
		indent--;
		ts << makeIndent( indent ) << "</column>" << endl;
	    }
	}
	for ( i = 0; i < table->verticalHeader()->count(); ++i ) {
	    if ( !table->verticalHeader()->label( i ).isNull() &&
		 table->verticalHeader()->label( i ).toInt() != i + 1 ||
		 table->verticalHeader()->iconSet( i ) ) {
		ts << makeIndent( indent ) << "<row>" << endl;
		indent++;
		QStringList l;
		l << table->verticalHeader()->label( i );
		QPtrList<QPixmap> pix;
		pix.setAutoDelete( TRUE );
		if ( table->verticalHeader()->iconSet( i ) )
		    pix.append( new QPixmap( table->verticalHeader()->iconSet( i )->pixmap() ) );
		saveItem( l, pix, ts, indent );
		indent--;
		ts << makeIndent( indent ) << "</row>" << endl;
	    }
	}
    }
#endif
}

void Resource::saveItem( QListViewItem *i, QTextStream &ts, int indent )
{
    QListView *lv = i->listView();
    while ( i ) {
	ts << makeIndent( indent ) << "<item>" << endl;
	indent++;

	QPtrList<QPixmap> pixmaps;
	QStringList textes;
	for ( int c = 0; c < lv->columns(); ++c ) {
	    pixmaps.append( i->pixmap( c ) );
	    textes << i->text( c );
	}
	saveItem( textes, pixmaps, ts, indent );

	if ( i->firstChild() )
	    saveItem( i->firstChild(), ts, indent );

	indent--;
	ts << makeIndent( indent ) << "</item>" << endl;
	i = i->nextSibling();
    }
}

void Resource::savePixmap( const QPixmap &p, QTextStream &ts, int indent, const QString &tagname )
{
    if ( p.isNull() ) {
	ts << makeIndent( indent ) << "<" + tagname + "></"  + tagname + ">" << endl;
	return;
    }

    if ( formwindow && formwindow->savePixmapInline() )
	ts << makeIndent( indent ) << "<" + tagname + ">" << saveInCollection( p ) << "</" + tagname + ">" << endl;
#ifndef KOMMANDER
    else if ( formwindow && formwindow->savePixmapInProject() )
	ts << makeIndent( indent ) << "<" + tagname + ">" << MetaDataBase::pixmapKey( formwindow, p.serialNumber() )
	   << "</" + tagname + ">" << endl;
#endif
    else
	ts << makeIndent( indent ) << "<" + tagname + ">" << MetaDataBase::pixmapArgument( formwindow, p.serialNumber() )
	   << "</" + tagname + ">" << endl;
}

QPixmap Resource::loadPixmap( const QDomElement &e, const QString &/*tagname*/ )
{
    QString arg = e.firstChild().toText().data();

    if ( formwindow && formwindow->savePixmapInline() ) {
	QImage img = loadFromCollection( arg );
	QPixmap pix;
	pix.convertFromImage( img );
	MetaDataBase::setPixmapArgument( formwindow, pix.serialNumber(), arg );
	return pix;
    } 
#ifndef KOMMANDER
    else if ( formwindow && formwindow->savePixmapInProject() ) {
	QPixmap pix;
	if ( mainwindow && mainwindow->currProject() )
	    pix = mainwindow->currProject()->pixmapCollection()->pixmap( arg );
    }
#endif
    else
    {
	QPixmap pix;
        pix = PixmapChooser::loadPixmap( "image.xpm" );
	MetaDataBase::setPixmapKey( formwindow, pix.serialNumber(), arg );
	return pix;
    }
    QPixmap pix = PixmapChooser::loadPixmap( "image.xpm" );
    MetaDataBase::setPixmapArgument( formwindow, pix.serialNumber(), arg );
    return pix;
}

void Resource::saveItem( const QStringList &text, const QPtrList<QPixmap> &pixmaps, QTextStream &ts, int indent )
{
    QStringList::ConstIterator it = text.begin();
    for ( ; it != text.end(); ++it ) {
	ts << makeIndent( indent ) << "<property name=\"text\">" << endl;
	indent++;
	ts << makeIndent( indent ) << "<string>" << entitize( *it ) << "</string>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</property>" << endl;
    }

    for ( int i = 0; i < (int)pixmaps.count(); ++i ) {
	QPixmap *p = ( (QPtrList<QPixmap>)pixmaps ).at( i );
	ts << makeIndent( indent ) << "<property name=\"pixmap\">" << endl;
	indent++;
	if ( p )
	    savePixmap( *p, ts, indent );
	else
	    savePixmap( QPixmap(), ts, indent );
	indent--;
	ts << makeIndent( indent ) << "</property>" << endl;
    }
}

void Resource::saveChildrenOf( QObject* obj, QTextStream &ts, int indent )
{
    const QObjectList *l = obj->children();
    if ( !l )
	return; // no children to save

    QString closeTag;
    // if the widget has a layout we pretend that all widget's childs are childs of the layout - makes the structure nicer
    QLayout *layout = 0;
    QDesignerGridLayout* grid = 0;
    if ( !obj->inherits( "QSplitter" ) &&
	 WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( obj ) ) ) &&
	 obj->isWidgetType() &&
	 WidgetFactory::layoutType( (QWidget*)obj, layout ) != WidgetFactory::NoLayout ) {
	WidgetFactory::LayoutType lay = WidgetFactory::layoutType( (QWidget*)obj, layout );
	switch ( lay ) {
	case WidgetFactory::HBox:
	    closeTag = makeIndent( indent ) + "</hbox>";
	    ts << makeIndent( indent ) << "<hbox>" << endl;
	    ++indent;
	    break;
	case WidgetFactory::VBox:
	    closeTag = makeIndent( indent ) + "</vbox>";
	    ts << makeIndent( indent ) << "<vbox>" << endl;
	    ++indent;
	    break;
	case WidgetFactory::Grid:
	    closeTag = makeIndent( indent ) + "</grid>";
	    ts << makeIndent( indent ) << "<grid>" << endl;
	    ++indent;
	    grid = (QDesignerGridLayout*) layout;
	    break;
	default:
	    break;
	}

	// save properties of layout
	if ( lay != WidgetFactory::NoLayout )
	    saveObjectProperties( layout, ts, indent );

    }

    for ( QPtrListIterator<QObject> it ( *l ); it.current(); ++it )
	saveObject( it.current(), grid, ts, indent );
    if ( !closeTag.isEmpty() ) {
	indent--;
	ts << closeTag << endl;
    }
}

void Resource::saveObjectProperties( QObject *w, QTextStream &ts, int indent )
{
    QStringList saved;
    QStringList changed;
    changed = MetaDataBase::changedProperties( w );
    if ( w->isWidgetType() ) {
	if ( w->inherits( "Spacer" ) ) {
	    if ( !changed.contains( "sizeHint" ) )
		changed << "sizeHint";
	    if ( !changed.contains( "geometry" ) )
		changed << "geometry";
	}
    } else if ( w->inherits( "QLayout" ) ) { // #### should be cleaner (RS)
	changed << "margin" << "spacing";
    }

    if ( w == formwindow->mainContainer() ) {
	if ( changed.findIndex( "geometry" ) == -1 )
	    changed << "geometry";
	if ( changed.findIndex( "caption" ) == -1 )
	    changed << "caption";
    }

    if ( changed.isEmpty() )
	    return;

    bool inLayout = w != formwindow->mainContainer() && !copying && w->isWidgetType() && ( (QWidget*)w )->parentWidget() &&
		    WidgetFactory::layoutType( ( (QWidget*)w )->parentWidget() ) != WidgetFactory::NoLayout;

    QStrList lst = w->metaObject()->propertyNames( !w->inherits( "Spacer" ) );
    for ( QPtrListIterator<char> it( lst ); it.current(); ++it ) {
	if ( changed.find( QString::fromLatin1( it.current() ) ) == changed.end() )
	    continue;
	if ( saved.find( QString::fromLatin1( it.current() ) ) != saved.end() )
	    continue;
	saved << QString::fromLatin1( it.current() );
	const QMetaProperty* p = w->metaObject()->
				 property( w->metaObject()->findProperty( it.current(), TRUE ), TRUE );
	if ( !p || !p->stored( w ) || ( inLayout && qstrcmp( p->name(), "geometry" ) == 0 ) )
	    continue;
	if ( w->inherits( "QLabel" ) && qstrcmp( p->name(), "pixmap" ) == 0 &&
	     ( !( (QLabel*)w )->pixmap() || ( (QLabel*)w )->pixmap()->isNull() ) )
	    continue;
	if ( w->inherits( "QDesignerMenuBar" ) &&
	     ( qstrcmp( p->name(), "itemName" ) == 0 || qstrcmp( p->name(), "itemNumber" ) == 0 ||
	       qstrcmp( p->name(), "itemText" ) == 0 ) )
	    continue;
	if ( qstrcmp( p->name(), "name" ) == 0 )
	    knownNames << w->property( "name" ).toString();
	ts << makeIndent( indent ) << "<property";
	ts << " name=\"" << it.current() << "\"";
	if ( !p->stdSet() )
	    ts << " stdset=\"0\"";
	ts << ">" << endl;
	indent++;
	if ( p->isSetType() ) {
	    saveSetProperty( w, it.current(), QVariant::nameToType( p->type() ), ts, indent );
	} else if ( p->isEnumType() ) {
	    saveEnumProperty( w, it.current(), QVariant::nameToType( p->type() ), ts, indent );
	} else {
	    saveProperty( w, it.current(), w->property( p->name() ), QVariant::nameToType( p->type() ), ts, indent );
	}
	indent--;
	ts << makeIndent( indent ) << "</property>" << endl;
    }

    if ( w->isWidgetType() && MetaDataBase::fakeProperties( w ) ) {
	QMap<QString, QVariant>* fakeProperties = MetaDataBase::fakeProperties( w );
	for ( QMap<QString, QVariant>::Iterator fake = fakeProperties->begin();
	      fake != fakeProperties->end(); ++fake ) {
	    if ( MetaDataBase::isPropertyChanged( w, fake.key() ) ) {
		if ( w->inherits( "CustomWidget" ) ) {
		    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)w )->customWidget();
		    if ( cw && !cw->hasProperty( fake.key().latin1() ) && fake.key() != "toolTip" && fake.key() != "whatsThis" )
			continue;
		}

		ts << makeIndent( indent ) << "<property name=\"" << fake.key() << "\" stdset=\"0\">" << endl;
		indent++;
		saveProperty( w, fake.key(), *fake, (*fake).type(), ts, indent );
		indent--;
		ts << makeIndent( indent ) << "</property>" << endl;
	    }
	}
    }
}

void Resource::saveSetProperty( QObject *w, const QString &name, QVariant::Type, QTextStream &ts, int indent )
{
    const QMetaProperty *p = w->metaObject()->property( w->metaObject()->findProperty( name, TRUE ), TRUE );
    QStrList l( p->valueToKeys( w->property( name ).toInt() ) );
    QString v;
    for ( uint i = 0; i < l.count(); ++i ) {
	v += l.at( i );
	if ( i < l.count() - 1 )
	    v += "|";
    }
    ts << makeIndent( indent ) << "<set>" << v << "</set>" << endl;
}

void Resource::saveEnumProperty( QObject *w, const QString &name, QVariant::Type, QTextStream &ts, int indent )
{
    const QMetaProperty *p = w->metaObject()->property( w->metaObject()->findProperty( name, TRUE ), TRUE );
    ts << makeIndent( indent ) << "<enum>" << p->valueToKey( w->property( name ).toInt() ) << "</enum>" << endl;
}

void Resource::saveProperty( QObject *w, const QString &name, const QVariant &value, QVariant::Type t, QTextStream &ts, int indent )
{
    if ( name == "hAlign" || name =="vAlign" || name == "wordwrap" || name == "layoutMargin" || name =="layoutSpacing" )
	return;
    int num, unum;
    double dob;
    QString comment;
    if ( w && formwindow->widgets()->find( (QWidget*)w ) )
	comment = MetaDataBase::propertyComment( w, name );
    switch ( t ) {
    case QVariant::String:
	ts << makeIndent( indent ) << "<string>" << entitize( value.toString() ) << "</string>" << endl;
	if ( !comment.isEmpty() )
	    ts << makeIndent( indent ) << "<comment>" << entitize( comment ) << "</comment>" << endl;
	break;
    case QVariant::CString:
	ts << makeIndent( indent ) << "<cstring>" << entitize( value.toCString() ).latin1() << "</cstring>" << endl;
	break;
    case QVariant::Bool:
	ts << makeIndent( indent ) << "<bool>" << mkBool( value.toBool() ) << "</bool>" << endl;
	break;
    case QVariant::Int:
	num = value.toInt();
	if ( w && w->inherits( "QLayout" ) ) {
	    if ( name == "spacing" )
		num = MetaDataBase::spacing( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (QLayout*)w ) ) );
	    else if ( name == "margin" )
		num = MetaDataBase::margin( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (QLayout*)w ) ) );
	}
	ts << makeIndent( indent ) << "<number>" << QString::number( num ) << "</number>" << endl;
	break;
    case QVariant::Double:
	dob = value.toDouble();
	ts << makeIndent( indent ) << "<number>" << QString::number( dob ) << "</number>" << endl;
	break;
    case QVariant::KeySequence:
	num = value.toInt();
	ts << makeIndent( indent ) << "<number>" << QString::number( num ) << "</number>" << endl;
	break;
    case QVariant::UInt:
	unum = value.toUInt();
	if ( w && w->inherits( "QLayout" ) ) {
	    if ( name == "spacing" )
		num = MetaDataBase::spacing( WidgetFactory::layoutParent( (QLayout*)w ) );
	    else if ( name == "margin" )
		num = MetaDataBase::margin( WidgetFactory::layoutParent( (QLayout*)w ) );
	}
	ts << makeIndent( indent ) << "<number>" << QString::number( unum ) << "</number>" << endl;
	break;
    case QVariant::Rect: {
	QVariant v( value );
	ts << makeIndent( indent ) << "<rect>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<x>" << QString::number( v.toRect().x() ) << "</x>" << endl;
	ts << makeIndent( indent ) << "<y>" << QString::number( v.toRect().y() ) << "</y>" << endl;
	ts << makeIndent( indent ) << "<width>" << QString::number( v.toRect().width() ) << "</width>" << endl;
	ts << makeIndent( indent ) << "<height>" << QString::number( v.toRect().height() ) << "</height>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</rect>" << endl;
    } break;
    case QVariant::Point: {
	QVariant v( value );
	ts << makeIndent( indent ) << "<point>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<x>" << QString::number( v.toPoint().x() ) << "</x>" << endl;
	ts << makeIndent( indent ) << "<y>" << QString::number( v.toPoint().y() ) << "</y>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</point>" << endl;
    } break;
    case QVariant::Size: {
	QVariant v( value );
	ts << makeIndent( indent ) << "<size>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<width>" << QString::number( v.toSize().width() ) << "</width>" << endl;
	ts << makeIndent( indent ) << "<height>" << QString::number( v.toSize().height() ) << "</height>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</size>" << endl;
    } break;
    case QVariant::Color: {
	QVariant v( value );
	ts << makeIndent( indent ) << "<color>" << endl;
	indent++;
	saveColor( ts, indent, v.toColor() );
	indent--;
	ts << makeIndent( indent ) << "</color>" << endl;
    } break;
    case QVariant::Font: {
	QVariant v( value );
	ts << makeIndent( indent ) << "<font>" << endl;
	QFont f( qApp->font() );
	if ( w && w->isWidgetType() && ((QWidget*)w)->parentWidget() )
	    f = ((QWidget*)w)->parentWidget()->font();
	QFont f2( v.toFont() );
	indent++;
	if ( f.family() != f2.family() )
	    ts << makeIndent( indent ) << "<family>" << f2.family() << "</family>" << endl;
	if ( f.pointSize() != f2.pointSize() )
	    ts << makeIndent( indent ) << "<pointsize>" << QString::number( f2.pointSize() ) << "</pointsize>" << endl;
	if ( f.bold() != f2.bold() )
	    ts << makeIndent( indent ) << "<bold>" << QString::number( (int)f2.bold() ) << "</bold>" << endl;
	if ( f.italic() != f2.italic() )
	    ts << makeIndent( indent ) << "<italic>" << QString::number( (int)f2.italic() ) << "</italic>" << endl;
	if ( f.underline() != f2.underline() )
	    ts << makeIndent( indent ) << "<underline>" << QString::number( (int)f2.underline() ) << "</underline>" << endl;
	if ( f.strikeOut() != f2.strikeOut() )
	    ts << makeIndent( indent ) << "<strikeout>" << QString::number( (int)f2.strikeOut() ) << "</strikeout>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</font>" << endl;
    } break;
    case QVariant::SizePolicy: {
	QSizePolicy sp( value.toSizePolicy() );
	ts << makeIndent( indent ) << "<sizepolicy>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<hsizetype>" << (int)sp.horData() << "</hsizetype>" << endl;
	ts << makeIndent( indent ) << "<vsizetype>" << (int)sp.verData() << "</vsizetype>" << endl;
	ts << makeIndent( indent ) << "<horstretch>" << (int)sp.horStretch() << "</horstretch>" << endl;
	ts << makeIndent( indent ) << "<verstretch>" << (int)sp.verStretch() << "</verstretch>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</sizepolicy>" << endl;
	break;
    }
    case QVariant::Pixmap:
	savePixmap( value.toPixmap(), ts, indent );
	break;
    case QVariant::IconSet:
	savePixmap( value.toIconSet().pixmap(), ts, indent, "iconset" );
	break;
    case QVariant::Image:
	ts << makeIndent( indent ) << "<image>" << saveInCollection( value.toImage() ) << "</image>" << endl;
    break;
    case QVariant::Palette: {
	QPalette p( value.toPalette() );
	ts << makeIndent( indent ) << "<palette>" << endl;
	indent++;

	ts << makeIndent( indent ) << "<active>" << endl;
	indent++;
	saveColorGroup( ts, indent, p.active() );
	indent--;
	ts << makeIndent( indent ) << "</active>" << endl;

	ts << makeIndent( indent ) << "<disabled>" << endl;
	indent++;
	saveColorGroup( ts, indent, p.disabled() );
	indent--;
	ts << makeIndent( indent ) << "</disabled>" << endl;

	ts << makeIndent( indent ) << "<inactive>" << endl;
	indent++;
	saveColorGroup( ts, indent, p.inactive() );
	indent--;
	ts << makeIndent( indent ) << "</inactive>" << endl;

	indent--;
	ts << makeIndent( indent ) << "</palette>" << endl;
    } break;
    case QVariant::Cursor:
	ts << makeIndent( indent ) << "<cursor>" << value.toCursor().shape() << "</cursor>" << endl;
	break;
    case QVariant::StringList: {
	QStringList lst = value.toStringList();
	uint i = 0;
	ts << makeIndent( indent ) << "<stringlist>" << endl;
	indent++;
	if ( !lst.isEmpty() ) {
	    for ( i = 0; i < lst.count(); ++i )
		ts << makeIndent( indent ) << "<string>" << entitize( lst[ i ] ) << "</string>" << endl;
	}
	indent--;
	ts << makeIndent( indent ) << "</stringlist>" << endl;
    } break;
    case QVariant::Date: {
	QDate d = value.toDate();
	ts << makeIndent( indent ) << "<date>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<year>" << d.year() << "</year>"  << endl;
	ts << makeIndent( indent ) << "<month>" << d.month() << "</month>"  << endl;
	ts << makeIndent( indent ) << "<day>" << d.day() << "</day>"  << endl;
	indent--;
	ts << makeIndent( indent ) << "</date>" << endl;
	break;
    }
    case QVariant::Time: {
	QTime t = value.toTime();
	ts << makeIndent( indent ) << "<time>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<hour>" << t.hour() << "</hour>"  << endl;
	ts << makeIndent( indent ) << "<minute>" << t.minute() << "</minute>"  << endl;
	ts << makeIndent( indent ) << "<second>" << t.second() << "</second>"  << endl;
	indent--;
	ts << makeIndent( indent ) << "</time>" << endl;
	break;
    }
    case QVariant::DateTime: {
	QDateTime dt = value.toDateTime();
	ts << makeIndent( indent ) << "<datetime>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<year>" << dt.date().year() << "</year>"  << endl;
	ts << makeIndent( indent ) << "<month>" << dt.date().month() << "</month>"  << endl;
	ts << makeIndent( indent ) << "<day>" << dt.date().day() << "</day>"  << endl;
	ts << makeIndent( indent ) << "<hour>" << dt.time().hour() << "</hour>"  << endl;
	ts << makeIndent( indent ) << "<minute>" << dt.time().minute() << "</minute>"  << endl;
	ts << makeIndent( indent ) << "<second>" << dt.time().second() << "</second>"  << endl;
	indent--;
	ts << makeIndent( indent ) << "</datetime>" << endl;
	break;
    }
    default:
	qWarning( "saving the property %s of type %d not supported yet", name.latin1(), (int)t );
    }
}

void Resource::saveColorGroup( QTextStream &ts, int indent, const QColorGroup &cg )
{
    for( int r = 0 ; r < QColorGroup::NColorRoles ; r++ ) {
	ts << makeIndent( indent ) << "<color>" << endl;
	indent++;
	saveColor( ts, indent, cg.color( (QColorGroup::ColorRole)r ) );
	indent--;
	ts << makeIndent( indent ) << "</color>" << endl;
	QPixmap* pm = cg.brush( (QColorGroup::ColorRole)r ).pixmap();
	if ( pm && !pm->isNull() )
	    savePixmap( *pm, ts, indent );
    }
}

void Resource::saveColor( QTextStream &ts, int indent, const QColor &c )
{
    ts << makeIndent( indent ) << "<red>" << QString::number( c.red() ) << "</red>" << endl;
    ts << makeIndent( indent ) << "<green>" << QString::number( c.green() ) << "</green>" << endl;
    ts << makeIndent( indent ) << "<blue>" << QString::number( c.blue() ) << "</blue>" << endl;
}

QObject *Resource::createObject( const QDomElement &e, QWidget *parent, QLayout* layout )
{
    lastItem = 0;
    QDomElement n = e.firstChild().toElement();
    QWidget *w = 0; // the widget that got created
    QObject *obj = 0; // gets the properties

    int row = e.attribute( "row" ).toInt();
    int col = e.attribute( "column" ).toInt();
    int rowspan = e.attribute( "rowspan" ).toInt();
    int colspan = e.attribute( "colspan" ).toInt();
    if ( rowspan < 1 )
	rowspan = 1;
    if ( colspan < 1 )
	colspan = 1;

    QString className = e.attribute( "class", "QWidget" );

    if ( !className.isNull() ) {
	obj = WidgetFactory::create( WidgetDatabase::idFromClassName( className ), parent, 0, FALSE );
	if ( !obj )
	    return 0;
	if ( !mainContainerSet ) {
	    if ( formwindow )
		formwindow->setMainContainer( (QWidget*)obj );
	    mainContainerSet = TRUE;
	}
	w = (QWidget*)obj;
	if ( w->inherits( "QMainWindow" ) )
	    w = ( (QMainWindow*)w )->centralWidget();
	if ( layout ) {
	    switch ( WidgetFactory::layoutType( layout ) ) {
	    case WidgetFactory::HBox:
		( (QHBoxLayout*)layout )->addWidget( w );
		break;
	    case WidgetFactory::VBox:
		( (QVBoxLayout*)layout )->addWidget( w );
		break;
	    case WidgetFactory::Grid:
		( (QDesignerGridLayout*)layout )->addMultiCellWidget( w, row, row + rowspan - 1,
								      col, col + colspan - 1 );
		break;
	    default:
		break;
	    }
	}

	if ( !toplevel )
	    toplevel = w;
	layout = 0;

	if ( w && formwindow ) {
	    if ( !parent || ( !parent->inherits( "QTabWidget" ) && !parent->inherits( "QWizard" ) ) )
		formwindow->insertWidget( w, pasting );
	    else if ( parent && ( parent->inherits( "QTabWidget" ) || parent->inherits( "QWizard" ) ) )
		MetaDataBase::addEntry( w );
	}
    }

    while ( !n.isNull() ) {
	if ( n.tagName() == "spacer" ) {
	    createSpacer( n, w, layout, Qt::Horizontal );
	} else if ( n.tagName() == "widget" ) {
	    createObject( n, w, layout );
	} else if ( n.tagName() == "hbox" ) {
	    layout = WidgetFactory::createLayout( w, layout, WidgetFactory::HBox );
	    obj = layout;
	    n = n.firstChild().toElement();
	    continue;
	} else if ( n.tagName() == "grid" ) {
	    layout = WidgetFactory::createLayout( w, layout, WidgetFactory::Grid );
	    obj = layout;
	    n = n.firstChild().toElement();
	    continue;
	} else if ( n.tagName() == "vbox" ) {
	    layout = WidgetFactory::createLayout( w, layout, WidgetFactory::VBox );
	    obj = layout;
	    n = n.firstChild().toElement();
	    continue;
	} else if ( n.tagName() == "property" && obj ) {
	    setObjectProperty( obj, n.attribute( "name" ), n.firstChild().toElement() );
	} else if ( n.tagName() == "attribute" && w ) {
	    QString attrib = n.attribute( "name" );
	    QVariant v = DomTool::elementToVariant( n.firstChild().toElement(), QVariant() );
	    if ( parent->inherits( "QTabWidget" ) ) {
		if ( attrib == "title" )
		    ( (QTabWidget*)parent )->insertTab( w, v.toString() );
	    } else if ( parent->inherits( "QWizard" ) ) {
		if ( attrib == "title" )
		    ( (QWizard*)parent )->addPage( w, v.toString() );
	    }
	} else if ( n.tagName() == "item" ) {
	    createItem( n, w );
	} else if ( n.tagName() == "column" || n.tagName() =="row" ) {
	    createColumn( n, w );
	} else if ( n.tagName() == "event" ) {
#ifndef KOMMANDER
	    MetaDataBase::setEventFunctions( obj, formwindow, MainWindow::self->currProject()->language(),
#else
	    MetaDataBase::setEventFunctions( obj, formwindow, "C++",
#endif
					     n.attribute( "name" ), QStringList::split( ',', n.attribute( "functions" ) ), FALSE );
	}

	n = n.nextSibling().toElement();
    }

    return w;
}

void Resource::createColumn( const QDomElement &e, QWidget *widget )
{
    if ( !widget )
	return;

    if ( widget->inherits( "QListView" ) && e.tagName() == "column" ) {
	QListView *lv = (QListView*)widget;
	QDomElement n = e.firstChild().toElement();
	QPixmap pix;
	bool hasPixmap = FALSE;
	QString txt;
	bool clickable = TRUE, resizeable = TRUE;
	while ( !n.isNull() ) {
	    if ( n.tagName() == "property" ) {
		QString attrib = n.attribute( "name" );
		QVariant v = DomTool::elementToVariant( n.firstChild().toElement(), QVariant() );
		if ( attrib == "text" )
		    txt = v.toString();
		else if ( attrib == "pixmap" ) {
		    pix = loadPixmap( n.firstChild().toElement().toElement() );
		    hasPixmap = !pix.isNull();
		} else if ( attrib == "clickable" )
		    clickable = v.toBool();
		else if ( attrib == "resizeable" )
		    resizeable = v.toBool();
	    }
	    n = n.nextSibling().toElement();
	}
	lv->addColumn( txt );
	int i = lv->header()->count() - 1;
	if ( hasPixmap ) {
	    lv->header()->setLabel( i, pix, txt );
	}
	if ( !clickable )
	    lv->header()->setClickEnabled( clickable, i );
	if ( !resizeable )
	    lv->header()->setResizeEnabled( resizeable, i );
    }
#ifndef QT_NO_TABLE
    else if ( widget->inherits( "QTable" ) ) {
	QTable *table = (QTable*)widget;
	bool isRow;
	if ( ( isRow = e.tagName() == "row" ) )
	    table->setNumRows( table->numRows() + 1 );
	else
	    table->setNumCols( table->numCols() + 1 );

	QDomElement n = e.firstChild().toElement();
	QPixmap pix;
	bool hasPixmap = FALSE;
	QString txt;
	QString field;
	QMap<QString, QString> fieldMap = MetaDataBase::columnFields( table );
	while ( !n.isNull() ) {
	    if ( n.tagName() == "property" ) {
		QString attrib = n.attribute( "name" );
		QVariant v = DomTool::elementToVariant( n.firstChild().toElement(), QVariant() );
		if ( attrib == "text" )
		    txt = v.toString();
		else if ( attrib == "pixmap" ) {
		    hasPixmap = !n.firstChild().firstChild().toText().data().isEmpty();
		    if ( hasPixmap )
			pix = loadPixmap( n.firstChild().toElement() );
		} else if ( attrib == "field" )
		    field = v.toString();
	    }
	    n = n.nextSibling().toElement();
	}

	int i = isRow ? table->numRows() - 1 : table->numCols() - 1;
	QHeader *h = !isRow ? table->horizontalHeader() : table->verticalHeader();
	if ( hasPixmap )
	    h->setLabel( i, pix, txt );
	else
	    h->setLabel( i, txt );
	if ( !isRow && !field.isEmpty() )
	    fieldMap.insert( txt, field );
	MetaDataBase::setColumnFields( table, fieldMap );
    }
#endif
}

void Resource::loadItem( const QDomElement &e, QPixmap &pix, QString &txt, bool &hasPixmap )
{
    QDomElement n = e;
    hasPixmap = FALSE;
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" ) {
	    QString attrib = n.attribute( "name" );
	    QVariant v = DomTool::elementToVariant( n.firstChild().toElement(), QVariant() );
	    if ( attrib == "text" )
		txt = v.toString();
	    else if ( attrib == "pixmap" ) {
		pix = loadPixmap( n.firstChild().toElement() );
		hasPixmap = !pix.isNull();
	    }
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::createItem( const QDomElement &e, QWidget *widget, QListViewItem *i )
{
    if ( !widget || !WidgetFactory::hasItems( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( widget ) ) ) )
	return;

    if ( widget->inherits( "QListBox" ) || widget->inherits( "QComboBox" ) ) {
	QDomElement n = e.firstChild().toElement();
	QPixmap pix;
	bool hasPixmap = FALSE;
	QString txt;
	loadItem( n, pix, txt, hasPixmap );
	QListBox *lb = 0;
	if ( widget->inherits( "QListBox" ) )
	    lb = (QListBox*)widget;
	else
	    lb = ( (QComboBox*)widget)->listBox();
	if ( hasPixmap ) {
	    new QListBoxPixmap( lb, pix, txt );
	} else {
	    new QListBoxText( lb, txt );
	}
    } else if ( widget->inherits( "QIconView" ) ) {
	QDomElement n = e.firstChild().toElement();
	QPixmap pix;
	bool hasPixmap = FALSE;
	QString txt;
	loadItem( n, pix, txt, hasPixmap );

	QIconView *iv = (QIconView*)widget;
	new QIconViewItem( iv, txt, pix );
    } else if ( widget->inherits( "QListView" ) ) {
	QDomElement n = e.firstChild().toElement();
	QPixmap pix;
	QValueList<QPixmap> pixmaps;
	QStringList textes;
	QListViewItem *item = 0;
	QListView *lv = (QListView*)widget;
	if ( i )
	    item = new QListViewItem( i, lastItem );
	else
	    item = new QListViewItem( lv, lastItem );
	while ( !n.isNull() ) {
	    if ( n.tagName() == "property" ) {
		QString attrib = n.attribute( "name" );
		QVariant v = DomTool::elementToVariant( n.firstChild().toElement(), QVariant() );
		if ( attrib == "text" )
		    textes << v.toString();
		else if ( attrib == "pixmap" ) {
		    QString s = v.toString();
		    if ( s.isEmpty() ) {
			pixmaps << QPixmap();
		    } else {
			pix = loadPixmap( n.firstChild().toElement() );
			pixmaps << pix;
		    }
		}
	    } else if ( n.tagName() == "item" ) {
		item->setOpen( TRUE );
		createItem( n, widget, item );
	    }

	    n = n.nextSibling().toElement();
	}

	for ( int i = 0; i < lv->columns(); ++i ) {
	    item->setText( i, textes[ i ] );
	    item->setPixmap( i, pixmaps[ i ] );
	}
	lastItem = item;
    }
}

QWidget *Resource::createSpacer( const QDomElement &e, QWidget *parent, QLayout *layout, Qt::Orientation o )
{
    QDomElement n = e.firstChild().toElement();
    int row = e.attribute( "row" ).toInt();
    int col = e.attribute( "column" ).toInt();
    int rowspan = e.attribute( "rowspan" ).toInt();
    int colspan = e.attribute( "colspan" ).toInt();
    if ( rowspan < 1 )
	rowspan = 1;
    if ( colspan < 1 )
	colspan = 1;

    Spacer *spacer = (Spacer*) WidgetFactory::create( WidgetDatabase::idFromClassName("Spacer"),
						      parent, "spacer", FALSE);
    spacer->setOrientation( o );
    spacer->setInteraciveMode( FALSE );
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" )
	    setObjectProperty( spacer, n.attribute( "name" ), n.firstChild().toElement() );
	n = n.nextSibling().toElement();
    }
    spacer->setInteraciveMode( TRUE );
    if ( formwindow )
	formwindow->insertWidget( spacer, pasting );
    if ( layout ) {
	if ( layout->inherits( "QBoxLayout" ) )
	    ( (QBoxLayout*)layout )->addWidget( spacer, 0, spacer->alignment() );
	else
	    ( (QDesignerGridLayout*)layout )->addMultiCellWidget( spacer, row, row + rowspan - 1, col, col + colspan - 1,
								  spacer->alignment() );
    }
    return spacer;
}

/*!
  Attention: this function has to be in sync with Uic::setObjectProperty(). If you change one, change both.
*/
void Resource::setObjectProperty( QObject* obj, const QString &prop, const QDomElement &e )
{
    const QMetaProperty *p = obj->metaObject()->property( obj->metaObject()->findProperty( prop, TRUE ), TRUE );

    if ( !obj->inherits( "QLayout" )  ) {// no layouts in metadatabase... (RS)
	if ( obj->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)obj )->customWidget();
	    if ( cw && !cw->hasProperty( prop.latin1() ) && !p && prop != "toolTip" && prop != "whatsThis" )
		return;
	}
	MetaDataBase::setPropertyChanged( obj, prop, TRUE );
    }

    QVariant defVarient;
    if ( e.tagName() == "font" ) {
	QFont f( qApp->font() );
	if ( obj->isWidgetType() && ( (QWidget*)obj )->parentWidget() )
	    f = ( (QWidget*)obj )->parentWidget()->font();
	defVarient = QVariant( f );
    }

    QString comment;
    QVariant v( DomTool::elementToVariant( e, defVarient, comment ) );

    if ( !comment.isEmpty() ) {
	MetaDataBase::addEntry( obj );
	MetaDataBase::setPropertyComment( obj, prop, comment );
    }

    if ( e.tagName() == "pixmap" ) {
	QPixmap pix = loadPixmap( e );
	if ( pix.isNull() )
	    return;
	v = QVariant( pix );
    } else if ( e.tagName() == "iconset" ) {
	QPixmap pix = loadPixmap( e, "iconset" );
	if ( pix.isNull() )
	    return;
	v = QVariant( QIconSet( pix ) );
    } else if ( e.tagName() == "image" ) {
	v = QVariant( loadFromCollection( v.toString() ) );
    }

    if ( !p ) {
	MetaDataBase::setFakeProperty( obj, prop, v );
	if ( obj->isWidgetType() ) {
	    if ( prop == "database" && obj != toplevel ) {
		QStringList lst = MetaDataBase::fakeProperty( obj, "database" ).toStringList();
		if ( lst.count() > 2 )
		    dbControls.insert( obj->name(), lst[ 2 ] );
		else if ( lst.count() == 2 )
		    dbTables.insert( obj->name(), lst );
	    }
	    return;
	}
    }


    if ( e.tagName() == "palette" ) {
	QDomElement n = e.firstChild().toElement();
	QPalette p;
	while ( !n.isNull() ) {
	    QColorGroup cg;
	    if ( n.tagName() == "active" ) {
		cg = loadColorGroup( n );
		p.setActive( cg );
	    } else if ( n.tagName() == "inactive" ) {
		cg = loadColorGroup( n );
		p.setInactive( cg );
	    } else if ( n.tagName() == "disabled" ) {
		cg = loadColorGroup( n );
		p.setDisabled( cg );
	    }
	    n = n.nextSibling().toElement();
	}
	v = QPalette( p );
    } else if ( e.tagName() == "enum" && p && p->isEnumType() ) {
	QString key( v.toString() );
	v = QVariant( p->keyToValue( key ) );
    } else if ( e.tagName() == "set" && p && p->isSetType() ) {
	QString keys( v.toString() );
	QStringList lst = QStringList::split( '|', keys );
	QStrList l;
	for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
	    l.append( *it );
	v = QVariant( p->keysToValue( l ) );
    }

    if ( prop == "caption" ) {
	QCString s1 = v.toCString();
	QString s2 = v.toString();
	if ( !s1.isEmpty() )
	    formwindow->setCaption( s1 );
	else if ( !s2.isEmpty() )
	    formwindow->setCaption( s2 );
    }
    if ( prop == "icon" ) {
	formwindow->setIcon( v.toPixmap() );
    }

    if ( prop == "geometry" ) {
	if ( obj == toplevel ) {
	    hadGeometry = TRUE;
	    toplevel->resize( v.toRect().size() );
	    return;
	} else if ( obj == formwindow->mainContainer() ) {
	    hadGeometry = TRUE;
	    formwindow->resize( v.toRect().size() );
	    return;
	}
    }

    if ( obj->inherits( "QLayout" ) ) {
	if ( prop == "spacing" ) {
	    MetaDataBase::setSpacing( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (QLayout*)obj ) ), v.toInt() );
	    return;
	}
	if ( prop == "margin" ) {
	    MetaDataBase::setMargin( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (QLayout*)obj ) ), v.toInt() );
	    return;
	}
    }

    if ( prop == "name" ) {
	if ( pasting ) {
	    QString s = v.toString();
	    formwindow->unify( (QWidget*)obj, s, TRUE );
	    obj->setName( s );
	    return;
	} else if ( formwindow && obj == formwindow->mainContainer() ) {
	    formwindow->setName( v.toCString() );
	}
    }

    if ( prop == "sizePolicy" ) {
	QSizePolicy sp = v.toSizePolicy();
	sp.setHeightForWidth( ( (QWidget*)obj )->sizePolicy().hasHeightForWidth() );
    }

    obj->setProperty( prop, v );
}


QString Resource::saveInCollection( const QImage &img )
{
    QString imgName = "none";
    QValueList<Image>::Iterator it = images.begin();
    for ( ; it != images.end(); ++it ) {
	if ( img == ( *it ).img ) {
	    imgName = ( *it ).name;
	    break;
	}
    }

    if ( imgName == "none" ) {
	Image i;
	imgName = "image" + QString::number( images.count() );
	i.name = imgName;
	i.img = img;
	images.append( i );
    }
    return imgName;
}

void Resource::saveImageData( const QImage &img, QTextStream &ts, int indent )
{
    QByteArray ba;
    QBuffer buf( ba );
    buf.open( IO_WriteOnly | IO_Translate );
    QImageIO iio( &buf, "XPM" );
    iio.setImage( img );
    iio.write();
    buf.close();
    ulong len = ba.size() * 2;
    QByteArray bazip( len );
    ::compress(  (uchar*) bazip.data(), &len, (uchar*) ba.data(), ba.size() );
    QString res;
    ts << makeIndent( indent ) << "<data format=\"XPM.GZ\" length=\"" << ba.size() << "\">";
    static const char hexchars[] = "0123456789abcdef";
    for ( int i = 0; i < (int)len; ++i ) {
	uchar s = (uchar) bazip[i];
	ts << hexchars[s >> 4];
	ts << hexchars[s & 0x0f];
    }
    ts << "</data>" << endl;
}

void Resource::saveImageCollection( QTextStream &ts, int indent )
{
    ts << makeIndent( indent ) << "<images>" << endl;
    indent++;

    QValueList<Image>::Iterator it = images.begin();
    for ( ; it != images.end(); ++it ) {
	ts << makeIndent( indent ) << "<image name=\"" << (*it).name << "\">" << endl;
	indent++;
	saveImageData( (*it).img, ts, indent );
	indent--;
	ts << makeIndent( indent ) << "</image>" << endl;
    }

    indent--;
    ts << makeIndent( indent ) << "</images>" << endl;
}

static QImage loadImageData( QDomElement &n2 )
{
    QImage img;
    QString data = n2.firstChild().toText().data();
    char *ba = new char[ data.length() / 2 ];
    for ( int i = 0; i < (int)data.length() / 2; ++i ) {
	char h = data[ 2 * i ].latin1();
	char l = data[ 2 * i  + 1 ].latin1();
	uchar r = 0;
	if ( h <= '9' )
	    r += h - '0';
	else
	    r += h - 'a' + 10;
	r = r << 4;
	if ( l <= '9' )
	    r += l - '0';
	else
	    r += l - 'a' + 10;
	ba[ i ] = r;
    }
    QString format = n2.attribute( "format", "PNG" );
    if ( format == "XPM.GZ" ) {
	ulong len = n2.attribute( "length" ).toULong();
	if ( len < data.length() * 5 )
	    len = data.length() * 5;
	QByteArray baunzip( len );
	::uncompress( (uchar*) baunzip.data(), &len, (uchar*) ba, data.length()/2 );
	img.loadFromData( (const uchar*)baunzip.data(), len, "XPM" );
    }  else {
	img.loadFromData( (const uchar*)ba, data.length() / 2, format );
    }
    delete [] ba;

    return img;
}

void Resource::loadImageCollection( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "image" ) {
	    Image img;
	    img.name =  n.attribute( "name" );
	    QDomElement n2 = n.firstChild().toElement();
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "data" )
		    img.img = loadImageData( n2 );
		n2 = n2.nextSibling().toElement();
	    }
	    images.append( img );
	    n = n.nextSibling().toElement();
	}
    }
}

QImage Resource::loadFromCollection( const QString &name )
{
    QValueList<Image>::Iterator it = images.begin();
    for ( ; it != images.end(); ++it ) {
	if ( ( *it ).name == name )
	    return ( *it ).img;
    }
    return QImage();
}

void Resource::saveConnections( QTextStream &ts, int indent )
{
    ts << makeIndent( indent ) << "<connections>" << endl;
    indent++;
    QValueList<MetaDataBase::Connection> connections = MetaDataBase::connections( formwindow );
    QValueList<MetaDataBase::Connection>::Iterator it = connections.begin();
    for ( ; it != connections.end(); ++it ) {
	MetaDataBase::Connection conn = *it;
	if ( ( knownNames.findIndex( QString( conn.sender->name() ) ) == -1 &&
	       qstrcmp( conn.sender->name(), "this" ) != 0 ) ||
	     ( knownNames.findIndex( QString( conn.receiver->name() ) ) == -1 &&
	       qstrcmp( conn.receiver->name(), "this" ) != 0 ) )
	    continue;
	if ( formwindow->isMainContainer( (QWidget*)(*it).receiver ) &&
	     !MetaDataBase::hasSlot( formwindow, MetaDataBase::normalizeSlot( (*it).slot ).latin1() ) )
	    continue;

	if ( conn.sender->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)conn.sender )->customWidget();
	    if ( cw && !cw->hasSignal( conn.signal ) )
		continue;
	}

	if ( conn.receiver->inherits( "CustomWidget" ) && !formwindow->isMainContainer( conn.receiver ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)conn.receiver )->customWidget();
	    if ( cw && !cw->hasSlot( MetaDataBase::normalizeSlot( conn.slot ).latin1() ) )
		continue;
	}

	ts << makeIndent( indent ) << "<connection>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<sender>" << entitize( conn.sender->name() ) << "</sender>" << endl;
	ts << makeIndent( indent ) << "<signal>" << entitize( conn.signal ) << "</signal>" << endl;
	ts << makeIndent( indent ) << "<receiver>" << entitize( conn.receiver->name() ) << "</receiver>" << endl;
	ts << makeIndent( indent ) << "<slot>" << entitize( MetaDataBase::normalizeSlot( conn.slot ) ) << "</slot>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</connection>" << endl;
    }

#ifndef KOMMANDER
    QString lang = formwindow->project()->language();
#else
    QString lang = "C++";
#endif
    LanguageInterface *iface = langIface;
    if ( iface && MetaDataBase::hasEvents( lang ) ) {
	QObjectList *l = formwindow->queryList( "QWidget" );
	l->append( formwindow );
	QPtrList<QAction> lst = formwindow->actionList();
	for ( QAction *a = lst.first(); a; a = lst.next() )
	    l->append( a );
	for ( QObject *o = l->first(); o; o = l->next() ) {
	    if ( !MetaDataBase::hasObject( o ) )
		continue;
	    QMap<QString, QStringList> eventFunctions = MetaDataBase::eventFunctions( o );
	    QMap<QString, QStringList>::ConstIterator it = eventFunctions.begin();
	    for ( ; it != eventFunctions.end(); ++it ) {
		QString sls = (*it).join( "," );
		MetaDataBase::Connection conn;
		conn.sender = o;
		conn.signal = it.key();
		conn.slot = sls;
		langConnections[ lang ].append( conn );
	    }
	}
	delete l;
    }

    indent--;
    ts << makeIndent( indent ) << "</connections>" << endl;
}

void Resource::loadConnections( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "connection" ) {
	    QString lang = n.attribute( "language", "C++" );
	    QDomElement n2 = n.firstChild().toElement();
	    MetaDataBase::Connection conn;
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "sender" ) {
		    conn.sender = 0;
		    QString name = n2.firstChild().toText().data();
		    if ( name == "this" || qstrcmp( toplevel->name(), name ) == 0 ) {
			conn.sender = toplevel;
		    } else {
			if ( name == "this" )
			    name = toplevel->name();
			QObjectList *l = toplevel->queryList( 0, name, FALSE );
			if ( l ) {
			    if ( l->first() )
				conn.sender = l->first();
			    delete l;
			}
			if ( !conn.sender )
			    conn.sender = formwindow->findAction(  name );
		    }
		} else if ( n2.tagName() == "signal" ) {
		    conn.signal = n2.firstChild().toText().data();
		} else if ( n2.tagName() == "receiver" ) {
		    QString name = n2.firstChild().toText().data();
		    if ( name == "this" || qstrcmp( toplevel->name(), name ) == 0 ) {
			conn.receiver = toplevel;
		    } else {
			QObjectList *l = toplevel->queryList( 0, name, FALSE );
			if ( l ) {
			    if ( l->first() )
				conn.receiver = l->first();
			    delete l;
			}
		    }
		} else if ( n2.tagName() == "slot" ) {
		    conn.slot = n2.firstChild().toText().data();
		}
		n2 = n2.nextSibling().toElement();
	    }
	    if ( formwindow ) {
		if ( conn.sender == formwindow )
		    conn.sender = formwindow->mainContainer();
		if ( conn.receiver == formwindow )
		    conn.receiver = formwindow->mainContainer();
	    }
	    if ( conn.sender && conn.receiver ) {
		if ( lang == "C++" ) {
		    MetaDataBase::addConnection( formwindow ? formwindow : toplevel,
						 conn.sender, conn.signal, conn.receiver, conn.slot );
		} else if ( MetaDataBase::hasEvents( lang ) ) {
		    MetaDataBase::setEventFunctions( conn.sender, formwindow, lang, conn.signal,
						     QStringList::split( ',', conn.slot ), FALSE );
		}
	    }
	} else if ( n.tagName() == "slot" ) { // compatibility with 2.x
	    MetaDataBase::Slot slot;
	    slot.specifier = n.attribute( "specifier", "virtual" );
	    if ( slot.specifier.isEmpty() )
		slot.specifier = "virtual";
	    slot.access = n.attribute( "access", "public" );
	    if ( slot.access.isEmpty() )
		slot.access = "public";
	    slot.language = n.attribute( "language", "C++" );
	    slot.returnType = n.attribute( "returnType", "void" );
	    if ( slot.returnType.isEmpty() )
		slot.returnType = "void";
	    slot.slot = n.firstChild().toText().data();
	    if ( !MetaDataBase::hasSlot( formwindow, slot.slot, TRUE ) )
		MetaDataBase::addSlot( formwindow, slot.slot, slot.specifier,
				       slot.access, slot.language, slot.returnType );
	    else
		MetaDataBase::changeSlotAttributes( formwindow, slot.slot,
						    slot.specifier, slot.access,
						    slot.language, slot.returnType );
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::saveCustomWidgets( QTextStream &ts, int indent )
{
    ts << makeIndent( indent ) << "<customwidgets>" << endl;
    indent++;

    QPtrList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();
    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
	if ( usedCustomWidgets.findIndex( w->className ) == -1 )
	    continue;
	ts << makeIndent( indent ) << "<customwidget>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<class>" << w->className << "</class>" << endl;
	ts << makeIndent( indent ) << "<header location=\""
	   << ( w->includePolicy == MetaDataBase::CustomWidget::Local ? "local" : "global" )
	   << "\">" << w->includeFile << "</header>" << endl;
	ts << makeIndent( indent ) << "<sizehint>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<width>" << w->sizeHint.width() << "</width>" << endl;
	ts << makeIndent( indent ) << "<height>" << w->sizeHint.height() << "</height>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</sizehint>" << endl;
	ts << makeIndent( indent ) << "<container>" << (int)w->isContainer << "</container>" << endl;
	ts << makeIndent( indent ) << "<sizepolicy>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<hordata>" << (int)w->sizePolicy.horData() << "</hordata>" << endl;
	ts << makeIndent( indent ) << "<verdata>" << (int)w->sizePolicy.verData() << "</verdata>" << endl;
	ts << makeIndent( indent ) << "<horstretch>" << (int)w->sizePolicy.horStretch() << "</horstretch>" << endl;
	ts << makeIndent( indent ) << "<verstretch>" << (int)w->sizePolicy.verStretch() << "</verstretch>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</sizepolicy>" << endl;
	ts << makeIndent( indent ) << "<pixmap>" << saveInCollection( w->pixmap->convertToImage() ) << "</pixmap>" << endl;
	if ( !w->lstSignals.isEmpty() ) {
	    for ( QValueList<QCString>::Iterator it = w->lstSignals.begin(); it != w->lstSignals.end(); ++it )
		ts << makeIndent( indent ) << "<signal>" << entitize( *it ) << "</signal>" << endl;
	}
	if ( !w->lstSlots.isEmpty() ) {
	    for ( QValueList<MetaDataBase::Slot>::Iterator it = w->lstSlots.begin(); it != w->lstSlots.end(); ++it )
		ts << makeIndent( indent ) << "<slot access=\"" << (*it).access << "\" specifier=\""
		   << (*it).specifier << "\">" << entitize( (*it).slot ) << "</slot>" << endl;
	}
	if ( !w->lstProperties.isEmpty() ) {
	    for ( QValueList<MetaDataBase::Property>::Iterator it = w->lstProperties.begin(); it != w->lstProperties.end(); ++it )
		ts << makeIndent( indent ) << "<property type=\"" << (*it).type << "\">" << entitize( (*it).property ) << "</property>" << endl;
	}
	indent--;
	ts << makeIndent( indent ) << "</customwidget>" << endl;
    }


    indent--;
    ts << makeIndent( indent ) << "</customwidgets>" << endl;
}

void Resource::loadCustomWidgets( const QDomElement &e, Resource *r )
{
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "customwidget" ) {
	    QDomElement n2 = n.firstChild().toElement();
	    MetaDataBase::CustomWidget *w = new MetaDataBase::CustomWidget;
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "class" ) {
		    w->className = n2.firstChild().toText().data();
		} else if ( n2.tagName() == "header" ) {
		    w->includeFile = n2.firstChild().toText().data();
		    QString s = n2.attribute( "location" );
		    if ( s != "local" )
			w->includePolicy = MetaDataBase::CustomWidget::Global;
		    else
			w->includePolicy = MetaDataBase::CustomWidget::Local;
		} else if ( n2.tagName() == "sizehint" ) {
		    QDomElement n3 = n2.firstChild().toElement();
		    while ( !n3.isNull() ) {
			if ( n3.tagName() == "width" )
			    w->sizeHint.setWidth( n3.firstChild().toText().data().toInt() );
			else if ( n3.tagName() == "height" )
			    w->sizeHint.setHeight( n3.firstChild().toText().data().toInt() );
			n3 = n3.nextSibling().toElement();
		    }
		} else if ( n2.tagName() == "sizepolicy" ) {
		    QDomElement n3 = n2.firstChild().toElement();
		    while ( !n3.isNull() ) {
			if ( n3.tagName() == "hordata" )
			    w->sizePolicy.setHorData( (QSizePolicy::SizeType)n3.firstChild().toText().data().toInt() );
			else if ( n3.tagName() == "verdata" )
			    w->sizePolicy.setVerData( (QSizePolicy::SizeType)n3.firstChild().toText().data().toInt() );
			else if ( n3.tagName() == "horstretch" )
			    w->sizePolicy.setHorStretch( n3.firstChild().toText().data().toInt() );
			else if ( n3.tagName() == "verstretch" )
			    w->sizePolicy.setVerStretch( n3.firstChild().toText().data().toInt() );
			n3 = n3.nextSibling().toElement();
		    }
		} else if ( n2.tagName() == "pixmap" ) {
		    QPixmap pix;
		    if ( r ) {
			pix = r->loadPixmap( n2 );
		    } else {
			QDomElement n3 = n2.firstChild().toElement();
			QImage img;
			while ( !n3.isNull() ) {
			    if ( n3.tagName() == "data" ) {
				img = loadImageData( n3 );
			    }
			    n3 = n3.nextSibling().toElement();
			}
			pix.convertFromImage( img );
		    }
		    w->pixmap = new QPixmap( pix );
		} else if ( n2.tagName() == "signal" ) {
		    w->lstSignals.append( n2.firstChild().toText().data().latin1() );
		} else if ( n2.tagName() == "container" ) {
		    w->isContainer = (bool)n2.firstChild().toText().data().toInt();
		} else if ( n2.tagName() == "slot" ) {
		    MetaDataBase::Slot slot;
		    slot.slot = n2.firstChild().toText().data().latin1();
		    slot.access = n2.attribute( "access" );
		    w->lstSlots.append( slot );
		} else if ( n2.tagName() == "property" ) {
		    MetaDataBase::Property property;
		    property.property = n2.firstChild().toText().data().latin1();
		    property.type = n2.attribute( "type" );
		    w->lstProperties.append( property );
		}
		n2 = n2.nextSibling().toElement();
	    }
	    MetaDataBase::addCustomWidget( w );
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::saveTabOrder( QTextStream &ts, int indent )
{
    QWidgetList l = MetaDataBase::tabOrder( toplevel );
    if ( l.isEmpty() )
	return;

    ts << makeIndent( indent ) << "<tabstops>" << endl;
    indent++;

    for ( QWidget *w = l.first(); w; w = l.next() ) {
	if ( w->testWState( Qt::WState_ForceHide ) || knownNames.findIndex( w->name() ) == -1 )
	    continue;
	ts << makeIndent( indent ) << "<tabstop>" << w->name() << "</tabstop>" << endl;
    }

    indent--;
    ts << makeIndent( indent ) << "</tabstops>" << endl;
}

void Resource::loadTabOrder( const QDomElement &e )
{
    QWidget *last = 0;
    QDomElement n = e.firstChild().toElement();
    QWidgetList widgets;
    while ( !n.isNull() ) {
	if ( n.tagName() == "tabstop" ) {
	    QString name = n.firstChild().toText().data();
	    if ( name.isEmpty() )
		continue;
	    QObjectList *l = toplevel->queryList( 0, name, FALSE );
	    if ( l ) {
		if ( l->first() ) {
		    QWidget *w = (QWidget*)l->first();
		    widgets.append( w );
		    if ( last )
			toplevel->setTabOrder( last, w );
		    last = w;
		}
		delete l;
	    }
	}
	n = n.nextSibling().toElement();
    }

    if ( !widgets.isEmpty() )
	MetaDataBase::setTabOrder( toplevel, widgets );
}

void Resource::saveMetaInfoBefore( QTextStream &ts, int indent )
{
    MetaDataBase::MetaInfo info = MetaDataBase::metaInfo( formwindow );
    QString cn;
    if ( info.classNameChanged && !info.className.isEmpty() )
	cn = info.className;
    else
	cn = formwindow->name();
    ts << makeIndent( indent ) << "<class>" << entitize( cn ) << "</class>" << endl;
    if ( !info.comment.isEmpty() )
	ts << makeIndent( indent ) << "<comment>" << entitize( info.comment ) << "</comment>" << endl;
    if ( !info.author.isEmpty() )
	ts << makeIndent( indent ) << "<author>" << entitize( info.author ) << "</author>" << endl;
}

void Resource::saveMetaInfoAfter( QTextStream &ts, int indent )
{
    MetaDataBase::MetaInfo info = MetaDataBase::metaInfo( formwindow );
#ifndef KOMMANDER
    if ( !langIface || formwindow->project()->language() == "C++" ) {
#else
    if ( !langIface ) {
#endif
	QValueList<MetaDataBase::Include> includes = MetaDataBase::includes( formwindow );
	QString extensionInclude;
	bool needExtensionInclude = FALSE;
#ifndef KOMMANDER
	if ( langIface && formwindow->project()->language() == "C++"  &&
	     formwindow->formFile()->hasFormCode() ) {
	    extensionInclude = QFileInfo( currFileName ).fileName() + langIface->formCodeExtension();
	    needExtensionInclude = TRUE;
	}
#endif
	if ( !includes.isEmpty() || needExtensionInclude ) {
	    ts << makeIndent( indent ) << "<includes>" << endl;
	    indent++;
	
	    for ( QValueList<MetaDataBase::Include>::Iterator it = includes.begin(); it != includes.end(); ++it ) {
		ts << makeIndent( indent ) << "<include location=\"" << (*it).location
		   << "\" impldecl=\"" << (*it).implDecl << "\">" << (*it).header << "</include>" << endl;
		if ( needExtensionInclude )
		    needExtensionInclude = (*it).header != extensionInclude;
	    }
	
	    if ( needExtensionInclude )
		ts << makeIndent( indent ) << "<include location=\"local\" impldecl=\"in implementation\">"
		   << extensionInclude << "</include>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</includes>" << endl;
	}
	
	QStringList forwards = MetaDataBase::forwards( formwindow );
	if ( !forwards.isEmpty() ) {
	    ts << makeIndent( indent ) << "<forwards>" << endl;
	    indent++;
	    for ( QStringList::Iterator it2 = forwards.begin(); it2 != forwards.end(); ++it2 )
		ts << makeIndent( indent ) << "<forward>" << entitize( *it2 ) << "</forward>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</forwards>" << endl;
	}
	QStringList vars = MetaDataBase::variables( formwindow );
	if ( !vars.isEmpty() ) {
	    ts << makeIndent( indent ) << "<variables>" << endl;
	    indent++;
	    for ( QStringList::Iterator it3 = vars.begin(); it3 != vars.end(); ++it3 )
		ts << makeIndent( indent ) << "<variable>" << entitize( *it3 ) << "</variable>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</variables>" << endl;
	}
	QStringList sigs = MetaDataBase::signalList( formwindow );
	if ( !sigs.isEmpty() ) {
	    ts << makeIndent( indent ) << "<signals>" << endl;
	    indent++;
	    for ( QStringList::Iterator it3 = sigs.begin(); it3 != sigs.end(); ++it3 )
		ts << makeIndent( indent ) << "<signal>" << entitize( *it3 ) << "</signal>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</signals>" << endl;
	}
	QValueList<MetaDataBase::Slot> slotList = MetaDataBase::slotList( formwindow );
	if ( !slotList.isEmpty() ) {
	    ts << makeIndent( indent ) << "<slots>" << endl;
	    indent++;
#ifndef KOMMANDER
	    QString lang = formwindow->project()->language();
#else
	    QString lang = "C++";
#endif
	    QValueList<MetaDataBase::Slot>::Iterator it = slotList.begin();
	    for ( ; it != slotList.end(); ++it ) {
		MetaDataBase::Slot slot = *it;
		ts << makeIndent( indent ) << "<slot";
		if ( slot.access != "public" )
		    ts << " access=\"" << slot.access << "\"";
		if ( slot.specifier != "virtual" )
		    ts << " specifier=\"" << slot.specifier << "\"";
		if ( slot.language != "C++" )
		    ts << " language=\"" << slot.language<< "\"";
		if ( slot.returnType != "void" )
		    ts << " returnType=\"" << entitize( slot.returnType ) << "\"";
		ts << ">" << entitize( slot.slot ) << "</slot>" << endl;
	    }
	    indent--;
	    ts << makeIndent( indent ) << "</slots>" << endl;
	}
    }

    if ( formwindow && formwindow->savePixmapInline() )
	;
#ifndef KOMMANDER
    else if ( formwindow && formwindow->savePixmapInProject() )
	ts << makeIndent( indent ) << "<pixmapinproject/>" << endl;
#endif
    else
	ts << makeIndent( indent ) << "<pixmapfunction>" << formwindow->pixmapLoaderFunction() << "</pixmapfunction>" << endl;
    if ( !( exportMacro = MetaDataBase::exportMacro( formwindow->mainContainer() ) ).isEmpty() )
	ts << makeIndent( indent ) << "<exportmacro>" << exportMacro << "</exportmacro>" << endl;
    if ( formwindow )
	ts << makeIndent( indent ) << "<layoutdefaults spacing=\"" << formwindow->layoutDefaultSpacing()
	   << "\" margin=\"" << formwindow->layoutDefaultMargin() << "\"/>" << endl;
}

QColorGroup Resource::loadColorGroup( const QDomElement &e )
{
    QColorGroup cg;
    int r = -1;
    QDomElement n = e.firstChild().toElement();
    QColor col;
    while ( !n.isNull() ) {
	if ( n.tagName() == "color" ) {
	    r++;
	    cg.setColor( (QColorGroup::ColorRole)r, (col = DomTool::readColor( n ) ) );
	} else if ( n.tagName() == "pixmap" ) {
	    QPixmap pix = loadPixmap( n );
	    cg.setBrush( (QColorGroup::ColorRole)r, QBrush( col, pix ) );
	}
	n = n.nextSibling().toElement();
    }
    return cg;
}

void Resource::saveChildActions( QAction *a, QTextStream &ts, int indent )
{
    if ( !a->children() )
	return;
    QObjectListIt it( *a->children() );
    while ( it.current() ) {
	QObject *o = it.current();
	++it;
	if ( !o->inherits( "QAction" ) )
	    continue;
	QAction *ac = (QAction*)o;
	bool isGroup = ac->inherits( "QActionGroup" );
	if ( isGroup )
	    ts << makeIndent( indent ) << "<actiongroup>" << endl;
	else
	    ts << makeIndent( indent ) << "<action>" << endl;
	indent++;
	saveObjectProperties( ac, ts, indent );
	indent--;
	if ( isGroup ) {
	    indent++;
	    saveChildActions( ac, ts, indent );
	    indent--;
	}
	if ( isGroup )
	    ts << makeIndent( indent ) << "</actiongroup>" << endl;
	else
	    ts << makeIndent( indent ) << "</action>" << endl;
    }
}

void Resource::saveActions( const QPtrList<QAction> &actions, QTextStream &ts, int indent )
{
    if ( actions.isEmpty() )
	return;
    ts << makeIndent( indent ) << "<actions>" << endl;
    indent++;
    QPtrListIterator<QAction> it( actions );
    while ( it.current() ) {
	QAction *a = it.current();
	bool isGroup = a->inherits( "QActionGroup" );
	if ( isGroup )
	    ts << makeIndent( indent ) << "<actiongroup>" << endl;
	else
	    ts << makeIndent( indent ) << "<action>" << endl;
	indent++;
	saveObjectProperties( a, ts, indent );
	indent--;
	if ( isGroup ) {
	    indent++;
	    saveChildActions( a, ts, indent );
	    indent--;
	}
	if ( isGroup )
	    ts << makeIndent( indent ) << "</actiongroup>" << endl;
	else
	    ts << makeIndent( indent ) << "</action>" << endl;
	++it;
    }
    indent--;
    ts << makeIndent( indent ) << "</actions>" << endl;
}

void Resource::loadChildAction( QObject *parent, const QDomElement &e )
{
    QDomElement n = e;
    QAction *a = 0;
    if ( n.tagName() == "action" ) {
	a = new QDesignerAction( parent );
	MetaDataBase::addEntry( a );
	QDomElement n2 = n.firstChild().toElement();
	while ( !n2.isNull() ) {
	    if ( n2.tagName() == "property" ) {
		setObjectProperty( a, n2.attribute( "name" ), n2.firstChild().toElement() );
	    } else if ( n2.tagName() == "event" ) {
#ifndef KOMMANDER
		MetaDataBase::setEventFunctions( a, formwindow, MainWindow::self->currProject()->language(),
#else
		MetaDataBase::setEventFunctions( a, formwindow, "C++",
#endif
						 n2.attribute( "name" ),
						 QStringList::split( ',', n2.attribute( "functions" ) ), FALSE );
	    }
	    n2 = n2.nextSibling().toElement();
	}
	if ( !parent->inherits( "QAction" ) )
	    formwindow->actionList().append( a );
    } else if ( n.tagName() == "actiongroup" ) {
	a = new QDesignerActionGroup( parent );
	MetaDataBase::addEntry( a );
	QDomElement n2 = n.firstChild().toElement();
	while ( !n2.isNull() ) {
	    if ( n2.tagName() == "property" ) {
		setObjectProperty( a, n2.attribute( "name" ), n2.firstChild().toElement() );
	    } else if ( n2.tagName() == "action" ||
			n2.tagName() == "actiongroup" ) {
		loadChildAction( a, n2 );
	    } else if ( n2.tagName() == "event" ) {
#ifndef KOMMANDER
		MetaDataBase::setEventFunctions( a, formwindow, MainWindow::self->currProject()->language(),
#else
		MetaDataBase::setEventFunctions( a, formwindow, "C++",
#endif
						 n2.attribute( "name" ),
						 QStringList::split( ',', n2.attribute( "functions" ) ), FALSE );
	    }
	    n2 = n2.nextSibling().toElement();
	}
	if ( !parent->inherits( "QAction" ) )
	    formwindow->actionList().append( a );
    }
}

void Resource::loadActions( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "action" ) {
	    loadChildAction( formwindow, n );
	} else if ( n.tagName() == "actiongroup" ) {
	    loadChildAction( formwindow, n );
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::saveToolBars( QMainWindow *mw, QTextStream &ts, int indent )
{
    ts << makeIndent( indent ) << "<toolbars>" << endl;
    indent++;

    QPtrList<QToolBar> tbList;
    for ( int i = 0; i <= (int)Qt::DockMinimized; ++i ) {
	tbList = mw->toolBars( (Qt::Dock)i );
	if ( tbList.isEmpty() )
	    continue;
	for ( QToolBar *tb = tbList.first(); tb; tb = tbList.next() ) {
	    if ( tb->isHidden() )
		continue;
	    ts << makeIndent( indent ) << "<toolbar dock=\"" << i << "\">" << endl;
	    indent++;
	    saveObjectProperties( tb, ts, indent );
	    QPtrList<QAction> actionList = ( (QDesignerToolBar*)tb )->insertedActions();
	    for ( QAction *a = actionList.first(); a; a = actionList.next() ) {
		if ( a->inherits( "QSeparatorAction" ) ) {
		    ts << makeIndent( indent ) << "<separator/>" << endl;
		} else {
		    if ( a->inherits( "QDesignerAction" ) && !( (QDesignerAction*)a )->supportsMenu() ) {
			QWidget *w = ( (QDesignerAction*)a )->widget();
			ts <<  makeIndent( indent ) << "<widget class=\""
			   << WidgetFactory::classNameOf( w ) << "\">" << endl;
			indent++;
			saveObjectProperties( w, ts, indent );
			const char *className = WidgetFactory::classNameOf( w );
			if ( w->isA( "CustomWidget" ) )
			    usedCustomWidgets << QString( className );
			if ( WidgetFactory::hasItems( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) )
			    saveItems( w, ts, indent );
			indent--;
			ts << makeIndent( indent ) << "</widget>" << endl;
		    } else {
			ts <<  makeIndent( indent ) << "<action name=\"" << a->name() << "\"/>" << endl;
		    }
		}
	    }
	    indent--;
	    ts << makeIndent( indent ) << "</toolbar>" << endl;
	}
    }
    indent--;
    ts << makeIndent( indent ) << "</toolbars>" << endl;
}

void Resource::saveMenuBar( QMainWindow *mw, QTextStream &ts, int indent )
{
    if ( !mw->child( 0, "QMenuBar" ) )
	return;
    ts << makeIndent( indent ) << "<menubar>" << endl;
    indent++;
    saveObjectProperties( mw->menuBar(), ts, indent );

    for ( int i = 0; i < (int)mw->menuBar()->count(); ++i ) {
	ts << makeIndent( indent ) << "<item text=\"" << entitize( mw->menuBar()->text( mw->menuBar()->idAt( i ) ) )
	   << "\" name=\"" << entitize( mw->menuBar()->findItem( mw->menuBar()->idAt( i ) )->popup()->name() ) << "\">" << endl;
	indent++;
	QMenuItem *m = mw->menuBar()->findItem( mw->menuBar()->idAt( i ) );
	if ( !m )
	    continue;
	QPtrList<QAction> actionList = ( (QDesignerPopupMenu*)m->popup() )->insertedActions();
	for ( QAction *a = actionList.first(); a; a = actionList.next() ) {
	    if ( a->inherits( "QSeparatorAction" ) )
		ts <<  makeIndent( indent ) << "<separator/>" << endl;
	    else
		ts <<  makeIndent( indent ) << "<action name=\"" << a->name() << "\"/>" << endl;
	}
	indent--;
	ts << makeIndent( indent ) << "</item>" << endl;
    }
    indent--;
    ts << makeIndent( indent ) << "</menubar>" << endl;
}

void Resource::loadToolBars( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    QMainWindow *mw = ( (QMainWindow*)formwindow->mainContainer() );
    QDesignerToolBar *tb = 0;
    while ( !n.isNull() ) {
	if ( n.tagName() == "toolbar" ) {
	    Qt::Dock dock = (Qt::Dock)n.attribute( "dock" ).toInt();
	    tb = new QDesignerToolBar( mw, dock );
	    QDomElement n2 = n.firstChild().toElement();
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "action" ) {
		    QAction *a = formwindow->findAction( n2.attribute( "name" ) );
		    if ( a ) {
			a->addTo( tb );
			tb->addAction( a );
		    }
		} else if ( n2.tagName() == "separator" ) {
		    QAction *a = new QSeparatorAction( 0 );
		    a->addTo( tb );
		    tb->addAction( a );
		} else if ( n2.tagName() == "widget" ) {
		    QWidget *w = (QWidget*)createObject( n2, tb );
		    QDesignerAction *a = new QDesignerAction( w, tb );
		    a->addTo( tb );
		    tb->addAction( a );
		    tb->installEventFilters( w );
		} else if ( n2.tagName() == "property" ) {
		    setObjectProperty( tb, n2.attribute( "name" ), n2.firstChild().toElement() );
		}
		n2 = n2.nextSibling().toElement();
	    }
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::loadMenuBar( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    QMainWindow *mw = ( (QMainWindow*)formwindow->mainContainer() );
    QDesignerMenuBar *mb = new QDesignerMenuBar( mw );
    while ( !n.isNull() ) {
	if ( n.tagName() == "item" ) {
	    QDesignerPopupMenu *popup = new QDesignerPopupMenu( mw );
	    popup->setName( n.attribute( "name" ) );
	    QDomElement n2 = n.firstChild().toElement();
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "action" ) {
		    QAction *a = formwindow->findAction( n2.attribute( "name" ) );
		    if ( a ) {
			a->addTo( popup );
			popup->addAction( a );
		    }
		} else if ( n2.tagName() == "separator" ) {
		    QAction *a = new QSeparatorAction( 0 );
		    a->addTo( popup );
		    popup->addAction( a );
		}
		n2 = n2.nextSibling().toElement();
	    }
	    mb->insertItem( n.attribute( "text" ), popup );
	} else if ( n.tagName() == "property" ) {
	    setObjectProperty( mb, n.attribute( "name" ), n.firstChild().toElement() );
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::saveFormCode()
{
#ifndef KOMMANDER
	QString lang = formwindow->project()->language();
#else
	QString lang = "C++";
#endif
    LanguageInterface *iface = langIface;
    if ( !iface )
	return;
    QMap<QString, QString> functionBodies = MetaDataBase::functionBodies( formwindow );
    if ( langIface->supports( LanguageInterface::StoreFormCodeSeperate ) ) {
	if ( formwindow->formFile()->hasTempFileName() ||
	     formwindow->formFile()->code().isEmpty() ||
	     !formwindow->formFile()->hasFormCode() )
	    return;
#ifndef KOMMANDER
	QFile f( formwindow->project()->makeAbsolute( formwindow->formFile()->codeFile() ) );
#else
	QFile f( formwindow->formFile()->codeFile() );
#endif
	if ( f.open( IO_WriteOnly | IO_Translate ) ) {
	    QTextStream ts( &f );
	    ts << formwindow->formFile()->code();
	}
    } else {
	QValueList<LanguageInterface::Function> funcs;
	QValueList<MetaDataBase::Slot> slotList = MetaDataBase::slotList( formwindow );
	QValueList<MetaDataBase::Slot>::Iterator sit = slotList.begin();
	for ( ; sit != slotList.end(); ++sit ) {
	    MetaDataBase::Slot slot = *sit;
	    QMap<QString, QString>::Iterator it =
		functionBodies.find( MetaDataBase::normalizeSlot( (*sit).slot ) );
	    LanguageInterface::Function func;
	    func.name = slot.slot;
	    func.body = *it;
	    func.comments = MetaDataBase::functionComments( formwindow, func.name );
	    func.returnType = slot.returnType;
	    funcs.append( func );
	}

	QValueList<LanguageInterface::Connection> conns;
	QValueList<MetaDataBase::Connection> mconns = langConnections[ lang ];
	for ( QValueList<MetaDataBase::Connection>::Iterator it = mconns.begin();
	      it != mconns.end(); ++it ) {
	    LanguageInterface::Connection conn;
	    conn.sender = (*it).sender->name();
	    conn.signal = (*it).signal;
	    conn.slot = (*it).slot;
	    conns.append( conn );
	}

	iface->saveFormCode( formwindow->name(), currFileName + iface->formCodeExtension(),
			     funcs, QStringList(), QStringList(), QStringList(),
			     MetaDataBase::variables( formwindow ), conns );
    }
}

 // compatibility with early 3.0 betas

static QString make_function_pretty( const QString &s )
{
    QString res = s;
    if ( res.find( ")" ) - res.find( "(" ) == 1 )
	return res;
    res.replace( QRegExp( "[(]" ), "( " );
    res.replace( QRegExp( "[)]" ), " )" );
    res.replace( QRegExp( "&" ), " &" );
    res.replace( QRegExp( "[*]" ), " *" );
    res.replace( QRegExp( "," ), ", " );
    res.replace( QRegExp( ":" ), " : " );
    res = res.simplifyWhiteSpace();
    return res;
}

void Resource::loadFunctions( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    QMap<QString, QString> bodies;
    while ( !n.isNull() ) {
	if ( n.tagName() == "function" ) {
	    QString name = n.attribute( "name" );
	    QString body = n.firstChild().toText().data();
	    bodies.insert( name, body );
	}
	n = n.nextSibling().toElement();
    }
    MetaDataBase::setFunctionBodies( formwindow, bodies, QString::null, QString::null );
    if ( !bodies.isEmpty() ) {
	LanguageInterface *iface = langIface;
	if ( !iface )
	    return;
	QString code;
	QValueList<MetaDataBase::Slot> slotList = MetaDataBase::slotList( formwindow );
	for ( QValueList<MetaDataBase::Slot>::Iterator it = slotList.begin(); it != slotList.end(); ++it ) {
#ifndef KOMMANDER
	    if ( (*it).language != formwindow->project()->language() )
#else
	    if ( (*it).language != "C++" )
#endif
		continue;
	    QString sl( (*it).slot );
	    QString comments = MetaDataBase::functionComments( formwindow, sl );
	    if ( !comments.isEmpty() )
		code += comments + "\n";
	    code += iface->createFunctionStart( formwindow->name(), make_function_pretty( sl ),
					       ( (*it).returnType.isEmpty() ?
						 QString( "void" ) :
						 (*it).returnType ) );
	    QMap<QString, QString>::Iterator bit = bodies.find( MetaDataBase::normalizeSlot( (*it).slot ) );
	    if ( bit != bodies.end() )
		code += "\n" + *bit + "\n\n";
	    else
		code += "\n" + iface->createEmptyFunction() + "\n\n";
	}
	if ( !code.isEmpty() ) {
	    formwindow->formFile()->setCode( code );
	    hasFunctions = TRUE;
	}
    }
	
}

void Resource::loadExtraSource()
{
#ifndef KOMMANDER
	QString lang = MainWindow::self->currProject()->language();
#else
	QString lang = "C++";
#endif
    LanguageInterface *iface = langIface;
    if ( hasFunctions || !iface )
	return;
    QValueList<LanguageInterface::Function> functions;
    QStringList forwards;
    QStringList includesImpl;
    QStringList includesDecl;
    QStringList vars;
    QValueList<LanguageInterface::Connection> connections;

    iface->loadFormCode( formwindow->name(), currFileName + iface->formCodeExtension(),
			 functions, forwards, includesImpl, includesDecl, vars, connections );

    if ( iface->supports( LanguageInterface::StoreFormCodeSeperate ) ) {
#ifndef KOMMANDER
	QFile f( formwindow->project()->makeAbsolute( formwindow->formFile()->codeFile() ) );
#else
	QFile f( formwindow->formFile()->codeFile()  );
#endif
	QString code;
	if ( f.open( IO_ReadOnly ) ) {
	    QTextStream ts( &f );
	    code = ts.read();
	}
	formwindow->formFile()->setCode( code );
    }

    for ( QValueList<LanguageInterface::Connection>::Iterator cit = connections.begin();
	  cit != connections.end(); ++cit ) {
	QObject *sender  = 0;
	QString name = (*cit).sender;
	if ( name == "this" || qstrcmp( toplevel->name(), name ) == 0 ) {
	    sender = ( (FormWindow*)toplevel )->mainContainer();
	} else {
	    if ( name == "this" )
		name = toplevel->name();
	    QObjectList *l = toplevel->queryList( 0, name, FALSE );
	    if ( l ) {
		if ( l->first() )
		    sender = l->first();
		delete l;
	    }
	    if ( !sender )
		sender = formwindow->findAction(  name );
	}
	MetaDataBase::setEventFunctions( sender, formwindow, lang, (*cit).signal,
					 QStringList::split( ',', (*cit).slot ), FALSE );
    }

    QMap<QString, QString> bodies = MetaDataBase::functionBodies( formwindow );

    for ( QValueList<LanguageInterface::Function>::Iterator fit = functions.begin();
	  fit != functions.end(); ++fit ) {
	if ( MetaDataBase::hasSlot( formwindow, (*fit).name.latin1() ) )
	    MetaDataBase::changeSlot( formwindow, (*fit).name.latin1(), (*fit).name.latin1() );
	else
	    MetaDataBase::addSlot( formwindow, (*fit).name.latin1(), "virtual", "public", lang, (*fit).returnType );
	MetaDataBase::setFunctionComments( formwindow, (*fit).name, (*fit).comments );
	bodies.insert( MetaDataBase::normalizeSlot( (*fit).name ), (*fit).body );
    }
    MetaDataBase::setFunctionBodies( formwindow, bodies, QString::null, QString::null );

    QStringList v = MetaDataBase::variables( formwindow );
    QStringList::Iterator vit;
    for ( vit = vars.begin(); vit != vars.end(); ++vit )
	v << *vit;
    MetaDataBase::setVariables( formwindow, v );

    QStringList f = MetaDataBase::forwards( formwindow );
    for ( vit = forwards.begin(); vit != forwards.end(); ++vit )
	f << *vit;
    MetaDataBase::setForwards( formwindow, f );

    QValueList<MetaDataBase::Include> incls = MetaDataBase::includes( formwindow );
    for ( vit = includesImpl.begin(); vit != includesImpl.end(); ++vit ) {
	MetaDataBase::Include inc;
	inc.header = *vit;
	inc.location = "in implementation";
	incls << inc;
    }
    for ( vit = includesDecl.begin(); vit != includesDecl.end(); ++vit ) {
	MetaDataBase::Include inc;
	inc.header = *vit;
	inc.location = "in declaration";
	incls << inc;
    }
    MetaDataBase::setIncludes( formwindow, incls );
}
