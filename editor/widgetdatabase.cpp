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

#if !defined(UIC)
#include "pixmapchooser.h"
#endif
#ifndef KOMMANDER
#include "widgetinterface.h"
#endif
#include "widgetdatabase.h"

#include <qapplication.h>
#define NO_STATIC_COLORS
#include "globaldefs.h"
#include <qstrlist.h>
#include <qdict.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qcleanuphandler.h>

#include <qfeatures.h>

#include <stdlib.h>

#include <kommanderfactory.h>

const int dbsize = 300;
const int dbcustom = 200;
const int dbdictsize = 211;
static WidgetDatabaseRecord* db[ dbsize ];
static QDict<int> *className2Id = 0;
static int dbcount  = 0;
static int dbcustomcount = 200;
static QStrList *wGroups;
static QStrList *invisibleGroups;
static bool whatsThisLoaded = FALSE;
#ifndef KOMMANDER
static QPluginManager<WidgetInterface> *widgetPluginManager = 0;
#endif
static bool plugins_set_up = FALSE;
static bool was_in_setup = FALSE;

#ifndef KOMMANDER
QCleanupHandler<QPluginManager<WidgetInterface> > cleanup_manager;
#endif

WidgetDatabaseRecord::WidgetDatabaseRecord()
{
    isForm = FALSE;
    isContainer = FALSE;
    icon = 0;
    nameCounter = 0;
}

WidgetDatabaseRecord::~WidgetDatabaseRecord()
{
    delete icon;
}


/*!
  \class WidgetDatabase widgetdatabase.h
  \brief The WidgetDatabase class holds information about widgets

  The WidgetDatabase holds information about widgets like toolTip(),
  iconSet(), ... It works Id-based, so all access functions take the
  widget id as parameter. To get the id for a widget (classname), use
  idFromClassName().

  All access functions are static.  Having multiple widgetdatabases in
  one application doesn't make sense anyway and so you don't need more
  than an instance of the widgetdatabase.

  For creating widgets, layouts, etc. see WidgetFactory.
*/

/*!
  Creates widget database. Does nothing.
*/

WidgetDatabase::WidgetDatabase()
{
}

/*!  Sets up the widget database. If the static widgetdatabase already
  exists, the functions returns immediately.
*/

void WidgetDatabase::setupDataBase( int id )
{
    was_in_setup = TRUE;
#ifndef UIC
    Q_UNUSED( id )
    if ( dbcount )
	return;
#else
    if ( dbcount && id != -2 )
	return;
    if ( dbcount && !plugins_set_up ) {
	setupPlugins();
	return;
    }
    if ( dbcount && plugins_set_up)
	return;
#endif

    wGroups = new QStrList;
    invisibleGroups = new QStrList;
    invisibleGroups->append( "Forms" );
    invisibleGroups->append( "Temp" );
    className2Id = new QDict<int>( dbdictsize );
    className2Id->setAutoDelete( TRUE );

    WidgetDatabaseRecord *r = 0;

    r = new WidgetDatabaseRecord;
    r->iconSet = "pushbutton.xpm";
    r->name = "QPushButton";
    r->group = widgetGroup( "Buttons" );
    r->toolTip = "Push Button";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "toolbutton.xpm";
    r->name = "QToolButton";
    r->group = widgetGroup( "Buttons" );
    r->toolTip = "Tool Button";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "radiobutton.xpm";
    r->name = "QRadioButton";
    r->group = widgetGroup( "Buttons" );
    r->toolTip = "Radio Button";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "checkbox.xpm";
    r->name = "QCheckBox";
    r->group = widgetGroup( "Buttons" );
    r->toolTip = "Check Box";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "groupbox.xpm";
    r->name = "QGroupBox";
    r->group = widgetGroup( "Containers" );
    r->toolTip = "Group Box";
    r->isContainer = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "buttongroup.xpm";
    r->name = "QButtonGroup";
    r->group = widgetGroup( "Containers" );
    r->toolTip = "Button Group";
    r->isContainer = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "frame.xpm";
    r->name = "QFrame";
    r->group = widgetGroup( "Containers" );
    r->toolTip = "Frame";
    r->isContainer = TRUE;

    append( r );

//#ifndef KOMMANDER
    r = new WidgetDatabaseRecord;
    r->iconSet = "tabwidget.xpm";
    r->name = "QTabWidget";
    r->group = widgetGroup( "Containers" );
    r->toolTip = "Tabwidget";
    r->isContainer = TRUE;

    append( r );
//#endif


    r = new WidgetDatabaseRecord;
    r->iconSet = "listbox.xpm";
    r->name = "QListBox";
    r->group = widgetGroup( "Views" );
    r->toolTip = "List Box";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "listview.xpm";
    r->name = "QListView";
    r->group = widgetGroup( "Views" );
    r->toolTip = "List View";

    append( r );

#if !defined(QT_NO_ICONVIEW) || defined(UIC)
    r = new WidgetDatabaseRecord;
    r->iconSet = "iconview.xpm";
    r->name = "QIconView";
    r->group = widgetGroup( "Views" );
    r->toolTip = "Icon View";

    append( r );
#endif

#if !defined(QT_NO_TABLE)
    r = new WidgetDatabaseRecord;
    r->iconSet = "table.xpm";
    r->name = "QTable";
    r->group = widgetGroup( "Views" );
    r->toolTip = "Table";

    append( r );
#endif

#if !defined(QT_NO_SQL)
    r = new WidgetDatabaseRecord;
    r->iconSet = "datatable.xpm";
    r->includeFile = "qdatatable.h";
    r->name = "QDataTable";
    r->group = widgetGroup( "Database" );
    r->toolTip = "Data Table";

    append( r );
#endif

    r = new WidgetDatabaseRecord;
    r->iconSet = "lineedit.xpm";
    r->name = "QLineEdit";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Line Edit";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "spinbox.xpm";
    r->name = "QSpinBox";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Spin Box";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "dateedit.xpm";
    r->name = "QDateEdit";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Date Edit";
    r->includeFile = "qdatetimeedit.h";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "timeedit.xpm";
    r->name = "QTimeEdit";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Time Edit";
    r->includeFile = "qdatetimeedit.h";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "datetimeedit.xpm";
    r->name = "QDateTimeEdit";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Date-Time Edit";
    r->includeFile = "qdatetimeedit.h";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "multilineedit.xpm";
    r->name = "QMultiLineEdit";
    r->group = widgetGroup( "Temp" );
    r->toolTip = "Multi Line Edit";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "richtextedit.xpm";
    r->name = "QTextEdit";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Rich Text Edit";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "combobox.xpm";
    r->name = "QComboBox";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Combo Box";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "slider.xpm";
    r->name = "QSlider";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Slider";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "scrollbar.xpm";
    r->name = "QScrollBar";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Scrollbar";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "dial.xpm";
    r->name = "QDial";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Dial";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "label.xpm";
    r->name = "QLabel";
    r->group = widgetGroup( "Temp" );
    r->toolTip = "Label";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "label.xpm";
    r->name = "TextLabel";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "Text Label";
    r->whatsThis = "The Text Label provides a widget to display static text.";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "pixlabel.xpm";
    r->name = "PixmapLabel";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "Pixmap Label";
    r->whatsThis = "The Pixmap Label provides a widget to display pixmaps.";

    append( r );


#ifndef KOMMANDER
    r = new WidgetDatabaseRecord;
    r->iconSet = "label.xpm";
    r->name = "TextLabel";
    r->group = widgetGroup( "Display" );
    r->toolTip = "Text Label";
    r->whatsThis = "The Text Label provides a widget to display static text.";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "pixlabel.xpm";
    r->name = "PixmapLabel";
    r->group = widgetGroup( "Display" );
    r->toolTip = "Pixmap Label";
    r->whatsThis = "The Pixmap Label provides a widget to display pixmaps.";

    append( r );
#endif

    r = new WidgetDatabaseRecord;
    r->iconSet = "lcdnumber.xpm";
    r->name = "QLCDNumber";
    r->group = widgetGroup( "Display" );
    r->toolTip = "LCD Number";

    append( r );

#ifndef KOMMANDER
    r = new WidgetDatabaseRecord;
    r->iconSet = "line.xpm";
    r->name = "Line";
    r->group = widgetGroup( "Display" );
    r->toolTip = "Line";
    r->includeFile = "qframe.h";
    r->whatsThis = "The Line widget provides horizontal and vertical lines.";

    append( r );
#endif

    r = new WidgetDatabaseRecord;
    r->iconSet = "progress.xpm";
    r->name = "QProgressBar";
    r->group = widgetGroup( "Display" );
    r->toolTip = "Progress Bar";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "textview.xpm";
    r->name = "QTextView";
    r->group = widgetGroup( "Temp" );
    r->toolTip = "Text View";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "textbrowser.xpm";
    r->name = "QTextBrowser";
    r->group = widgetGroup( "Display" );
    r->toolTip = "Text Browser";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "spacer.xpm";
    r->name = "Spacer";
    r->group = widgetGroup( "Temp" );
    r->toolTip = "Spacer";
    r->whatsThis = "The Spacer provides horizontal and vertical spacing to be able to manipulate the behaviour of layouts.";

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "QWidget";
    r->isForm = TRUE;
    r->group = widgetGroup( "Forms" );

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "QDialog";
    r->group = widgetGroup( "Forms" );
    r->isForm = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "QWizard";
    r->group = widgetGroup( "Forms" );
    r->isContainer = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "QDesignerWizard";
    r->group = widgetGroup( "Forms" );
    r->isContainer = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "QLayoutWidget";
    r->group = widgetGroup( "Temp" );
    r->includeFile = "";
    r->isContainer = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "QSplitter";
    r->group = widgetGroup( "Temp" );
    r->includeFile = "qsplitter.h";
    r->isContainer = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "tabwidget.xpm";
    r->name = "QDesignerTabWidget";
    r->group = widgetGroup( "Temp" );
    r->isContainer = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "tabwidget.xpm";
    r->name = "QDesignerWidget";
    r->group = widgetGroup( "Temp" );
    r->isContainer = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "tabwidget.xpm";
    r->name = "QDesignerDialog";
    r->group = widgetGroup( "Temp" );
    r->isContainer = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "QMainWindow";
    r->includeFile = "qmainwindow.h";
    r->group = widgetGroup( "Temp" );
    r->isContainer = TRUE;

    append( r );

#ifndef QT_NO_SQL
    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "QDataBrowser";
    r->includeFile = "qdatabrowser.h";
    r->group = widgetGroup( "Database" );
    r->toolTip = "Data Browser";
    r->iconSet = "databrowser.xpm";
    r->isContainer = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "QDataView";
    r->includeFile = "qdataview.h";
    r->group = widgetGroup( "Database" );
    r->toolTip = "Data View";
    r->iconSet = "dataview.xpm";
    r->isContainer = TRUE;

    append( r );
#endif

#ifdef KOMMANDER

    // FIXME : load icons from Kommander DIR
    r = new WidgetDatabaseRecord;
    r->iconSet = "listview.xpm";
    r->name = "ListView";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "List View";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "listbox.xpm";
    r->name = "ListBox";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "List Box";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "dialog.xpm";
    r->name = "SubDialog";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A push button that when clicked, creates a new dialog";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "tabwidget.xpm";
    r->name = "TabWidget";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A widget with tabs";
    r->isContainer = TRUE;

    append( r );
	
    r = new WidgetDatabaseRecord;
    r->name = "EditorTabWidget";
    r->group = widgetGroup( "Temp" );
    r->isContainer = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
	r->iconSet = "lineedit.xpm";
    r->name = "LineEdit";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A line edit";

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "Dialog";
    r->group = widgetGroup( "Forms" );
    r->isContainer = TRUE;
    r->isForm = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "EditorDialog";
    r->group = widgetGroup( "Temp" );
    r->isContainer = TRUE;
    r->isForm = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
	r->iconSet = "pushbutton.xpm";
    r->name = "ExecButton";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A button that when clicked, execute a command";

    append( r );

    r = new WidgetDatabaseRecord;
	r->iconSet = "pushbutton.xpm";
//    r->iconSet = "closebutton.png";
    r->name = "CloseButton";
 //   r->includeFile = "closebutton.h";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A button that closes the dialog it's in";
  //  r->isContainer = FALSE;

    append( r );

    r = new WidgetDatabaseRecord;
   // r->iconSet = "fileselector.png";
    r->name = "FileSelector";
	r->iconSet = "lineedit.xpm";
    //r->includeFile = "fileselector.h";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A widget made up of a line edit and push button, for selecting files and folders";
//    r->isContainer = FALSE;

    append( r );

    r = new WidgetDatabaseRecord;
 //   r->iconSet = "textedit.png";
    r->name = "TextEdit";
	r->iconSet = "textedit.xpm";
 //   r->includeFile = "textedit.h";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A rich text edit";
 //   r->isContainer = FALSE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "radiobutton.xpm";
    r->name = "RadioButton";
 //   r->includeFile = "radiobutton.h";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A radio button";
 //   r->isContainer = FALSE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "buttongroup.xpm";
    r->name = "ButtonGroup";
//    r->includeFile = "buttongroup.h";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A widget for grouping buttons together";
    r->isContainer = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "groupbox.xpm";
    r->name = "GroupBox";
  //  r->includeFile = "groupbox.h";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A widget for grouping other widgets together";
    r->isContainer = TRUE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "checkbox.xpm";
    r->name = "CheckBox";
 //   r->includeFile = "checkbox.h";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A check box";
//    r->isContainer = FALSE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "combobox.xpm";
    r->name = "ComboBox";
//    r->includeFile = "combobox.h";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A combo box";
//    r->isContainer = FALSE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "spinbox.xpm";
    r->name = "SpinBoxInt";
//    r->includeFile = "spinboxint.h";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A spin box";
//    r->isContainer = FALSE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "ScriptObject";
//    r->group = widgetGroup( "Kommander" );
 //   r->toolTip = "";
//    r->isContainer = FALSE;

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "RichTextEditor";
	r->iconSet = "textedit.xpm";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A small rich text editor";

 	append( r );

    r = new WidgetDatabaseRecord;
    r->name = "TreeWidget";
	r->iconSet = "listview.xpm";
    r->group = widgetGroup( "Kommander" );
    r->toolTip = "A tree widget";

    append( r );


#endif

#ifndef UIC
    setupPlugins();
#endif
}

void WidgetDatabase::setupPlugins()
{
    if ( plugins_set_up )
	return;
    plugins_set_up = TRUE;
    FeatureList widgets = KommanderFactory::featureList();
    for ( FeatureList::Iterator it = widgets.begin(); it != widgets.end(); ++it ) 
    {
	if ( hasWidget( it.key() ) )
	    continue;
	WidgetDatabaseRecord *r = new WidgetDatabaseRecord;

	QString grp = (*it).group;
	if ( grp.isEmpty() )
	    grp = "Kommander";
	r->group = widgetGroup( grp );
	r->toolTip = (*it).toolTip;
	r->whatsThis = (*it).whatsThis;
	r->isContainer = (*it).isContainer;
	r->name = it.key();
	append( r );
    }
}

/*!
  Returns the number of elements in the widget database.
*/

int WidgetDatabase::count()
{
    setupDataBase( -1 );
    return dbcount;
}

/*!
  Returns the id at which the ids of custom widgets start.
*/

int WidgetDatabase::startCustom()
{
    setupDataBase( -1 );
    return dbcustom;
}

/*!
  Returns the iconset which represents the class registered as \a id.
*/

QIconSet WidgetDatabase::iconSet( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return QIconSet();
#if !defined(UIC) && !defined(RESOURCE)
    if ( !r->icon )
	r->icon = new QIconSet( PixmapChooser::loadPixmap( r->iconSet, PixmapChooser::Small ),
				PixmapChooser::loadPixmap( r->iconSet, PixmapChooser::Large ) );
    return *r->icon;
#else
    return QIconSet();
#endif
}

/*!
  Returns the classname of the widget which is registered as \a id.
*/

QString WidgetDatabase::className( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return QString::null;
    return r->name;
}

/*!
  Returns the group the widget registered as \a id belongs to.
*/

QString WidgetDatabase::group( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return QString::null;
    return r->group;
}

/*!
  Returns the tooltip text of the widget which is registered as \a id.
*/

QString WidgetDatabase::toolTip( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return QString::null;
    return r->toolTip;
}

/*!
  Returns the what's this? text of the widget which is registered as \a id.
*/

QString WidgetDatabase::whatsThis( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return QString::null;
    return r->whatsThis;
}

/*!
  Returns the include file if the widget which is registered as \a id.
*/

QString WidgetDatabase::includeFile( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return QString::null;
    if ( r->includeFile.isNull() )
	return r->name.lower() + ".h";
    return r->includeFile;
}

/*!  Returns whether the widget registered as \a id is a form.
*/
bool WidgetDatabase::isForm( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return FALSE;
    return r->isForm;
}

/*!  Returns whether the widget registered as \a id can have children.
*/

bool WidgetDatabase::isContainer( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return FALSE;
    return r->isContainer || r->isForm;
}

QString WidgetDatabase::createWidgetName( int id )
{
    setupDataBase( id );
    QString n = className( id );
    if ( n == "QLayoutWidget" )
	n = "Layout";
    if ( n[ 0 ] == 'Q' )
	n = n.mid( 1 );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return n;
    n += QString::number( ++r->nameCounter );
    return n;
}

/*!  Returns the id for \a name or -1 if \a name is unknown.
 */
int WidgetDatabase::idFromClassName( const QString &name )
{
    setupDataBase( -1 );
    if ( name.isEmpty() )
	return 0;
    int *i = className2Id->find( name );
    if ( i )
	return *i;
    if ( name == "FormWindow" )
	return idFromClassName( "QLayoutWidget" );
#ifdef UIC
    setupDataBase( -2 );
    i = className2Id->find( name );
    if ( i )
	return *i;
#endif
    return -1;
}

bool WidgetDatabase::hasWidget( const QString &name )
{
    return className2Id->find( name ) != 0;
}

WidgetDatabaseRecord *WidgetDatabase::at( int index )
{
    if ( index < 0 )
	return 0;
    if ( index >= dbcustom && index < dbcustomcount )
	return db[ index ];
    if ( index < dbcount )
	return db[ index ];
    return 0;
}

void WidgetDatabase::insert( int index, WidgetDatabaseRecord *r )
{
    if ( index < 0 || index >= dbsize )
	return;
    db[ index ] = r;
    className2Id->insert( r->name, new int( index ) );
    if ( index < dbcustom )
	dbcount = QMAX( dbcount, index );
}

void WidgetDatabase::append( WidgetDatabaseRecord *r )
{
    if ( !was_in_setup )
	setupDataBase( -1 );
    insert( dbcount++, r );
}

QString WidgetDatabase::widgetGroup( const QString &g )
{
    if ( wGroups->find( g ) == -1 )
	wGroups->append( g );
    return g;
}

bool WidgetDatabase::isGroupEmpty( const QString &grp )
{
    WidgetDatabaseRecord *r = 0;
    for ( int i = 0; i < dbcount; ++i ) {
	if ( !( r = db[ i ] ) )
	    continue;
	if ( r->group == grp )
	{
	// FIXME
	    if(r->group == "Kommander")
		    return FALSE;

	    if(r->name[0] != 'Q') 
  	        return FALSE;
	}
    }
    return TRUE;
}

QString WidgetDatabase::widgetGroup( int i )
{
    setupDataBase( -1 );
    if ( i >= 0 && i < (int)wGroups->count() )
	return wGroups->at( i );
    return QString::null;
}

int WidgetDatabase::numWidgetGroups()
{
    setupDataBase( -1 );
    return wGroups->count();
}

bool WidgetDatabase::isGroupVisible( const QString &g )
{
    setupDataBase( -1 );
    return invisibleGroups->find( g ) == -1;
}

int WidgetDatabase::addCustomWidget( WidgetDatabaseRecord *r )
{
    insert( dbcustomcount++, r );
    return dbcustomcount - 1;
}

bool WidgetDatabase::isCustomWidget( int id )
{
    if ( id >= dbcustom && id < dbcustomcount )
	return TRUE;
    return FALSE;
}

bool WidgetDatabase::isWhatsThisLoaded()
{
    return whatsThisLoaded;
}

void WidgetDatabase::loadWhatsThis( const QString &docPath )
{
    QString whatsthisFile = docPath + "/whatsthis";
    QFile f( whatsthisFile );
    if ( !f.open( IO_ReadOnly ) )
	return;
    QTextStream ts( &f );
    while ( !ts.atEnd() ) {
	QString s = ts.readLine();
	QStringList l = QStringList::split( " | ", s );
	int id = idFromClassName( l[ 1 ] );
	WidgetDatabaseRecord *r = at( id );
	if ( r )
	    r->whatsThis = l[ 0 ];
    }
    whatsThisLoaded = TRUE;
}


// ### Qt 3.1: make these publically accessible via QWidgetDatabase API
#ifndef KOMMANDER
#if defined(UIC)
bool dbnounload = FALSE;
QStringList *dbpaths = 0;
#endif

QPluginManager<WidgetInterface> *widgetManager()
{
    if ( !widgetPluginManager ) {
#ifndef KOMMANDER
	widgetPluginManager = new QPluginManager<WidgetInterface>( IID_Widget, QApplication::libraryPaths(), "/designer" );
#else
	widgetPluginManager = new QPluginManager<WidgetInterface>( IID_Widget, QApplication::libraryPaths(), "" );
#endif

	cleanup_manager.add( &widgetPluginManager );
#if defined(UIC)
	if ( dbnounload )
	    widgetPluginManager->setAutoUnload( FALSE );
	if ( dbpaths ) {
	    QStringList::ConstIterator it = dbpaths->begin();
	    for ( ; it != dbpaths->end(); ++it )
		widgetPluginManager->addLibraryPath( *it );
	}
#endif
    }
    return widgetPluginManager;
}
#endif
