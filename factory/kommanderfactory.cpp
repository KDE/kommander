/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
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
/* Modifications by Marc Britton (c) 2002 under GNU GPL, terms as above */

#include <kconfig.h>
#include <klibloader.h>

#include "kommanderfactory.h"
#include <kommanderplugin.h>


#include <qfeatures.h>
#include "config.h"
#ifndef QT_NO_SQL
#include "database2.h"
#endif
#include <qdom.h>
#include <qdir.h>
#include <qlayout.h>
#include <qmetaobject.h>
#include "domtool.h"
#include <qapplication.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qobjectlist.h>
#include <private/qpluginmanager_p.h>
#include <qmime.h>
#include <qdragobject.h>
#include <zlib.h>

#ifndef QT_NO_SQL
#include <qsqlrecord.h>
#include <qsqldatabase.h>
#include <qdatatable.h>
#endif

// include all Qt widgets we support
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qiconview.h>
#include <qheader.h>
#ifndef QT_NO_TABLE
#include <qtable.h>
#endif
#include <qlistbox.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qmultilineedit.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qwizard.h>
#include <qlcdnumber.h>
#include <qprogressbar.h>
#include <qtextview.h>
#include <qtextbrowser.h>
#include <qdial.h>
#include <qslider.h>
#include <qframe.h>
#include <qwidgetstack.h>
#include <qtextedit.h>
#include <qscrollbar.h>
#include <qmainwindow.h>
#include <qsplitter.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qdatetimeedit.h>
#include <qtoolbox.h>

#include <stdlib.h>
#include <kglobal.h>
#include <klocale.h>

QPtrList<KommanderPlugin> widgetPlugins;

QMap<QWidget*, QString> *qwf_functions = 0;
QMap<QWidget*, QString> *qwf_forms = 0;
QString *qwf_language = 0;
bool qwf_execute_code = true;
bool qwf_stays_on_top = false;
QString *qwf_currFileName = 0L; //is this really used?

KommanderFactory::KommanderFactory()
    : dbControls( 0 ), usePixmapCollection( false )
{
    widgetPlugins.setAutoDelete( true );
    defSpacing = 6;
    defMargin = 11;
    if (!qwf_currFileName)
      qwf_currFileName = new QString();
}

KommanderFactory::~KommanderFactory()
{
  delete qwf_currFileName;
  qwf_currFileName = 0L;
}

QWidget *KommanderFactory::create( const QString &uiFile, QObject *connector, QWidget *parent, const char *name )
{
    QFile f( uiFile );
    if ( !f.open( IO_ReadOnly ) )
      return 0;

    if (!qwf_currFileName)
      qwf_currFileName = new QString();
    *qwf_currFileName = uiFile;
    QWidget *w = KommanderFactory::create( &f, connector, parent, name );
    if ( !qwf_forms )
	qwf_forms = new QMap<QWidget*, QString>;
    qwf_forms->insert( w, uiFile );
    return w;
}

QWidget *KommanderFactory::create( QIODevice *dev, QObject *connector, QWidget *parent, const char *name )
{
    QDomDocument doc;
    QString errMsg;
    int errLine;
    if ( !doc.setContent( dev, &errMsg, &errLine ) ) {
	qDebug( QString("Parse error: ") + errMsg + QString(" in line %d"), errLine );
	return 0;
    }

    DomTool::fixDocument( doc );

    KommanderFactory *widgetFactory = new KommanderFactory;
    widgetFactory->toplevel = 0;

    QDomElement e = doc.firstChild().toElement().firstChild().toElement();

    QDomElement variables = e;
    while ( variables.tagName() != "variables" && !variables.isNull() )
	variables = variables.nextSibling().toElement();

    QDomElement eltSlots = e;
    while ( eltSlots.tagName() != "slots" && !eltSlots.isNull() )
	eltSlots = eltSlots.nextSibling().toElement();

    QDomElement connections = e;
    while ( connections.tagName() != "connections" && !connections.isNull() )
	connections = connections.nextSibling().toElement();

    QDomElement imageCollection = e;
    while ( imageCollection.tagName() != "images" && !imageCollection.isNull() )
	imageCollection = imageCollection.nextSibling().toElement();

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
	} else if ( e.tagName() == "variable" ) { // compatibility with old betas
	    widgetFactory->variables << e.firstChild().toText().data();
	} else if ( e.tagName() == "pixmapinproject" ) {
	    widgetFactory->usePixmapCollection = true;
	} else if ( e.tagName() == "layoutdefaults" ) {
	    widgetFactory->defSpacing = e.attribute( "spacing", QString::number( widgetFactory->defSpacing ) ).toInt();
	    widgetFactory->defMargin = e.attribute( "margin", QString::number( widgetFactory->defMargin ) ).toInt();
	}
	e = e.nextSibling().toElement();
    }

    if ( !imageCollection.isNull() )
	widgetFactory->loadImageCollection( imageCollection );

    widgetFactory->createWidgetInternal( widget, parent, 0, widget.attribute("class", "QWidget") );
    QWidget *w = widgetFactory->toplevel;
    if ( !w ) {
	delete widgetFactory;
	return 0;
    }

    if ( !variables.isNull() ) {
	for ( QDomElement n = variables.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
	    if ( n.tagName() == "variable" )
		widgetFactory->variables << n.firstChild().toText().data();
    }
    if ( !eltSlots.isNull() ) {
	for ( QDomElement n = eltSlots.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
	    if ( n.tagName() == "slot" ) {
		QString s = n.firstChild().toText().data();
		widgetFactory->languageSlots.insert( s.left( s.find( "(" ) ) , n.attribute( "language", "C++" ) );
	    }
    }

    if ( !actions.isNull() )
	widgetFactory->loadActions( actions );
    if ( !toolbars.isNull() )
	widgetFactory->loadToolBars( toolbars );
    if ( !menubar.isNull() )
	widgetFactory->loadMenuBar( menubar );

    if ( !connections.isNull() )
	widgetFactory->loadConnections( connections, connector );
    if ( w && name && qstrlen( name ) > 0 )
	w->setName( name );

    if ( !tabOrder.isNull() )
	widgetFactory->loadTabOrder( tabOrder );


    if ( widgetFactory->toplevel ) {
#ifndef QT_NO_SQL
	QMap<QWidget*, SqlWidgetConnection>::Iterator cit = widgetFactory->sqlWidgetConnections.begin();
	for( ; cit != widgetFactory->sqlWidgetConnections.end(); ++cit ) {
	    if ( widgetFactory->noDatabaseWidgets.find( cit.key()->name() ) != widgetFactory->noDatabaseWidgets.end() )
		continue;
	    if ( cit.key()->inherits( "QDesignerDataBrowser2" ) )
		( (QDesignerDataBrowser2*)cit.key() )->initPreview( (*cit).conn, (*cit).table, cit.key(), *(*cit).dbControls );
	    else if ( cit.key()->inherits( "QDesignerDataView2" ) )
		( (QDesignerDataView2*)cit.key() )->initPreview( (*cit).conn, (*cit).table, cit.key(), *(*cit).dbControls );
	}

	for ( QMap<QString, QStringList>::Iterator it = widgetFactory->dbTables.begin(); it != widgetFactory->dbTables.end(); ++it ) {
	    QDataTable *table = (QDataTable*)widgetFactory->toplevel->child( it.key(), "QDataTable" );
	    if ( !table )
		continue;
	    if ( widgetFactory->noDatabaseWidgets.find( table->name() ) != widgetFactory->noDatabaseWidgets.end() )
		continue;
	    QValueList<Field> fieldMap = *widgetFactory->fieldMaps.find( table );
	    QString conn = (*it)[ 0 ];
	    QSqlCursor* c = 0;
	    QSqlDatabase *db = 0;
	    if ( conn.isEmpty() || conn == "(default)" ) {
		db = QSqlDatabase::database();
		c = new QSqlCursor( (*it)[ 1 ] );
	    } else {
		db = QSqlDatabase::database( conn );
		c = new QSqlCursor( (*it)[ 1 ], true, db );
	    }
	    if ( db ) {
		table->setSqlCursor( c, fieldMap.isEmpty(), true );
		table->refresh( QDataTable::RefreshAll );
	    }
	}
#endif

    }

    for ( QMap<QString, QString>::Iterator it = widgetFactory->buddies.begin(); it != widgetFactory->buddies.end(); ++it ) {
	QLabel *label = (QLabel*)widgetFactory->toplevel->child( it.key(), "QLabel" );
	QWidget *buddy = (QWidget*)widgetFactory->toplevel->child( *it, "QWidget" );
	if ( label && buddy )
	    label->setBuddy( buddy );
    }

    delete widgetFactory;

    QApplication::sendPostedEvents();

    return w;
}

void KommanderFactory::addPlugin( KommanderPlugin *plugin )
{
    widgetPlugins.append( plugin );
}

QWidget *KommanderFactory::createWidget( const QString &literalClassName, QWidget *parent, const char *name )
{
  QString className = literalClassName;

  // create widgets we know
  if (className == "QPushButton")
    return new QPushButton(parent, name);
  else if (className == "QToolButton")
    return new QToolButton(parent, name);
  else if (className == "QCheckBox")
    return new QCheckBox(parent, name);
  else if (className == "QRadioButton")
    return new QRadioButton(parent, name);
  else if (className == "QGroupBox")
    return new QGroupBox(parent, name);
  else if (className == "QButtonGroup")
    return new QButtonGroup(parent, name);
  else if (className == "QIconView")
  {
#if !defined(QT_NO_ICONVIEW)
    return new QIconView(parent, name);
#endif
  }
  else if (className == "QTable")
  {
#if !defined(QT_NO_TABLE)
    return new QTable(parent, name);
#endif
  }
  else if (className == "QListBox")
    return new QListBox(parent, name);
  else if (className == "QListView")
    return new QListView(parent, name);
  else if (className == "QLineEdit")
    return new QLineEdit(parent, name);
  else if (className == "QSpinBox")
    return new QSpinBox(parent, name);
  else if (className == "QMultiLineEdit")
    return new QMultiLineEdit(parent, name);
  else if (className == "QLabel")
    return new QLabel(parent, name);
  else if (className == "QLayoutWidget")
    return new QWidget(parent, name);
  else if (className == "QTabWidget")
    return new QTabWidget(parent, name);
  else if (className == "QComboBox")
    return new QComboBox(false, parent, name);
  else if (className == "QWidget")
  {
    if (!qwf_stays_on_top)
      return new QWidget(parent, name);
    return new QWidget(parent, name, Qt::WStyle_StaysOnTop);
  }
  else if (className == "QDialog")
  {
    if (!qwf_stays_on_top)
      return new QDialog(parent, name);
    return new QDialog(parent, name, false, Qt::WStyle_StaysOnTop);
  }
  else if (className == "QWizard")
    return new QWizard(parent, name);
  else if (className == "QLCDNumber")
    return new QLCDNumber(parent, name);
  else if (className == "QProgressBar")
    return new QProgressBar(parent, name);
  else if (className == "QTextView")
    return new QTextView(parent, name);
  else if (className == "QTextBrowser")
    return new QTextBrowser(parent, name);
  else if (className == "QDial")
    return new QDial(parent, name);
  else if (className == "QSlider")
    return new QSlider(parent, name);
  else if (className == "QFrame")
    return new QFrame(parent, name);
  else if (className == "QSplitter")
    return new QSplitter(parent, name);
  else if (className == "Line")
  {
    QFrame *f = new QFrame(parent, name);
    f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    return f;
  }
  else if (className == "QTextEdit")
    return new QTextEdit(parent, name);
  else if (className == "QDateEdit")
    return new QDateEdit(parent, name);
  else if (className == "QTimeEdit")
    return new QTimeEdit(parent, name);
  else if (className == "QDateTimeEdit")
    return new QDateTimeEdit(parent, name);
  else if (className == "QScrollBar")
    return new QScrollBar(parent, name);
  else if (className == "QPopupMenu")
    return new QPopupMenu(parent, name);
  else if (className == "QWidgetStack")
    return new QWidgetStack(parent, name);
  else if (className == "QMainWindow")
  {
    QMainWindow *mw = 0;
    if (!qwf_stays_on_top)
      mw = new QMainWindow(parent, name);
    else
      mw = new QMainWindow(parent, name, Qt::WType_TopLevel | Qt::WStyle_StaysOnTop);
    mw->setCentralWidget(new QWidget(mw, "qt_central_widget"));
    mw->centralWidget()->show();
    (void) mw->statusBar();
    return mw;
  }
#if !defined(QT_NO_SQL)
  else if (className == "QDataTable")
    return new QDataTable(parent, name);
  else if (className == "QDataBrowser")
    return new QDesignerDataBrowser2(parent, name);
  else if (className == "QDataView")
    return new QDesignerDataView2(parent, name);
#endif

  // try to create it using the loaded kommander widget plugins
  //find the widget plugin which can create className
  for (KommanderPlugin * p = widgetPlugins.first(); p; p = widgetPlugins.next())
  {
    QWidget *w = p->create(className, parent, name);
    if (w)
      return w;
  }

  // no success
  return 0;
}

static int num_plugins_loaded = 0;

int KommanderFactory::loadPlugins(bool force)
{
  if (num_plugins_loaded > 0 && !force)
    return num_plugins_loaded;

  num_plugins_loaded = 0;
  KConfig cfg("kommanderrc", true);
  QStringList plugins = "libkommanderwidgets";
  plugins += cfg.readListEntry("plugins");
  QStringList::Iterator it;
  KLibLoader *f = KLibLoader::self();
  for (it = plugins.begin(); it != plugins.end(); ++it)
  {
    KLibrary *l = f->library((*it).latin1());
    if (l)
    {
      if (l->hasSymbol("kommander_plugin"))
      {
        void *(*kommander_plugin) () = (void *(*)()) l->symbol("kommander_plugin");
        KommanderPlugin *p = (KommanderPlugin *) (*kommander_plugin) ();
        widgetPlugins.append(p);
        ++num_plugins_loaded;
      } else
      {
        qWarning("KommanderFactory::loadPlugins - '%s' isn't a Kommander Plugin library, skipping.",
            l->fileName().latin1());
      }
    } else
    {
      qWarning("KommanderFactory::loadPlugins - Can't load Kommander plugin library %s",
          (*it).latin1());
    }
  }
  //qDebug("KommanderFactory::loadPlugins returning %d", num_plugins_loaded);
  return num_plugins_loaded;
}

FeatureList KommanderFactory::featureList()
{
    FeatureList features;
    for ( KommanderPlugin *p = widgetPlugins.first(); p ; p = widgetPlugins.next() )
    {
	QStringList widgets = p->widgets();
	QStringList::Iterator it;
	for( it = widgets.begin() ; it != widgets.end() ; ++it )
	{
	    QString wn = *it;
	    features[wn] = KommanderWidgetInfo( p->group(wn), p->toolTip(wn),p->iconSet(wn), p->whatsThis(wn), p->isContainer(wn) );
	}
    }
    return features;
    //iterate through widgetPlugins, appending KommanderPlugin::widgets() to features
}

QWidget *KommanderFactory::createWidgetInternal( const QDomElement &e, QWidget *parent, QLayout* layout, const QString &classNameArg )
{
    lastItem = 0;
    QDomElement n = e.firstChild().toElement();
    QWidget *w = 0; // the widget that got created
    QObject *obj = 0; // gets the properties

    QString className = classNameArg;

    int row = e.attribute( "row" ).toInt();
    int col = e.attribute( "column" ).toInt();
    int rowspan = e.attribute( "rowspan" ).toInt();
    int colspan = e.attribute( "colspan" ).toInt();
    if ( rowspan < 1 )
	rowspan = 1;
    if ( colspan < 1 )
	colspan = 1;
    if ( !className.isEmpty() ) {
	if ( !layout && className  == "QLayoutWidget" )
	    className = "QWidget";
	if ( layout && className == "QLayoutWidget" ) {
	    // hide layout widgets
	    w = parent;
	} else {
	    obj = KommanderFactory::createWidget( className, parent, 0 );
	    if ( !obj )
	    {
		return 0;
	    }
	    w = (QWidget*)obj;
	    if ( !toplevel )
		toplevel = w;
	    if ( w->inherits( "QMainWindow" ) )
		w = ( (QMainWindow*)w )->centralWidget();
	    if ( layout ) {
		switch( layoutType( layout ) ) {
		case HBox:
		    ( (QHBoxLayout*)layout )->addWidget( w );
		    break;
		case VBox:
		    ( (QVBoxLayout*)layout )->addWidget( w );
		    break;
		case Grid:
		    ( (QGridLayout*)layout )->addMultiCellWidget( w, row, row + rowspan - 1,
								  col, col + colspan - 1 );
		    break;
		default:
		    break;
		}
	    }

	    layout = 0;
	}
    }

    while ( !n.isNull() ) {
	if ( n.tagName() == "spacer" ) {
	    createSpacer( n, layout );
	} else if ( n.tagName() == "widget" ) {
	    QMap< QString, QString> *oldDbControls = dbControls;
	    createWidgetInternal( n, w, layout, n.attribute( "class", "QWidget" ) );
	    dbControls = oldDbControls;
	} else if ( n.tagName() == "hbox" ) {
	    QLayout *parentLayout = layout;
	    if ( layout && layout->inherits( "QGridLayout" ) )
		layout = createLayout( 0, 0, KommanderFactory::HBox );
	    else
		layout = createLayout( w, layout, KommanderFactory::HBox );
	    obj = layout;
	    n = n.firstChild().toElement();
	    if ( parentLayout && parentLayout->inherits( "QGridLayout" ) )
		( (QGridLayout*)parentLayout )->addMultiCellLayout( layout, row, row + rowspan - 1, col, col + colspan - 1 );
	    continue;
	} else if ( n.tagName() == "grid" ) {
	    QLayout *parentLayout = layout;
	    if ( layout && layout->inherits( "QGridLayout" ) )
		layout = createLayout( 0, 0, KommanderFactory::Grid );
	    else
		layout = createLayout( w, layout, KommanderFactory::Grid );
	    obj = layout;
	    n = n.firstChild().toElement();
	    if ( parentLayout && parentLayout->inherits( "QGridLayout" ) )
		( (QGridLayout*)parentLayout )->addMultiCellLayout( layout, row, row + rowspan - 1, col, col + colspan - 1 );
	    continue;
	} else if ( n.tagName() == "vbox" ) {
	    QLayout *parentLayout = layout;
	    if ( layout && layout->inherits( "QGridLayout" ) )
		layout = createLayout( 0, 0, KommanderFactory::VBox );
	    else
		layout = createLayout( w, layout, KommanderFactory::VBox );
	    obj = layout;
	    n = n.firstChild().toElement();
	    if ( parentLayout && parentLayout->inherits( "QGridLayout" ) )
		( (QGridLayout*)parentLayout )->addMultiCellLayout( layout, row, row + rowspan - 1, col, col + colspan - 1 );
	    continue;
	} else if ( n.tagName() == "property" && obj ) {
	    setProperty( obj, n.attribute( "name" ), n.firstChild().toElement() );
	} else if ( n.tagName() == "attribute" && w ) {
	    QString attrib = n.attribute( "name" );
	    QVariant v = DomTool::elementToVariant( n.firstChild().toElement(), QVariant() );
	    if ( parent->inherits( "QTabWidget" ) ) {
		if ( attrib == "title" )
		    ( (QTabWidget*)parent )->insertTab( w, translate(v.toString()) );
	    } else
        if ( parent->inherits( "QToolBox" ) ) {
        if ( attrib == "label" )
            ( (QToolBox*)parent )->addItem( w, translate(v.toString()) );
        }else if ( parent->inherits( "QWizard" ) ) {
		if ( attrib == "title" )
		    ( (QWizard*)parent )->addPage( w, translate(v.toString()) );
	    }
	} else if ( n.tagName() == "item" ) {
	    createItem( n, w );
	} else if ( n.tagName() == "column" || n.tagName() == "row" ) {
	    createColumn( n, w );
	}

	n = n.nextSibling().toElement();
    }

    return w;
}

QLayout *KommanderFactory::createLayout( QWidget *widget, QLayout*  layout, LayoutType type )
{
    int spacing = defSpacing;
    int margin = defMargin;

    if ( !layout && widget && widget->inherits( "QTabWidget" ) )
	widget = ((QTabWidget*)widget)->currentPage();
    if ( !layout && widget && widget->inherits( "QToolBox" ) )
    widget = ((QToolBox*)widget)->currentItem();

    if ( !layout && widget && widget->inherits( "QWizard" ) )
	widget = ((QWizard*)widget)->currentPage();

    if ( !layout && widget && widget->inherits( "QWidgetStack" ) )
	widget = ((QWidgetStack*)widget)->visibleWidget();

    if ( !layout && widget && widget->inherits( "QGroupBox" ) ) {
	QGroupBox *gb = (QGroupBox*)widget;
	gb->setColumnLayout( 0, Qt::Vertical );
	gb->layout()->setMargin( 0 );
	gb->layout()->setSpacing( 0 );
	QLayout *l;
	switch ( type ) {
	case HBox:
	    l = new QHBoxLayout( gb->layout() );
	    l->setAlignment( Qt::AlignTop );
	    return l;
	case VBox:
	    l = new QVBoxLayout( gb->layout(), spacing );
	    l->setAlignment( Qt::AlignTop );
	    return l;
	case Grid:
	    l = new QGridLayout( gb->layout() );
	    l->setAlignment( Qt::AlignTop );
	    return l;
	default:
	    return 0;
	}
    } else {
	if ( layout ) {
	    QLayout *l;
	    switch ( type ) {
	    case HBox:
		l = new QHBoxLayout( layout );
		l->setSpacing( spacing );
		l->setMargin( margin );
		return l;
	    case VBox:
		l = new QVBoxLayout( layout );
		l->setSpacing( spacing );
		l->setMargin( margin );
		return l;
	    case Grid: {
		l = new QGridLayout( layout );
		l->setSpacing( spacing );
		l->setMargin( margin );
		return l;
	    }
	    default:
		return 0;
	    }
	} else {
	    QLayout *l;
	    switch ( type ) {
	    case HBox:
		l = new QHBoxLayout( widget );
		if ( !widget ) {
		    l->setMargin( margin );
		    l->setSpacing( spacing );
		}
		return l;
	    case VBox:
		l = new QVBoxLayout( widget );
		if ( !widget ) {
		    l->setMargin( margin );
		    l->setSpacing( spacing );
		}
		return l;
	    case Grid: {
		l = new QGridLayout( widget );
		if ( !widget ) {
		    l->setMargin( margin );
		    l->setSpacing( spacing );
		}
		return l;
	    }
	    default:
		return 0;
	    }
	}
    }
}

KommanderFactory::LayoutType KommanderFactory::layoutType( QLayout *layout ) const
{
    if ( layout->inherits( "QHBoxLayout" ) )
	return HBox;
    else if ( layout->inherits( "QVBoxLayout" ) )
	return VBox;
    else if ( layout->inherits( "QGridLayout" ) )
	return Grid;
    return NoLayout;
}

void KommanderFactory::setProperty( QObject* obj, const QString &prop, const QDomElement &e )
{
    const QMetaProperty *p = obj->metaObject()->property( obj->metaObject()->findProperty( prop, true ), true );

    QVariant defVariant;
    if ( e.tagName() == "font" ) {
	QFont f( qApp->font() );
	if ( obj->isWidgetType() && ( (QWidget*)obj )->parentWidget() )
	    f = ( (QWidget*)obj )->parentWidget()->font();
	defVariant = QVariant( f );
    }

    QString comment;
    QVariant v( DomTool::elementToVariant( e, defVariant, comment ) );

    if ( e.tagName() == "pixmap" ) {
	QPixmap pix = loadPixmap( e );
	if ( pix.isNull() )
	    return;
	v = QVariant( pix );
    } else if ( e.tagName() == "iconset" ) {
	QPixmap pix = loadPixmap( e );
	if ( pix.isNull() )
	    return;
	v = QVariant( QIconSet( pix ) );
    } else if ( e.tagName() == "image" ) {
	v = QVariant( loadFromCollection( v.toString() ) );
    } else if ( e.tagName() == "string" ) {
	v = QVariant( translate( v.asString(), comment ) );
    }

    if ( !p ) {
	if ( obj->isWidgetType() ) {
	    if ( prop == "toolTip" ) {
		if ( !v.toString().isEmpty() )
		    QToolTip::add( (QWidget*)obj, translate(v.toString()) );
	    } else if ( prop == "whatsThis" ) {
		if ( !v.toString().isEmpty() )
		    QWhatsThis::add( (QWidget*)obj, translate(v.toString()) );
	    }
#ifndef QT_NO_SQL
	    if ( prop == "database" && !obj->inherits( "QDataView" )
		 && !obj->inherits( "QDataBrowser" ) ) {
		QStringList lst = DomTool::elementToVariant( e, QVariant( QStringList() ) ).toStringList();
		if ( lst.count() > 2 ) {
		    if ( dbControls )
			dbControls->insert( obj->name(), lst[ 2 ] );
		} else if ( lst.count() == 2 ) {
		    dbTables.insert( obj->name(), lst );
		}
	    } else if ( prop == "database" ) {
		QStringList lst = DomTool::elementToVariant( e, QVariant( QStringList() ) ).toStringList();
		if ( lst.count() == 2 && obj->inherits( "QWidget" ) ) {
		    SqlWidgetConnection conn( lst[ 0 ], lst[ 1 ] );
		    sqlWidgetConnections.insert( (QWidget*)obj, conn );
		    dbControls = conn.dbControls;
		}
	    } else
#endif
		if ( prop == "buddy" ) {
		buddies.insert( obj->name(), v.toCString() );
	    } else if ( prop == "frameworkCode" ) {
		if ( !DomTool::elementToVariant( e, QVariant( true, 0 ) ).toBool() ) {
		    noDatabaseWidgets << obj->name();
		}
	    } else if ( prop == "buttonGroupId" ) {
		if ( obj->inherits( "QButton" ) && obj->parent()->inherits( "QButtonGroup" ) )
		    ( (QButtonGroup*)obj->parent() )->insert( (QButton*)obj, v.toInt() );
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

    if ( prop == "geometry" ) {
	if ( obj == toplevel ) {
	    toplevel->resize( v.toRect().size() );
	    return;
	}
    }

    obj->setProperty( prop, v );
}

void KommanderFactory::createSpacer( const QDomElement &e, QLayout *layout )
{
    QDomElement n = e.firstChild().toElement();
    int row = e.attribute( "row" ).toInt();
    int col = e.attribute( "column" ).toInt();
    int rowspan = e.attribute( "rowspan" ).toInt();
    int colspan = e.attribute( "colspan" ).toInt();

    Qt::Orientation orient = Qt::Horizontal;
    int w = 0, h = 0;
    QSizePolicy::SizeType sizeType = QSizePolicy::Preferred;
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" ) {
	    QString prop = n.attribute( "name" );
	    if ( prop == "orientation" ) {
		if ( n.firstChild().firstChild().toText().data() == "Horizontal" )
		    orient = Qt::Horizontal;
		else
		    orient = Qt::Vertical;
	    } else if ( prop == "sizeType" ) {
		if ( n.firstChild().firstChild().toText().data() == "Fixed" )
		    sizeType = QSizePolicy::Fixed;
		else if ( n.firstChild().firstChild().toText().data() == "Minimum" )
		    sizeType = QSizePolicy::Minimum;
		else if ( n.firstChild().firstChild().toText().data() == "Maximum" )
		    sizeType = QSizePolicy::Maximum;
		else if ( n.firstChild().firstChild().toText().data() == "Preferred" )
		    sizeType = QSizePolicy::Preferred;
		else if ( n.firstChild().firstChild().toText().data() == "MinimumExpanding" )
		    sizeType = QSizePolicy::MinimumExpanding;
		else if ( n.firstChild().firstChild().toText().data() == "Expanding" )
		    sizeType = QSizePolicy::Expanding;
	    } else if ( prop == "sizeHint" ) {
		w = n.firstChild().firstChild().firstChild().toText().data().toInt();
		h = n.firstChild().firstChild().nextSibling().firstChild().toText().data().toInt();
	    }
	}
	n = n.nextSibling().toElement();
    }

    if ( rowspan < 1 )
	rowspan = 1;
    if ( colspan < 1 )
	colspan = 1;
    QSpacerItem *item = new QSpacerItem( w, h, orient == Qt::Horizontal ? sizeType : QSizePolicy::Minimum,
					 orient == Qt::Vertical ? sizeType : QSizePolicy::Minimum );
    if ( layout ) {
	if ( layout->inherits( "QBoxLayout" ) )
	    ( (QBoxLayout*)layout )->addItem( item );
	else
	    ( (QGridLayout*)layout )->addMultiCell( item, row, row + rowspan - 1, col, col + colspan - 1,
						    orient == Qt::Horizontal ? Qt::AlignVCenter : Qt::AlignHCenter );
    }
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

void KommanderFactory::loadImageCollection( const QDomElement &e )
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

QImage KommanderFactory::loadFromCollection( const QString &name )
{
    QValueList<Image>::Iterator it = images.begin();
    for ( ; it != images.end(); ++it ) {
	if ( ( *it ).name == name )
	    return ( *it ).img;
    }
    return QImage();
}

QPixmap KommanderFactory::loadPixmap( const QDomElement &e )
{
    QString arg = e.firstChild().toText().data();
    if ( usePixmapCollection ) {
	const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( arg );
	if ( !m )
	    return QPixmap();
	QPixmap pix;
	QImageDrag::decode( m, pix );
	return pix;
    }

    QImage img = loadFromCollection( arg );
    QPixmap pix;
    pix.convertFromImage( img );
    return pix;
}

QColorGroup KommanderFactory::loadColorGroup( const QDomElement &e )
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

struct Connection
{
    QObject *sender, *receiver;
    QCString signal, slot;
    bool operator==( const Connection &c ) const {
	return sender == c.sender && receiver == c.receiver &&
	       signal == c.signal && slot == c.slot ;
    }
};

class NormalizeObject : public QObject
{
public:
    NormalizeObject() : QObject() {}
    static QCString normalizeSignalSlot( const char *signalSlot ) { return QObject::normalizeSignalSlot( signalSlot ); }
};

void KommanderFactory::loadConnections( const QDomElement &e, QObject *connector )
{
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "connection" ) {
	    QString lang = n.attribute( "language", "C++" );
	    QDomElement n2 = n.firstChild().toElement();
	    Connection conn;
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "sender" ) {
		    QString name = n2.firstChild().toText().data();
		    if ( name == "this" || qstrcmp( toplevel->name(), name ) == 0 ) {
			conn.sender = toplevel;
		    } else {
			if ( name == "this" )
			    name = toplevel->name();
			QObjectList *l = toplevel->queryList( 0, name, false );
			if ( l ) {
			    if ( l->first() )
				conn.sender = l->first();
			    delete l;
			}
		    }
		    if ( !conn.sender )
			conn.sender = findAction( name );
		} else if ( n2.tagName() == "signal" ) {
		    conn.signal = n2.firstChild().toText().data();
		} else if ( n2.tagName() == "receiver" ) {
		    QString name = n2.firstChild().toText().data();
		    if ( name == "this" || qstrcmp( toplevel->name(), name ) == 0 ) {
			conn.receiver = toplevel;
		    } else {
			QObjectList *l = toplevel->queryList( 0, name, false );
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

	    conn.signal = NormalizeObject::normalizeSignalSlot( conn.signal );
	    conn.slot = NormalizeObject::normalizeSignalSlot( conn.slot );

	    QObject *sender = 0, *receiver = 0;
	    QObjectList *l = toplevel->queryList( 0, conn.sender->name(), false );
	    if ( qstrcmp( conn.sender->name(), toplevel->name() ) == 0 ) {
		sender = toplevel;
	    } else {
		if ( !l || !l->first() ) {
		    delete l;
		    n = n.nextSibling().toElement();
		    continue;
		}
		sender = l->first();
		delete l;
	    }
	    if ( !sender )
		sender = findAction( conn.sender->name() );

	    if ( qstrcmp( conn.receiver->name(), toplevel->name() ) == 0 ) {
		receiver = toplevel;
	    } else {
		l = toplevel->queryList( 0, conn.receiver->name(), false );
		if ( !l || !l->first() ) {
		    delete l;
		    n = n.nextSibling().toElement();
		    continue;
		}
		receiver = l->first();
		delete l;
	    }

	    if ( lang == "C++" ) {
		QString s = "2""%1";
		s = s.arg( conn.signal );
		QString s2 = "1""%1";
		s2 = s2.arg( conn.slot );

		QStrList signalList = sender->metaObject()->signalNames( true );
		QStrList slotList = receiver->metaObject()->slotNames( true );

		// if this is a connection to a custom slot and we have a connector, try this as receiver
		if ( slotList.find( conn.slot ) == -1 && receiver == toplevel && connector ) {
		    slotList = connector->metaObject()->slotNames( true );
		    receiver = connector;
		}

		// avoid warnings
		if ( signalList.find( conn.signal ) == -1 ||
		     slotList.find( conn.slot ) == -1 ) {
		    n = n.nextSibling().toElement();
		    continue;
		}
		QObject::connect( sender, s, receiver, s2 );
	    } else {
		EventFunction ef = eventMap[ conn.sender ];
		ef.events.append( conn.signal );
		ef.functions.append( QStringList::split( ',', conn.slot ) );
		eventMap.replace( conn.sender, ef );
	    }
	} else if ( n.tagName() == "slot" ) {
	    QString s = n.firstChild().toText().data();
	    languageSlots.insert( s.left( s.find( "(" ) ) , n.attribute( "language" ) );
	}
	n = n.nextSibling().toElement();
    }
}

void KommanderFactory::loadTabOrder( const QDomElement &e )
{
    QWidget *last = 0;
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "tabstop" ) {
	    QString name = n.firstChild().toText().data();
	    QObjectList *l = toplevel->queryList( 0, name, false );
	    if ( l ) {
		if ( l->first() ) {
		    QWidget *w = (QWidget*)l->first();
		    if ( last )
			toplevel->setTabOrder( last, w );
		    last = w;
		}
		delete l;
	    }
	}
	n = n.nextSibling().toElement();
    }
}

void KommanderFactory::createColumn( const QDomElement &e, QWidget *widget )
{
    if ( widget->inherits( "QListView" ) && e.tagName() == "column" ) {
	QListView *lv = (QListView*)widget;
	QDomElement n = e.firstChild().toElement();
	QPixmap pix;
	bool hasPixmap = false;
	QString txt;
	bool clickable = true, resizeable = true;
	while ( !n.isNull() ) {
	    if ( n.tagName() == "property" ) {
		QString attrib = n.attribute( "name" );
		QVariant v = DomTool::elementToVariant( n.firstChild().toElement(), QVariant() );
		if ( attrib == "text" )
		    txt = translate(v.toString());
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
#ifndef QT_NO_SQL
	bool isSql = (widget->inherits( "QDataTable" ));
#endif
	bool isRow;
	if ( ( isRow = e.tagName() == "row" ) )
	    table->setNumRows( table->numRows() + 1 );
	else {
#ifndef QT_NO_SQL
	    if ( !isSql )
#endif
		table->setNumCols( table->numCols() + 1 );
	}

	QDomElement n = e.firstChild().toElement();
	QPixmap pix;
	bool hasPixmap = false;
	QString txt;
	QString field;
	QValueList<Field> fieldMap;
	if ( fieldMaps.find( table ) != fieldMaps.end() ) {
	    fieldMap = *fieldMaps.find( table );
	    fieldMaps.remove( table );
	}
	while ( !n.isNull() ) {
	    if ( n.tagName() == "property" ) {
		QString attrib = n.attribute( "name" );
		QVariant v = DomTool::elementToVariant( n.firstChild().toElement(), QVariant() );
		if ( attrib == "text" )
		    txt = translate(v.toString());
		else if ( attrib == "pixmap" ) {
		    hasPixmap = !n.firstChild().firstChild().toText().data().isEmpty();
		    if ( hasPixmap )
			pix = loadPixmap( n.firstChild().toElement().toElement() );
		} else if ( attrib == "field" )
		    field = v.toString();
	    }
	    n = n.nextSibling().toElement();
	}

	int i = isRow ? table->numRows() - 1 : table->numCols() - 1;
	QHeader *h = !isRow ? table->horizontalHeader() : table->verticalHeader();
	if ( hasPixmap ) {
#ifndef QT_NO_SQL
	    if ( isSql )
		((QDataTable*)table)->addColumn( field, txt, -1, pix );
	    else
#endif
		h->setLabel( i, pix, txt );
	} else {
#ifndef QT_NO_SQL
	    if ( isSql ) {
		((QDataTable*)table)->addColumn( field, txt );
	    }
	    else
#endif
		h->setLabel( i, txt );
	}
	if ( !isRow && !field.isEmpty() ) {
	    fieldMap.append( Field( txt, (hasPixmap ? pix : QPixmap()), field ) );
	    fieldMaps.insert( table, fieldMap );
	}
    }
#endif
}

void KommanderFactory::loadItem( const QDomElement &e, QPixmap &pix, QString &txt, bool &hasPixmap )
{
    QDomElement n = e;
    hasPixmap = false;
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" ) {
	    QString attrib = n.attribute( "name" );
	    QVariant v = DomTool::elementToVariant( n.firstChild().toElement(), QVariant() );
	    if ( attrib == "text" )
		txt = translate(v.toString());
	    else if ( attrib == "pixmap" ) {
		pix = loadPixmap( n.firstChild().toElement() );
		hasPixmap = !pix.isNull();
	    }
	}
	n = n.nextSibling().toElement();
    }
}

void KommanderFactory::createItem( const QDomElement &e, QWidget *widget, QListViewItem *i )
{
    if ( widget->inherits( "QListBox" ) || widget->inherits( "QComboBox" ) ) {
	QDomElement n = e.firstChild().toElement();
	QPixmap pix;
	bool hasPixmap = false;
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
#ifndef QT_NO_ICONVIEW
    } else if ( widget->inherits( "QIconView" ) ) {
	QDomElement n = e.firstChild().toElement();
	QPixmap pix;
	bool hasPixmap = false;
	QString txt;
	loadItem( n, pix, txt, hasPixmap );

	QIconView *iv = (QIconView*)widget;
	new QIconViewItem( iv, txt, pix );
#endif
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
		    textes << translate(v.toString());
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
		item->setOpen( true );
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



void KommanderFactory::loadChildAction( QObject *parent, const QDomElement &e )
{
    QDomElement n = e;
    QAction *a = 0;
    EventFunction ef;
    if ( n.tagName() == "action" ) {
	a = new QAction( parent );
	QDomElement n2 = n.firstChild().toElement();
	while ( !n2.isNull() ) {
	    if ( n2.tagName() == "property" ) {
		setProperty( a, n2.attribute( "name" ), n2.firstChild().toElement() );
	    } else if ( n2.tagName() == "event" ) {
		ef.events.append( n2.attribute( "name" ) );
		ef.functions.append( QStringList::split( ',', n2.attribute( "functions" ) ) );
	    }
	    n2 = n2.nextSibling().toElement();
	}
	if ( !parent->inherits( "QAction" ) )
	    actionList.append( a );
    } else if ( n.tagName() == "actiongroup" ) {
	a = new QActionGroup( parent );
	QDomElement n2 = n.firstChild().toElement();
	while ( !n2.isNull() ) {
	    if ( n2.tagName() == "property" ) {
		setProperty( a, n2.attribute( "name" ), n2.firstChild().toElement() );
	    } else if ( n2.tagName() == "action" ||
			n2.tagName() == "actiongroup" ) {
		loadChildAction( a, n2 );
	    } else if ( n2.tagName() == "event" ) {
		ef.events.append( n2.attribute( "name" ) );
		ef.functions.append( QStringList::split( ',', n2.attribute( "functions" ) ) );
	    }
	    n2 = n2.nextSibling().toElement();
	}
	if ( !parent->inherits( "QAction" ) )
	    actionList.append( a );
    }
    if ( a )
	eventMap.insert( a, ef );
}

void KommanderFactory::loadActions( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "action" ) {
	    loadChildAction( toplevel, n );
	} else if ( n.tagName() == "actiongroup" ) {
	    loadChildAction( toplevel, n );
	}
	n = n.nextSibling().toElement();
    }
}

void KommanderFactory::loadToolBars( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    QMainWindow *mw = ( (QMainWindow*)toplevel );
    QToolBar *tb = 0;
    while ( !n.isNull() ) {
	if ( n.tagName() == "toolbar" ) {
	    Qt::Dock dock = (Qt::Dock)n.attribute( "dock" ).toInt();
	    tb = new QToolBar( QString::null, mw, dock );
	    tb->setLabel( n.attribute( "label" ) );
	    tb->setName( n.attribute( "name" ) );
	    QDomElement n2 = n.firstChild().toElement();
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "action" ) {
		    QAction *a = findAction( n2.attribute( "name" ) );
		    if ( a )
			a->addTo( tb );
		} else if ( n2.tagName() == "separator" ) {
		    tb->addSeparator();
		} else if ( n2.tagName() == "widget" ) {
		    (void)createWidgetInternal( n2, tb, 0, n2.attribute( "class", "QWidget" ) );
		} else if ( n2.tagName() == "property" ) {
		    setProperty( tb, n2.attribute( "name" ), n2.firstChild().toElement() );
		}
		n2 = n2.nextSibling().toElement();
	    }
	}
	n = n.nextSibling().toElement();
    }
}

void KommanderFactory::loadMenuBar( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    QMainWindow *mw = ( (QMainWindow*)toplevel );
    QMenuBar *mb = mw->menuBar();
    while ( !n.isNull() ) {
	if ( n.tagName() == "item" ) {
	    QPopupMenu *popup = new QPopupMenu( mw );
	    popup->setName( n.attribute( "name" ) );
	    QDomElement n2 = n.firstChild().toElement();
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "action" ) {
		    QAction *a = findAction( n2.attribute( "name" ) );
		    if ( a )
			a->addTo( popup );
		} else if ( n2.tagName() == "separator" ) {
		    popup->insertSeparator();
		}
		n2 = n2.nextSibling().toElement();
	    }
	    mb->insertItem( translate( n.attribute( "text" ) ), popup );
	} else if ( n.tagName() == "property" ) {
	    setProperty( mb, n.attribute( "name" ), n.firstChild().toElement() );
	}
	n = n.nextSibling().toElement();
    }
}


QAction *KommanderFactory::findAction( const QString &name )
{
    for ( QAction *a = actionList.first(); a; a = actionList.next() ) {
	if ( QString( a->name() ) == name )
	    return a;
	QAction *ac = (QAction*)a->child( name.latin1(), "QAction" );
	if ( ac )
	    return ac;
    }
    return 0;
}

void KommanderFactory::loadImages( const QString &dir )
{
  QDir d(dir);
  QStringList l = d.entryList(QDir::Files);
  for (QStringList::Iterator it = l.begin(); it != l.end(); ++it)
    QMimeSourceFactory::defaultFactory()->setPixmap(*it, QPixmap(d.path() + "/" + *it, "PNG"));
}

QString KommanderFactory::translate( const QString& sourceText, const QString& comment )
{
  if (!sourceText.isEmpty() && !comment.isEmpty())
    return KGlobal::locale()->translate(comment.utf8(), sourceText.utf8());
  else if (!sourceText.isEmpty())
    return KGlobal::locale()->translate(sourceText.utf8());
  else
    return sourceText;
}
