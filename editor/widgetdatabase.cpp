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
//#define NO_STATIC_COLORS
#include "globaldefs.h"
#include <qstrlist.h>
#include <qdict.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qcleanuphandler.h>

#include <qfeatures.h>

#include <stdlib.h>

#include <kommanderfactory.h>
#include <klocale.h>

const int dbsize = 300;
const int dbcustom = 200;
const int dbdictsize = 211;
static WidgetDatabaseRecord* widget_db[ dbsize ];
static QDict<int> *className2Id = 0;
static int dbcount  = 0;
static int dbcustomcount = 200;
static QStrList *wGroups;
static QStrList *invisibleGroups;
static bool whatsThisLoaded = false;
#ifndef KOMMANDER
static QPluginManager<WidgetInterface> *widgetPluginManager = 0;
#endif
static bool plugins_set_up = false;
static bool was_in_setup = false;

#ifndef KOMMANDER
QCleanupHandler<QPluginManager<WidgetInterface> > cleanup_manager;
#endif

WidgetDatabaseRecord::WidgetDatabaseRecord()
{
    isForm = false;
    isContainer = false;
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
  was_in_setup = true;
#ifndef UIC
  Q_UNUSED(id) if (dbcount)
    return;
#else
  if (dbcount && id != -2)
    return;
  if (dbcount && !plugins_set_up)
  {
    setupPlugins();
    return;
  }
  if (dbcount && plugins_set_up)
    return;
#endif

  wGroups = new QStrList;
  invisibleGroups = new QStrList;
  invisibleGroups->append("Forms");
  invisibleGroups->append("Temp");
  className2Id = new QDict < int >(dbdictsize);
  className2Id->setAutoDelete(true);

  WidgetDatabaseRecord *r = 0;

  /* Designer widgets */
  r = new WidgetDatabaseRecord;
  r->iconSet = "pushbutton.xpm";
  r->name = "QPushButton";
  r->group = widgetGroup("Buttons");
  r->toolTip = i18n("Push Button");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "toolbutton.xpm";
  r->name = "QToolButton";
  r->group = widgetGroup("Buttons");
  r->toolTip = i18n("Tool Button");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "radiobutton.xpm";
  r->name = "QRadioButton";
  r->group = widgetGroup("Buttons");
  r->toolTip = i18n("Radio Button");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "checkbox.xpm";
  r->name = "QCheckBox";
  r->group = widgetGroup("Buttons");
  r->toolTip = i18n("Check Box");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "groupbox.xpm";
  r->name = "QGroupBox";
  r->group = widgetGroup("Containers");
  r->toolTip = i18n("Group Box");
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "buttongroup.xpm";
  r->name = "QButtonGroup";
  r->group = widgetGroup("Containers");
  r->toolTip = i18n("Button Group");
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "frame.xpm";
  r->name = "QFrame";
  r->group = widgetGroup("Containers");
  r->toolTip = i18n("Frame");
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "tabwidget.xpm";
  r->name = "QTabWidget";
  r->group = widgetGroup("Containers");
  r->toolTip = i18n("Tabwidget");
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "listbox.xpm";
  r->name = "QListBox";
  r->group = widgetGroup("Views");
  r->toolTip = i18n("List Box");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "listview.xpm";
  r->name = "QListView";
  r->group = widgetGroup("Views");
  r->toolTip = i18n("List View");
  append(r);

#if !defined(QT_NO_ICONVIEW) || defined(UIC)
  r = new WidgetDatabaseRecord;
  r->iconSet = "iconview.xpm";
  r->name = "QIconView";
  r->group = widgetGroup("Views");
  r->toolTip = i18n("Icon View");
  append(r);
#endif

#if !defined(QT_NO_TABLE)
  r = new WidgetDatabaseRecord;
  r->iconSet = "table.xpm";
  r->name = "QTable";
  r->group = widgetGroup("Views");
  r->toolTip = i18n("Table");
  append(r);
#endif

#if !defined(QT_NO_SQL)
  r = new WidgetDatabaseRecord;
  r->iconSet = "datatable.xpm";
  r->includeFile = "qdatatable.h";
  r->name = "QDataTable";
  r->group = widgetGroup("Database");
  r->toolTip = i18n("Data Table");
  append(r);
#endif

  r = new WidgetDatabaseRecord;
  r->iconSet = "lineedit.xpm";
  r->name = "QLineEdit";
  r->group = widgetGroup("Input");
  r->toolTip = i18n("Line Edit");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "spinbox.xpm";
  r->name = "QSpinBox";
  r->group = widgetGroup("Input");
  r->toolTip = i18n("Spin Box");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "dateedit.xpm";
  r->name = "QDateEdit";
  r->group = widgetGroup("Input");
  r->toolTip = i18n("Date Edit");
  r->includeFile = "qdatetimeedit.h";
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "timeedit.xpm";
  r->name = "QTimeEdit";
  r->group = widgetGroup("Input");
  r->toolTip = i18n("Time Edit");
  r->includeFile = "qdatetimeedit.h";
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "datetimeedit.xpm";
  r->name = "QDateTimeEdit";
  r->group = widgetGroup("Input");
  r->toolTip = i18n("Date-Time Edit");
  r->includeFile = "qdatetimeedit.h";
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "multilineedit.xpm";
  r->name = "QMultiLineEdit";
  r->group = widgetGroup("Temp");
  r->toolTip = i18n("Multi Line Edit");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "richtextedit.xpm";
  r->name = "QTextEdit";
  r->group = widgetGroup("Input");
  r->toolTip = i18n("Rich Text Edit");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "combobox.xpm";
  r->name = "QComboBox";
  r->group = widgetGroup("Input");
  r->toolTip = i18n("Combo Box");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "slider.xpm";
  r->name = "QSlider";
  r->group = widgetGroup("Input");
  r->toolTip = i18n("Slider");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "scrollbar.xpm";
  r->name = "QScrollBar";
  r->group = widgetGroup("Input");
  r->toolTip = i18n("Scrollbar");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "dial.xpm";
  r->name = "QDial";
  r->group = widgetGroup("Input");
  r->toolTip = i18n("Dial");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "label.xpm";
  r->name = "QLabel";
  r->group = widgetGroup("Temp");
  r->toolTip = i18n("Label");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "lcdnumber.xpm";
  r->name = "QLCDNumber";
  r->group = widgetGroup("Display");
  r->toolTip = i18n("LCD Number");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "progress.xpm";
  r->name = "QProgressBar";
  r->group = widgetGroup("Display");
  r->toolTip = i18n("Progress Bar");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "textview.xpm";
  r->name = "QTextView";
  r->group = widgetGroup("Temp");
  r->toolTip = i18n("Text View");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "textbrowser.xpm";
  r->name = "QTextBrowser";
  r->group = widgetGroup("Display");
  r->toolTip = i18n("Text Browser");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "spacer.xpm";
  r->name = "Spacer";
  r->group = widgetGroup("Temp");
  r->toolTip = i18n("Spacer");
  r->whatsThis =
      i18n("The Spacer provides horizontal and vertical spacing to be able to manipulate the behaviour of layouts.");
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "QWidget";
  r->isForm = true;
  r->group = widgetGroup("Forms");
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "QDialog";
  r->group = widgetGroup("Forms");
  r->isForm = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "QWizard";
  r->group = widgetGroup("Forms");
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "QDesignerWizard";
  r->group = widgetGroup("Forms");
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "QLayoutWidget";
  r->group = widgetGroup("Temp");
  r->includeFile = "";
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "QSplitter";
  r->group = widgetGroup("Temp");
  r->includeFile = "qsplitter.h";
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "tabwidget.xpm";
  r->name = "QDesignerTabWidget";
  r->group = widgetGroup("Temp");
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "tabwidget.xpm";
  r->name = "QDesignerWidget";
  r->group = widgetGroup("Temp");
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "tabwidget.xpm";
  r->name = "QDesignerDialog";
  r->group = widgetGroup("Temp");
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "";
  r->name = "QMainWindow";
  r->includeFile = "qmainwindow.h";
  r->group = widgetGroup("Temp");
  r->isContainer = true;
  append(r);

#ifndef QT_NO_SQL
  r = new WidgetDatabaseRecord;
  r->name = "QDataBrowser";
  r->includeFile = "qdatabrowser.h";
  r->group = widgetGroup("Database");
  r->toolTip = "Data Browser";
  r->iconSet = "databrowser.xpm";
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "QDataView";
  r->includeFile = "qdataview.h";
  r->group = widgetGroup("Database");
  r->toolTip = "Data View";
  r->iconSet = "dataview.xpm";
  r->isContainer = true;
  append(r);
#endif

  r = new WidgetDatabaseRecord;
  r->name = "Dialog";
  r->group = widgetGroup("Forms");
  r->isContainer = true;
  r->isForm = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "Wizard";
  r->group = widgetGroup("Forms");
  r->isContainer = true;
  r->isForm = true;
  append(r);
  
  r = new WidgetDatabaseRecord;
  r->name = "EditorDialog";
  r->group = widgetGroup("Temp");
  r->isContainer = true;
  r->isForm = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "EditorWizard";
  r->group = widgetGroup("Temp");
  r->isContainer = true;
  r->isForm = true;
  append(r);
  
  r = new WidgetDatabaseRecord;
  r->name = "EditorTabWidget";
  r->group = widgetGroup("Temp");
  r->isContainer = true;
  append(r);



  /* Kommandr widgets */
  r = new WidgetDatabaseRecord;
  r->iconSet = "label.xpm";
  r->name = "Label";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("Text Label");
  r->whatsThis = i18n("The Text Label provides a widget to display static text.");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "pixlabel.xpm";
  r->name = "PixmapLabel";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("Pixmap Label");
  r->whatsThis = i18n("The Pixmap Label provides a widget to display pixmaps.");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "lineedit.xpm";
  r->name = "LineEdit";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A line edit");
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "TextEdit";
  r->iconSet = "textedit.png";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A rich text edit");
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "TextBrowser";
  r->iconSet = "textbrowser.png";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("Text Browser");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "listbox.xpm";
  r->name = "ListBox";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("List Box");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "combobox.xpm";
  r->name = "ComboBox";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A combo box");
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "TreeWidget";
  r->iconSet = "listview.png";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A tree widget");
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "Table";
  r->iconSet = "table.xmp";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A table widget");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "pushbutton.xpm";
  r->name = "ExecButton";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A button that when clicked, execute a command");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "closebutton.png";
  r->name = "CloseButton";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A button that closes the dialog it is in");
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "Konsole";
  r->iconSet = "konsole.png";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A listbox showing output of a script");
  append(r);
  
  r = new WidgetDatabaseRecord;
  r->name = "FileSelector";
  r->iconSet = "lineedit.xpm";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A widget made up of a line edit and push button, for selecting files and folders");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "checkbox.xpm";
  r->name = "CheckBox";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A check box");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "radiobutton.xpm";
  r->name = "RadioButton";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A radio button");
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "buttongroup.xpm";
  r->name = "ButtonGroup";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A widget for grouping buttons together");
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "groupbox.xpm";
  r->name = "GroupBox";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A widget for grouping other widgets together");
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "tabwidget.xpm";
  r->name = "TabWidget";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A widget with tabs");
  r->isContainer = true;
  append(r);

  r = new WidgetDatabaseRecord;
  r->iconSet = "spinbox.xpm";
  r->name = "SpinBoxInt";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A spin box");
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "Slider";
  r->iconSet = "slider.png";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("Slider");
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "RichTextEditor";
  r->iconSet = "richtextedit.xpm";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A small rich text editor");
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "StatusBar";
  r->iconSet = "statusbar.png";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A status bar");
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "ProgressBar";
  r->iconSet = "progress.png";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A progress bar");
  append(r);
  
  r = new WidgetDatabaseRecord;
  r->name = "ScriptObject";
  r->iconSet = "shellscript.png";
  r->group = widgetGroup( "Kommander" );
  r->toolTip = i18n("A hidden script container");
  append(r);

  r = new WidgetDatabaseRecord;
  r->name = "Timer";
  r->iconSet = "timer.png";
  r->group = widgetGroup( "Kommander" );
  r->toolTip = i18n("A timer for running scripts periodically");
  append(r);
 
 r = new WidgetDatabaseRecord;
  r->iconSet = "datepicker.xpm";
  r->name = "DatePicker";
  r->group = widgetGroup("Kommander");
  r->toolTip = i18n("A date selection widget");
  append(r);

#ifndef UIC
  setupPlugins();
#endif
}

void WidgetDatabase::setupPlugins()
{
  if (plugins_set_up)
    return;
  //qDebug("WidgetDatabase::setupPlugins");
  plugins_set_up = true;
  FeatureList widgets = KommanderFactory::featureList();
  //qDebug("num features = %d", widgets.count());
  for (FeatureList::Iterator it = widgets.begin(); it != widgets.end(); ++it)
  {
    if (hasWidget(it.key()))
      continue;
    WidgetDatabaseRecord *r = new WidgetDatabaseRecord;

    QString grp = (*it).group;
    if (grp.isEmpty())
      grp = "Kommander";
    r->group = widgetGroup(grp);
    r->toolTip = (*it).toolTip;
    r->whatsThis = (*it).whatsThis;
    r->isContainer = (*it).isContainer;
    r->name = it.key();
    append(r);
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
	return false;
    return r->isForm;
}

/*!  Returns whether the widget registered as \a id can have children.
*/

bool WidgetDatabase::isContainer( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return false;
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
	return widget_db[ index ];
    if ( index < dbcount )
	return widget_db[ index ];
    return 0;
}

void WidgetDatabase::insert( int index, WidgetDatabaseRecord *r )
{
    if ( index < 0 || index >= dbsize )
	return;
    widget_db[ index ] = r;
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
	if ( !( r = widget_db[ i ] ) )
	    continue;
	if ( r->group == grp )
	{
	// FIXME
	    if(r->group == "Kommander")
		    return false;

	    if(r->name[0] != 'Q')
  	        return false;
	}
    }
    return true;
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
	return true;
    return false;
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
    whatsThisLoaded = true;
}


// ### Qt 3.1: make these publically accessible via QWidgetDatabase API
#ifndef KOMMANDER
#if defined(UIC)
bool dbnounload = false;
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
	    widgetPluginManager->setAutoUnload( false );
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
