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

#include "kommanderfactory.h"
#include <kommanderplugin.h>

#include <kconfig.h>
#include <kdebug.h>
#include <klibloader.h>
#include <kglobal.h>
#include <klocale.h>
#include <kassistantdialog.h>


#include <QListIterator>
#include <QHBoxLayout>
#include <QList>
#include <Q3ListBox>
#include <Q3ListView>
#include <QTableWidget>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QMainWindow>
#include <QStackedWidget>
#include <QGroupBox>
#include <QButtonGroup>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QToolBox>
#include <QWizard>
#include <QLabel>
#include <QWidget>
#include <QSplitter>
#include <QApplication>
#include <QToolTip>
#include <QGridLayout>
#include <QPixmap>
#include <QVBoxLayout>
#include <QAbstractButton>
#include <QDomDocument>
#include <QDir>
#include <QLayout>
#include <QMetaObject>
#include <QMetaProperty>
#include <QComboBox>

#include "domtool.h"

#include <stdlib.h>
#include <zlib.h>

static QList<KommanderPlugin *> widgetPlugins;

QMap<QWidget*, QString> *qwf_functions = 0;
QMap<QWidget*, QString> *qwf_forms = 0;
QString *qwf_language = 0;
bool qwf_execute_code = true;
bool qwf_stays_on_top = false;
QString qwf_currFileName = "";

KommanderFactory::KommanderFactory()
        : dbControls ( 0 ), usePixmapCollection ( false )
{
    //widgetPlugins.setAutoDelete( true );
    defSpacing = 6;
    defMargin = 11;
}

KommanderFactory::~KommanderFactory()
{
}

QWidget *KommanderFactory::create ( const QString &uiFile, QObject *connector, QWidget *parent, const char *name )
{
    QFile f ( uiFile );
    if ( !f.open ( QIODevice::ReadOnly ) )
        return 0;

    qwf_currFileName = uiFile;
    QWidget *w = KommanderFactory::create ( &f, connector, parent, name );
    if ( !qwf_forms )
        qwf_forms = new QMap<QWidget*, QString>;
    qwf_forms->insert ( w, uiFile );
    return w;
}

QWidget *KommanderFactory::create ( QIODevice *dev, QObject *connector, QWidget *parent, const char *name )
{
    QDomDocument doc;
    QString errMsg;
    int errLine;
    if ( !doc.setContent ( dev, &errMsg, &errLine ) )
    {
        //qDebug( QString("Parse error: ") + errMsg + QString(" in line %d"), errLine );
        return 0;
    }

    DomTool::fixDocument ( doc );

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

    QDomElement databases = e;
    while ( databases.tagName() != "databases" && !databases.isNull() )
        databases = databases.nextSibling().toElement();


    QDomElement widget;
    while ( !e.isNull() )
    {
        if ( e.tagName() == "widget" )
        {
            widget = e;
        }
        else if ( e.tagName() == "variable" )   // compatibility with old betas
        {
            widgetFactory->variables << e.firstChild().toText().data();
        }
        else if ( e.tagName() == "pixmapinproject" )
        {
            widgetFactory->usePixmapCollection = true;
        }
        else if ( e.tagName() == "layoutdefaults" )
        {
            widgetFactory->defSpacing = e.attribute ( "spacing", QString::number ( widgetFactory->defSpacing ) ).toInt();
            widgetFactory->defMargin = e.attribute ( "margin", QString::number ( widgetFactory->defMargin ) ).toInt();
        }
        e = e.nextSibling().toElement();
    }

    if ( !imageCollection.isNull() )
        widgetFactory->loadImageCollection ( imageCollection );

    widgetFactory->createWidgetInternal ( widget, parent, 0, widget.attribute ( "class", "QWidget" ) );
    QWidget *w = widgetFactory->toplevel;
    if ( !w )
    {
        delete widgetFactory;
        return 0;
    }

    if ( !variables.isNull() )
    {
        for ( QDomElement n = variables.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
            if ( n.tagName() == "variable" )
                widgetFactory->variables << n.firstChild().toText().data();
    }
    if ( !eltSlots.isNull() )
    {
        for ( QDomElement n = eltSlots.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
            if ( n.tagName() == "slot" )
            {
                QString s = n.firstChild().toText().data();
                widgetFactory->languageSlots.insert ( s.left ( s.indexOf ( "(" ) ) , n.attribute ( "language", "C++" ) );
            }
    }

    if ( !actions.isNull() )
        widgetFactory->loadActions ( actions );
    if ( !toolbars.isNull() )
        widgetFactory->loadToolBars ( toolbars );
    if ( !menubar.isNull() )
        widgetFactory->loadMenuBar ( menubar );

    if ( !connections.isNull() )
        widgetFactory->loadConnections ( connections, connector );
    if ( w && name && qstrlen ( name ) > 0 )
        w->setObjectName ( name );

    if ( !tabOrder.isNull() )
        widgetFactory->loadTabOrder ( tabOrder );

    for ( QMap<QString, QString>::Iterator it = widgetFactory->buddies.begin(); it != widgetFactory->buddies.end(); ++it )
    {
        QLabel *label = widgetFactory->toplevel->findChild<QLabel *> ( it.key() );
        QWidget *buddy = widgetFactory->toplevel->findChild<QWidget *> ( *it );
        if ( label && buddy )
            label->setBuddy ( buddy );
    }

    delete widgetFactory;

    QApplication::sendPostedEvents();

    return w;
}

void KommanderFactory::addPlugin ( KommanderPlugin *plugin )
{
    widgetPlugins.append ( plugin );
}

QWidget *KommanderFactory::createWidget ( const QString &literalClassName, QWidget *parent, const char *name )
{
    QString className = literalClassName;
    /*
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
        return new Q3GroupBox(parent, name);
      else if (className == "QButtonGroup")
        return new Q3ButtonGroup(parent, name);
      else if (className == "QIconView")
      {
    #if !defined(QT_NO_ICONVIEW)
        return new Q3IconView(parent, name);
    #endif
      }
      else if (className == "QTable")
      {
    #if !defined(QT_NO_TABLE)
        return new Q3Table(parent, name);
    #endif
      }
      else if (className == "QListBox")
        return new Q3ListBox(parent, name);
      else if (className == "QListView")
        return new Q3ListView(parent, name);
      else if (className == "QLineEdit")
        return new QLineEdit(parent, name);
      else if (className == "QSpinBox")
        return new QSpinBox(parent, name);
      else if (className == "QMultiLineEdit")
        return new Q3MultiLineEdit(parent, name);
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
        return new Q3Wizard(parent, name);
      else if (className == "QLCDNumber")
        return new QLCDNumber(parent, name);
      else if (className == "QProgressBar")
        return new Q3ProgressBar(parent, name);
      else if (className == "QTextView")
        return new Q3TextView(parent, name);
      else if (className == "QTextBrowser")
        return new Q3TextBrowser(parent, name);
      else if (className == "QDial")
        return new QDial(parent, name);
      else if (className == "QSlider")
        return new QSlider(parent, name);
      else if (className == "QFrame")
        return new Q3Frame(parent, name);mes
      else if (className == "QSplitter")rem
        return new QSplitter(parent, name);
      else if (className == "Line")
      {
        Q3Frame *f = new Q3Frame(parent, name);
        f->setFrameStyle(Q3Frame::HLine | Q3Frame::Sunken);
        return f;
      }
      else if (className == "QTextEdit")
        return new Q3TextEdit(parent, name);
      else if (className == "QDateEdit")
        return new Q3DateEdit(parent, name);
      else if (className == "QTimeEdit")
        return new Q3TimeEdit(parent, name);
      else if (className == "QDateTimeEdit")
        return new Q3DateTimeEdit(parent, name);
      else if (className == "QScrollBar")
        return new QScrollBar(parent, name);
      else if (className == "QPopupMenu")
        return new Q3PopupMenu(parent, name);
      else if (className == "QWidgetStack")
        return new Q3WidgetStack(parent, name);
      else if (className == "QMainWindow")
      {
        Q3MainWindow *mw = 0;
        if (!qwf_stays_on_top)
          mw = new Q3MainWindow(parent, name);
        else
          mw = new Q3MainWindow(parent, name, Qt::WType_TopLevel | Qt::WStyle_StaysOnTop);
        mw->setCentralWidget(new QWidget(mw, "qt_central_widget"));
        mw->centralWidget()->show();
        (void) mw->statusBar();
        return mw;
      }
    */
    // try to create it using the loaded kommander widget plugins
    //find the widget plugin which can create className

    for ( int i = 0; i < widgetPlugins.size(); i++ )
    {
        KommanderPlugin * p = widgetPlugins.at ( i );
        QWidget *w = p->create ( className, parent, name );
        if ( w )
            return w;
    }

    // no success
    return 0;
}

static int num_plugins_loaded = 0;

int KommanderFactory::loadPlugins ( bool force )
{
    if ( num_plugins_loaded > 0 && !force )
        return num_plugins_loaded;

    num_plugins_loaded = 0;
    KConfig config ( "kommanderrc" );
    KConfigGroup cfg ( &config, "plugins" );
    QStringList plugins;
    plugins << "libkommanderwidgets";
    plugins += cfg.readEntry ( "plugins" );
    QStringList::Iterator it;
    KLibLoader *f = KLibLoader::self(); //FIXME: Use KPluginLoader
    for ( it = plugins.begin(); it != plugins.end(); ++it )
    {
        KLibrary *l = f->library ( ( *it ) );
        if ( l )
        {
            void *addr = l->resolveSymbol ( "kommander_plugin" );
            if ( addr )
            {
                void * ( *kommander_plugin ) () = ( void * ( * ) () ) l->resolveSymbol ( "kommander_plugin" );
                KommanderPlugin *p = ( KommanderPlugin * ) ( *kommander_plugin ) ();
                widgetPlugins.append ( p );
                ++num_plugins_loaded;
            }
            else
            {
                kWarning() << "KommanderFactory::loadPlugins - "<< l->fileName() << "isn't a Kommander Plugin library, skipping." ;
            }
        }
        else
            if ( ! ( *it ).isEmpty() )
            {
                kWarning() << "KommanderFactory::loadPlugins - Can't load Kommander plugin library " << *it;
            }
    }
    //qDebug("KommanderFactory::loadPlugins returning %d", num_plugins_loaded);
    return num_plugins_loaded;
}

FeatureList KommanderFactory::featureList()
{
    FeatureList features;
    for ( int i = 0; i < widgetPlugins.size(); i++ )
    {
        KommanderPlugin * p = widgetPlugins.at ( i );
        QStringList widgets = p->widgets();
        QStringList::Iterator it;
        for ( it = widgets.begin() ; it != widgets.end() ; ++it )
        {
            QString wn = *it;
            features[wn] = KommanderWidgetInfo ( p->group ( wn ), p->toolTip ( wn ), p->whatsThis ( wn ), p->isContainer ( wn ) );
        }
    }
    return features;
    //iterate through widgetPlugins, appending KommanderPlugin::widgets() to features
}

QWidget *KommanderFactory::createWidgetInternal ( const QDomElement &e, QWidget *parent, QLayout* layout, const QString &classNameArg )
{
    lastItem = 0;
    QDomElement n = e.firstChild().toElement();
    QWidget *w = 0; // the widget that got created
    QObject *obj = 0; // gets the properties

    QString className = classNameArg;

    int row = e.attribute ( "row" ).toInt();
    int col = e.attribute ( "column" ).toInt();
    int rowspan = e.attribute ( "rowspan" ).toInt();
    int colspan = e.attribute ( "colspan" ).toInt();
    if ( rowspan < 1 )
        rowspan = 1;
    if ( colspan < 1 )
        colspan = 1;
    if ( !className.isEmpty() )
    {
        if ( !layout && className  == "QLayoutWidget" )
            className = "QWidget";
        if ( layout && className == "QLayoutWidget" )
        {
            // hide layout widgets
            w = parent;
        }
        else
        {
            obj = KommanderFactory::createWidget ( className, parent, 0 );
            if ( !obj )
            {
                return 0;
            }
            w = ( QWidget* ) obj;
            if ( !toplevel )
                toplevel = w;
            if ( qobject_cast<QMainWindow*>(w) )
                w = qobject_cast<QMainWindow*>(w)->centralWidget();
            if ( layout )
            {
                switch ( layoutType ( layout ) )
                {
                    case HBox:
                        ( ( QHBoxLayout* ) layout )->addWidget ( w );
                        break;
                    case VBox:
                        ( ( QVBoxLayout* ) layout )->addWidget ( w );
                        break;
                    case Grid:
                        static_cast< QGridLayout*>(layout)->addWidget ( w, row, col, row + rowspan - 1,
                                col + colspan - 1 );
                        break;
                    default:
                        break;
                }
            }

            layout = 0;
        }
    }

    while ( !n.isNull() )
    {
        if ( n.tagName() == "spacer" )
        {
            createSpacer ( n, layout );
        }
        else if ( n.tagName() == "widget" )
        {
            QMap< QString, QString> *oldDbControls = dbControls;
            createWidgetInternal ( n, w, layout, n.attribute ( "class", "QWidget" ) );
            dbControls = oldDbControls;
        }
        else if ( n.tagName() == "hbox" )
        {
            QLayout *parentLayout = layout;
            if ( layout && qobject_cast<QGridLayout*>(layout) )
                layout = createLayout ( 0, 0, KommanderFactory::HBox );
            else
                layout = createLayout ( w, layout, KommanderFactory::HBox );
            obj = layout;
            n = n.firstChild().toElement();
            if ( parentLayout && qobject_cast<QGridLayout*>(parentLayout) )
                qobject_cast<QGridLayout*>(parentLayout)->addLayout ( layout, row, row + rowspan - 1, col, col + colspan - 1 );
            continue;
        }
        else if ( n.tagName() == "grid" )
        {
            QLayout *parentLayout = layout;
            if ( layout && qobject_cast<QGridLayout*>(layout) )
                layout = createLayout ( 0, 0, KommanderFactory::Grid );
            else
                layout = createLayout ( w, layout, KommanderFactory::Grid );
            obj = layout;
            n = n.firstChild().toElement();
            if ( parentLayout && qobject_cast<QGridLayout*>(parentLayout) )
                qobject_cast<QGridLayout*>(parentLayout)->addLayout ( layout, row, row + rowspan - 1, col, col + colspan - 1 );
            continue;
        }
        else if ( n.tagName() == "vbox" )
        {
            QLayout *parentLayout = layout;
            if ( layout && qobject_cast<QGridLayout*>(layout) )
                layout = createLayout ( 0, 0, KommanderFactory::VBox );
            else
                layout = createLayout ( w, layout, KommanderFactory::VBox );
            obj = layout;
            n = n.firstChild().toElement();
            if ( parentLayout && qobject_cast<QGridLayout*>(parentLayout) )
                qobject_cast<QGridLayout*>(parentLayout)->addLayout ( layout, row, row + rowspan - 1, col, col + colspan - 1 );
            continue;
        }
        else if ( n.tagName() == "property" && obj )
        {
            setProperty ( obj, n.attribute ( "name" ), n.firstChild().toElement() );
        }
        else if ( n.tagName() == "attribute" && w )
        {
            QString attrib = n.attribute ( "name" );
            QVariant v = DomTool::elementToVariant ( n.firstChild().toElement(), QVariant() );
            if ( qobject_cast<QTabWidget*>(parent) )
            {
                if ( attrib == "title" )
                    qobject_cast<QTabWidget*>(parent)->insertTab ( -1, w, translate ( v.toString() ) );
            }
            else if ( qobject_cast<KAssistantDialog*>(parent) )
            {
                if ( attrib == "title" )
                    qobject_cast<KAssistantDialog*>(parent)->addPage ( w, translate ( v.toString() ) );
            }
        }
        else if ( n.tagName() == "item" )
        {
            createItem ( n, w );
        }
        else if ( n.tagName() == "column" || n.tagName() == "row" )
        {
            createColumn ( n, w );
        }

        n = n.nextSibling().toElement();
    }

    return w;
}

QLayout *KommanderFactory::createLayout ( QWidget *widget, QLayout*  layout, LayoutType type )
{
    int spacing = defSpacing;
    int margin = defMargin;

    if ( !layout && widget && qobject_cast<QTabWidget*>(widget) )
        widget = qobject_cast<QTabWidget*>(widget)->currentWidget();
    if ( !layout && widget && qobject_cast<QToolBox*>(widget) )
      widget = qobject_cast<QToolBox*>(widget)->widget(qobject_cast<QToolBox*>(widget)->currentIndex());
    if ( !layout && widget && qobject_cast<QWizard*>(widget) )
        widget = qobject_cast<QWizard*>(widget)->currentPage();

//FIXME
//     if ( !layout && widget && widget->inherits( "QStackedWidget" ) )
//     widget = ((QStackedWidget*)widget)->visibleWidget();

    if ( !layout && widget && qobject_cast<QGroupBox*>(widget) )
    {
        QGroupBox *gb = qobject_cast<QGroupBox*>(widget);
        QLayout *gbLayout = gb->layout();
        //gb->setLayout(new QVBoxLayout());
        gbLayout->setMargin ( 0 );
        gbLayout->setSpacing ( 0 );
        QLayout *l;
        switch ( type )
        {
            case HBox:
                l = new QHBoxLayout();
                gbLayout->addItem ( l );
                l->setAlignment ( Qt::AlignTop );
                return l;
            case VBox:
                l = new QVBoxLayout();
                gbLayout->addItem ( l );
                l->setSpacing ( spacing );
                l->setAlignment ( Qt::AlignTop );
                return l;
            case Grid:
                l = new QGridLayout();
                gbLayout->addItem ( l );
                l->setAlignment ( Qt::AlignTop );
                return l;
            default:
                return 0;
        }
    }
    else
    {
        if ( layout )
        {
            QLayout *l;
            switch ( type )
            {
                case HBox:
                    l = new QHBoxLayout();
                    layout->addItem ( l );
                    l->setSpacing ( spacing );
                    l->setMargin ( margin );
                    return l;
                case VBox:
                    l = new QVBoxLayout();
                    layout->addItem ( l );
                    l->setSpacing ( spacing );
                    l->setMargin ( margin );
                    return l;
                case Grid:
                {
                    l = new QGridLayout();
                    layout->addItem ( l );
                    l->setSpacing ( spacing );
                    l->setMargin ( margin );
                    return l;
                }
                default:
                    return 0;
            }
        }
        else
        {
            QLayout *l = 0L;
            switch ( type )
            {
                case HBox:
                {
                    l = new QHBoxLayout;
                    break;
                }
                case VBox:
                {
                    l = new QVBoxLayout;
                    break;
                }
                case Grid:
                {
                    l = new QGridLayout;
                    break;
                }
                default:
                    return 0;
            }
            if ( !widget )
            {
                l->setMargin(margin);
                l->setSpacing(spacing);
            } else
            { 
              widget->setLayout(l);
            }
            return l;
        }
    }
    return 0;
}

KommanderFactory::LayoutType KommanderFactory::layoutType ( QLayout *layout ) const
{
    if ( qobject_cast<QHBoxLayout*>(layout) )
        return HBox;
    else if ( qobject_cast<QVBoxLayout*>(layout) )
        return VBox;
    else if ( qobject_cast<QGridLayout*>(layout) )
        return Grid;
    return NoLayout;
}

void KommanderFactory::setProperty ( QObject* obj, const QString &prop, const QDomElement &e )
{
    const QMetaProperty p = obj->metaObject()->property ( obj->metaObject()->indexOfProperty ( prop.toAscii() ) );

    QVariant defVariant;
    if ( e.tagName() == "font" )
    {
        QFont f ( qApp->font() );
        if ( obj->isWidgetType() && ( ( QWidget* ) obj )->parentWidget() )
            f = ( ( QWidget* ) obj )->parentWidget()->font();
        defVariant = QVariant ( f );
    }

    QString comment;
    QVariant v ( DomTool::elementToVariant ( e, defVariant, comment ) );

    if ( e.tagName() == "pixmap" )
    {
        QPixmap pix = loadPixmap ( e );
        if ( pix.isNull() )
            return;
        v = QVariant ( pix );
    }
    else if ( e.tagName() == "iconset" )
    {
        QPixmap pix = loadPixmap ( e );
        if ( pix.isNull() )
            return;
        v = QVariant ( QIcon ( pix ) );
    }
    else if ( e.tagName() == "image" )
    {
        v = QVariant ( loadFromCollection ( v.toString() ) );
    }
    else if ( e.tagName() == "string" )
    {
        v = QVariant ( translate ( v.toString(), comment ) );
    }

    if ( !p.isValid() )
    {
        if ( obj->isWidgetType() )
        {
            if ( prop == "toolTip" )
            {
                if ( !v.toString().isEmpty() )
                    ( ( QWidget* ) obj )->setToolTip ( translate ( v.toString() ) );
            }
            else if ( prop == "whatsThis" )
            {
                if ( !v.toString().isEmpty() )
                    ( ( QWidget* ) obj )->setWhatsThis ( translate ( v.toString() ) );
            }
            if ( prop == "buddy" )
            {
                buddies.insert ( obj->objectName(), v.toByteArray() );
            }
            else if ( prop == "frameworkCode" )
            {
                if ( !DomTool::elementToVariant ( e, QVariant ( true, 0 ) ).toBool() )
                {
                    noDatabaseWidgets << obj->objectName();
                }
            }
            else if ( prop == "buttonGroupId" )
            {
                if ( qobject_cast<QAbstractButton*>(obj) && qobject_cast<QButtonGroup*>(obj->parent()) )
                    qobject_cast<QButtonGroup*>(obj->parent())->addButton ( qobject_cast<QAbstractButton*>(obj), v.toInt() );
            }

            return;
        }
    }

    if ( e.tagName() == "palette" )
    {
        QDomElement n = e.firstChild().toElement();
        QPalette p;
        while ( !n.isNull() )
        {
            QColorGroup cg;
            if ( n.tagName() == "active" )
            {
                cg = loadColorGroup ( n );
                p.setActive ( cg );
            }
            else if ( n.tagName() == "inactive" )
            {
                cg = loadColorGroup ( n );
                p.setInactive ( cg );
            }
            else if ( n.tagName() == "disabled" )
            {
                cg = loadColorGroup ( n );
                p.setDisabled ( cg );
            }
            n = n.nextSibling().toElement();
        }
        v = QPalette ( p );
    }
    else if ( e.tagName() == "enum" && p.isValid() && p.isEnumType() )
    {
        QString key ( v.toString() );
        v = QVariant ( p.enumerator().keyToValue ( key.toAscii() ) );
    }
    else if ( e.tagName() == "set" && p.isValid() && p.isFlagType() )
    {
        QString keys ( v.toString() );
        v = QVariant ( p.enumerator().keysToValue ( keys.toAscii() ) );
    }

    if ( prop == "geometry" )
    {
        if ( obj == toplevel )
        {
            toplevel->resize ( v.toRect().size() );
            return;
        }
    }

    obj->setProperty ( prop.toAscii(), v );
}

void KommanderFactory::createSpacer ( const QDomElement &e, QLayout *layout )
{
    QDomElement n = e.firstChild().toElement();
    int row = e.attribute ( "row" ).toInt();
    int col = e.attribute ( "column" ).toInt();
    int rowspan = e.attribute ( "rowspan" ).toInt();
    int colspan = e.attribute ( "colspan" ).toInt();

    Qt::Orientation orient = Qt::Horizontal;
    int w = 0, h = 0;
    QSizePolicy::SizeType sizeType = QSizePolicy::Preferred;
    while ( !n.isNull() )
    {
        if ( n.tagName() == "property" )
        {
            QString prop = n.attribute ( "name" );
            if ( prop == "orientation" )
            {
                if ( n.firstChild().firstChild().toText().data() == "Horizontal" )
                    orient = Qt::Horizontal;
                else
                    orient = Qt::Vertical;
            }
            else if ( prop == "sizeType" )
            {
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
            }
            else if ( prop == "sizeHint" )
            {
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
    QSpacerItem *item = new QSpacerItem ( w, h, orient == Qt::Horizontal ? sizeType : QSizePolicy::Minimum,
                                          orient == Qt::Vertical ? sizeType : QSizePolicy::Minimum );
    if ( layout )
    {
        if ( qobject_cast<QBoxLayout*>(layout) )
            qobject_cast<QBoxLayout*>(layout)->addItem ( item );
        else
            qobject_cast<QGridLayout*>(layout)->addItem ( item, row, row + rowspan - 1, col, col + colspan - 1,
                                                   orient == Qt::Horizontal ? Qt::AlignVCenter : Qt::AlignHCenter );
    }
}

static QImage loadImageData ( QDomElement &n2 )
{
    QImage img;
    QString data = n2.firstChild().toText().data();
    char *ba = new char[ data.length() / 2 ];
    for ( int i = 0; i < ( int ) data.length() / 2; ++i )
    {
        char h = data[ 2 * i ].toLatin1();
        char l = data[ 2 * i  + 1 ].toLatin1();
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
    QString format = n2.attribute ( "format", "PNG" );
    if ( format == "XPM.GZ" ) {
        ulong len = n2.attribute( "length" ).toULong();
        if ( len < (ulong) data.length() * 5 )
            len = data.length() * 5;
        QByteArray baunzip(len, ' ');
        ::uncompress( (uchar*) baunzip.data(), &len, (uchar*) ba, data.length()/2 );
        img.loadFromData( (const uchar*)baunzip.data(), len, "XPM" );
    }  else
    {
        img.loadFromData ( ( const uchar* ) ba, data.length() / 2, format.toAscii() );
    }
    delete [] ba;

    return img;
}

void KommanderFactory::loadImageCollection ( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() )
    {
        if ( n.tagName() == "image" )
        {
            Image img;
            img.name =  n.attribute ( "name" );
            QDomElement n2 = n.firstChild().toElement();
            while ( !n2.isNull() )
            {
                if ( n2.tagName() == "data" )
                    img.img = loadImageData ( n2 );
                n2 = n2.nextSibling().toElement();
            }
            images.append ( img );
            n = n.nextSibling().toElement();
        }
    }
}

QImage KommanderFactory::loadFromCollection ( const QString &name )
{
    QList<Image>::Iterator it = images.begin();
    for ( ; it != images.end(); ++it )
    {
        if ( ( *it ).name == name )
            return ( *it ).img;
    }
    return QImage();
}

QPixmap KommanderFactory::loadPixmap ( const QDomElement &e )
{
    QString arg = e.firstChild().toText().data();
    // try to fix old way of storing data
    if ( usePixmapCollection )
    {
        /*QMimeData m;
        m.setData("",arg);
        if ( !m->hasImage() )
        return QPixmap();
        QPixmap pix;
        pix = qvariant_cast<QImage>m.imageData();
        return pix;*/
    }

    return QPixmap::fromImage(loadFromCollection(arg));
}

QColorGroup KommanderFactory::loadColorGroup ( const QDomElement &e )
{
    QColorGroup cg;
    int r = -1;
    QDomElement n = e.firstChild().toElement();
    QColor col;
    while ( !n.isNull() )
    {
        if ( n.tagName() == "color" )
        {
            r++;
            cg.setColor ( ( QPalette::ColorRole ) r, ( col = DomTool::readColor ( n ) ) );
        }
        else if ( n.tagName() == "pixmap" )
        {
            QPixmap pix = loadPixmap ( n );
            cg.setBrush ( ( QPalette::ColorRole ) r, QBrush ( col, pix ) );
        }
        n = n.nextSibling().toElement();
    }
    return cg;
}

struct Connection
{
    QObject *sender, *receiver;
    QByteArray signal, slot;
    bool operator== ( const Connection &c ) const
    {
        return sender == c.sender && receiver == c.receiver &&
               signal == c.signal && slot == c.slot ;
    }
};


void KommanderFactory::loadConnections ( const QDomElement &e, QObject *connector )
{
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() )
    {
        if ( n.tagName() == "connection" )
        {
            QString lang = n.attribute ( "language", "C++" );
            QDomElement n2 = n.firstChild().toElement();
            Connection conn;
            while ( !n2.isNull() )
            {
                if ( n2.tagName() == "sender" )
                {
                    QString name = n2.firstChild().toText().data();
                    if ( name == "this" || qstrcmp ( toplevel->objectName().toAscii(), name.toAscii() ) == 0 )
                    {
                        conn.sender = toplevel;
                    }
                    else
                    {
                        if ( name == "this" )
                            name = toplevel->objectName();
                        QList<QObject *> l = toplevel->findChildren<QObject *> ( name );
                        if ( !l.isEmpty() )
                        {
                            if ( l.first() )
                                conn.sender = l.first();
                        }
                    }
                    if ( !conn.sender )
                        conn.sender = findAction ( name );
                }
                else if ( n2.tagName() == "signal" )
                {
                    conn.signal = n2.firstChild().toText().data().toAscii();
                }
                else if ( n2.tagName() == "receiver" )
                {
                    QString name = n2.firstChild().toText().data();
                    if ( name == "this" || qstrcmp ( toplevel->objectName().toAscii(), name.toAscii() ) == 0 )
                    {
                        conn.receiver = toplevel;
                    }
                    else
                    {
                        QList<QObject *> l = toplevel->findChildren<QObject *> ( name );
                        if ( !l.isEmpty() )
                        {
                            if ( l.first() )
                                conn.receiver = l.first();
                        }
                    }
                }
                else if ( n2.tagName() == "slot" )
                {
                    conn.slot = n2.firstChild().toText().data().toAscii();
                }
                n2 = n2.nextSibling().toElement();
            }

            conn.signal = QMetaObject::normalizedSignature ( conn.signal );
            conn.slot = QMetaObject::normalizedSignature ( conn.slot );

            QObject *sender = 0, *receiver = 0;
            QList<QObject *> l = toplevel->findChildren<QObject *> ( conn.sender->objectName() );
            if ( qstrcmp ( conn.sender->objectName().toAscii(), toplevel->objectName().toAscii() ) == 0 )
            {
                sender = toplevel;
            }
            else
            {
                if ( l.isEmpty() || !l.first() )
                {
                    n = n.nextSibling().toElement();
                    continue;
                }
                sender = l.first();
            }
            if ( !sender )
                sender = findAction ( conn.sender->objectName() );

            if ( qstrcmp ( conn.receiver->objectName().toAscii(), toplevel->objectName().toAscii() ) == 0 )
            {
                receiver = toplevel;
            }
            else
            {
                l = toplevel->findChildren<QObject *> ( conn.receiver->objectName() );
                if ( l.isEmpty() || !l.first() )
                {
                    n = n.nextSibling().toElement();
                    continue;
                }
                receiver = l.first();
            }

            if ( lang == "C++" )
            {
                QString s = "2""%1";
                s = s.arg ( QString ( conn.signal ) );
                QString s2 = "1""%1";
                s2 = s2.arg ( QString ( conn.slot ) );

                // if this is a connection to a custom slot and we have a connector, try this as receiver
                if ( receiver->metaObject()->indexOfSlot ( conn.slot ) == -1 && receiver == toplevel && connector )
                {
                    receiver = connector;
                }
                // avoid warnings
                if ( sender->metaObject()->indexOfSignal ( conn.signal ) == -1 ||
                        receiver->metaObject()->indexOfSlot ( conn.slot ) == -1 )
                {
                    n = n.nextSibling().toElement();
                    continue;
                }
                QObject::connect ( sender, s.toAscii(), receiver, s2.toAscii() );
            }
            else
            {
                EventFunction ef = eventMap[ conn.sender ];
                ef.events.append ( conn.signal );
                ef.functions.append ( QString ( conn.slot ).split ( ',' ) );
                eventMap.replace ( conn.sender, ef );
            }
        }
        else if ( n.tagName() == "slot" )
        {
            QString s = n.firstChild().toText().data();
            languageSlots.insert ( s.left ( s.indexOf ( "(" ) ) , n.attribute ( "language" ) );
        }
        n = n.nextSibling().toElement();
    }
}

void KommanderFactory::loadTabOrder ( const QDomElement &e )
{
    QWidget *last = 0;
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() )
    {
        if ( n.tagName() == "tabstop" )
        {
            QString name = n.firstChild().toText().data();
            QList<QWidget *> l = toplevel->findChildren<QWidget *> ( name );
            if ( !l.isEmpty() )
            {
                if ( l.first() )
                {
                    QWidget *w = l.first();
                    if ( last )
                        toplevel->setTabOrder ( last, w );
                    last = w;
                }
            }
        }
        n = n.nextSibling().toElement();
    }
}

void KommanderFactory::createColumn ( const QDomElement &e, QWidget *widget )
{
    if ( qobject_cast<Q3ListView*>(widget) && e.tagName() == "column" )
    {
        Q3ListView *lv = qobject_cast<Q3ListView*>(widget);
        QDomElement n = e.firstChild().toElement();
        QPixmap pix;
        bool hasPixmap = false;
        QString txt;
        bool clickable = true, resizeable = true;
        while ( !n.isNull() )
        {
            if ( n.tagName() == "property" )
            {
                QString attrib = n.attribute ( "name" );
                QVariant v = DomTool::elementToVariant ( n.firstChild().toElement(), QVariant() );
                if ( attrib == "text" )
                    txt = translate ( v.toString() );
                else if ( attrib == "pixmap" )
                {
                    pix = loadPixmap ( n.firstChild().toElement().toElement() );
                    hasPixmap = !pix.isNull();
                }
                else if ( attrib == "clickable" )
                    clickable = v.toBool();
                else if ( attrib == "resizeable" )
                    resizeable = v.toBool();
            }
            n = n.nextSibling().toElement();
        }
        lv->addColumn ( txt );
        /*FIXME int i = lv->header()->count() - 1;
        if ( hasPixmap ) {
            lv->header()->setLabel( i, pix, txt );
        }
        if ( !clickable )
            lv->header()->setClickEnabled( clickable, i );
        if ( !resizeable )
            lv->header()->setResizeEnabled( resizeable, i );
            */
    }
    else if ( qobject_cast<QTableWidget*>(widget) )
    {
        QTableWidget *table = qobject_cast<QTableWidget*>(widget);
        bool isRow;
        if ( ( isRow = e.tagName() == "row" ) )
            table->setRowCount ( table->rowCount() + 1 );
        else
        {
            table->setColumnCount ( table->columnCount() + 1 );
        }

        QDomElement n = e.firstChild().toElement();
        QPixmap pix;
        bool hasPixmap = false;
        QString txt;
        QString field;
        QList<Field> fieldMap;
        if ( fieldMaps.count ( table ) > 0 )
        {
            fieldMap = fieldMaps.take ( table );
        }
        while ( !n.isNull() )
        {
            if ( n.tagName() == "property" )
            {
                QString attrib = n.attribute ( "name" );
                QVariant v = DomTool::elementToVariant ( n.firstChild().toElement(), QVariant() );
                if ( attrib == "text" )
                    txt = translate ( v.toString() );
                else if ( attrib == "pixmap" )
                {
                    hasPixmap = !n.firstChild().firstChild().toText().data().isEmpty();
                    if ( hasPixmap )
                        pix = loadPixmap ( n.firstChild().toElement().toElement() );
                }
                else if ( attrib == "field" )
                    field = v.toString();
            }
            n = n.nextSibling().toElement();
        }

        int i = isRow ? table->rowCount() - 1 : table->columnCount() - 1;
        QTableWidgetItem *header;
        if (isRow)
        {
          header = table->verticalHeaderItem(i);
          if (!header)
          {
             header = new QTableWidgetItem;
             table->setVerticalHeaderItem(i, header);
          }
        } else
        {
          header = table->horizontalHeaderItem(i);
          if (!header)
          {
            header = new QTableWidgetItem;
            table->setHorizontalHeaderItem(i, header);
          }
        }
                                          
        if (hasPixmap)
        {
          header->setIcon(pix);
        }
        else
        {
          header->setText(txt);
        }
        if ( !isRow && !field.isEmpty() )
        {
            fieldMap.append ( Field ( txt, ( hasPixmap ? pix : QPixmap() ), field ) );
            // FIXME QMap with private
            fieldMaps.insert ( table, fieldMap );
        }
    }
}

void KommanderFactory::loadItem ( const QDomElement &e, QPixmap &pix, QString &txt, bool &hasPixmap )
{
    QDomElement n = e;
    hasPixmap = false;
    while ( !n.isNull() )
    {
        if ( n.tagName() == "property" )
        {
            QString attrib = n.attribute ( "name" );
            QVariant v = DomTool::elementToVariant ( n.firstChild().toElement(), QVariant() );
            if ( attrib == "text" )
                txt = translate ( v.toString() );
            else if ( attrib == "pixmap" )
            {
                pix = loadPixmap ( n.firstChild().toElement() );
                hasPixmap = !pix.isNull();
            }
        }
        n = n.nextSibling().toElement();
    }
}

void KommanderFactory::createItem ( const QDomElement &e, QWidget *widget, Q3ListViewItem *i )
{
    if (qobject_cast<Q3ListBox*>(widget) || qobject_cast<QComboBox*>(widget))
    {
        QDomElement n = e.firstChild().toElement();
        QPixmap pix;
        bool hasPixmap = false;
        QString txt;
        loadItem(n, pix, txt, hasPixmap);
        Q3ListBox *lb = 0;
        if (qobject_cast<Q3ListBox*>(widget))
        {
            lb = qobject_cast<Q3ListBox*>(widget);
            if (hasPixmap) {
              new Q3ListBoxPixmap(lb, pix, txt);
            } else {
              new Q3ListBoxText(lb, txt);
            }
        } 
        else if (qobject_cast<QComboBox*>(widget))
        { 
          QComboBox *cb = qobject_cast<QComboBox*>(widget);
          if (hasPixmap) {
            cb->addItem(pix, txt);
          } else {
            cb->addItem(txt);
          }
          
        }  
        /*#ifndef QT_NO_ICONVIEW
            } else if ( widget->inherits( "QIconView" ) ) {
            QDomElement n = e.firstChild().toElement();
            QPixmap pix;
            bool hasPixmap = false;
            QString txt;
            loadItem( n, pix, txt, hasPixmap );

            QIconView *iv = (QIconView*)widget;
            new QIconViewItem( iv, txt, pix );
        #endif*/
    }
    else if ( qobject_cast<Q3ListView*>(widget) )
    {
        QDomElement n = e.firstChild().toElement();
        QPixmap pix;
        QList<QPixmap> pixmaps;
        QStringList textes;
        Q3ListViewItem *item = 0;
        Q3ListView *lv = qobject_cast<Q3ListView*>(widget);
        if ( i )
            item = new Q3ListViewItem ( i, lastItem ); // lastitem FIXME
        else
            item = new Q3ListViewItem ( lv, lastItem );
        while ( !n.isNull() )
        {
            if ( n.tagName() == "property" )
            {
                QString attrib = n.attribute ( "name" );
                QVariant v = DomTool::elementToVariant ( n.firstChild().toElement(), QVariant() );
                if ( attrib == "text" )
                    textes << translate ( v.toString() );
                else if ( attrib == "pixmap" )
                {
                    QString s = v.toString();
                    if ( s.isEmpty() )
                    {
                        pixmaps << QPixmap();
                    }
                    else
                    {
                        pix = loadPixmap ( n.firstChild().toElement() );
                        pixmaps << pix;
                    }
                }
            }
            else if ( n.tagName() == "item" )
            {
                //item->setOpen( true );
                createItem ( n, widget, item );
            }

            n = n.nextSibling().toElement();
        }

        for ( int i = 0; i < lv->columns(); ++i )
        {
            item->setText (i, textes[ i ] );
            item->setPixmap (i, pixmaps[ i ] );
        }
        lastItem = item;
    }
}



void KommanderFactory::loadChildAction ( QObject *parent, const QDomElement &e )
{
    QDomElement n = e;
    QAction *a = 0;
    EventFunction ef;
    if ( n.tagName() == "action" )
    {
        a = new QAction ( parent );
        QDomElement n2 = n.firstChild().toElement();
        while ( !n2.isNull() )
        {
            if ( n2.tagName() == "property" )
            {
                setProperty ( a, n2.attribute ( "name" ), n2.firstChild().toElement() );
            }
            else if ( n2.tagName() == "event" )
            {
                ef.events.append ( n2.attribute ( "name" ) );
                ef.functions.append ( n2.attribute ( "functions" ).split ( ',' ) );
            }
            n2 = n2.nextSibling().toElement();
        }
        if ( !qobject_cast<QAction*>(parent) )
            actionList.append ( a );
    }
    else if ( n.tagName() == "actiongroup" )
    {
        /* FIXMEE   a = new QActionGroup( parent );
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
        */    }
    if ( a )
        eventMap.insert ( a, ef );
}

void KommanderFactory::loadActions ( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() )
    {
        if ( n.tagName() == "action" )
        {
            loadChildAction ( toplevel, n );
        }
        else if ( n.tagName() == "actiongroup" )
        {
            loadChildAction ( toplevel, n );
        }
        n = n.nextSibling().toElement();
    }
}

/*not used yet*/
void KommanderFactory::loadToolBars ( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    QMainWindow *mw = ( ( QMainWindow* ) toplevel );
    QToolBar *tb = 0;
    while ( !n.isNull() )
    {
        if ( n.tagName() == "toolbar" )
        {
            Qt::ToolBarDock dock = ( Qt::ToolBarDock ) n.attribute ( "dock" ).toInt();
            tb = new QToolBar ( mw );   //krazy:exclude=nullstrassign for old broken gcc
            tb->setWindowTitle ( n.attribute ( "label" ) );
            tb->setObjectName ( n.attribute ( "name" ) );
            QDomElement n2 = n.firstChild().toElement();
            while ( !n2.isNull() )
            {
                if ( n2.tagName() == "action" )
                {
                    QAction *a = findAction ( n2.attribute ( "name" ) );
                    tb->addAction(a);
                }
                else if ( n2.tagName() == "separator" )
                {
                    tb->addSeparator();
                }
                else if ( n2.tagName() == "widget" )
                {
                    ( void ) createWidgetInternal ( n2, tb, 0, n2.attribute ( "class", "QWidget" ) );
                }
                else if ( n2.tagName() == "property" )
                {
                    setProperty ( tb, n2.attribute ( "name" ), n2.firstChild().toElement() );
                }
                n2 = n2.nextSibling().toElement();
            }
        }
        n = n.nextSibling().toElement();
    }
}
/*not used yet*/
void KommanderFactory::loadMenuBar ( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    QMainWindow *mw = ( ( QMainWindow* ) toplevel );
    QMenuBar *mb = mw->menuBar();
    while ( !n.isNull() )
    {
        if ( n.tagName() == "item" )
        {
            QMenu *popup = new QMenu ( mw );
            popup->setObjectName ( n.attribute ( "name" ) );
            QDomElement n2 = n.firstChild().toElement();
            while ( !n2.isNull() )
            {
                if ( n2.tagName() == "action" )
                {
                    QAction *a = findAction ( n2.attribute ( "name" ) );
                    popup->addAction(a);
                }
                else if ( n2.tagName() == "separator" )
                {
                    popup->insertSeparator();
                }
                n2 = n2.nextSibling().toElement();
            }
            mb->insertItem ( translate ( n.attribute ( "text" ) ), popup );
        }
        else if ( n.tagName() == "property" )
        {
            setProperty ( mb, n.attribute ( "name" ), n.firstChild().toElement() );
        }
        n = n.nextSibling().toElement();
    }
}


QAction *KommanderFactory::findAction ( const QString &name )
{
    QListIterator<QAction *> it ( actionList );
    while ( it.hasNext() )
    {
        QAction * a = it.next();
        if ( QString ( a->objectName() ) == name )
            return a;
        QAction *ac = a->findChild<QAction *> ( name.toLatin1() );
        if ( ac )
            return ac;
    }
    return 0;
}

void KommanderFactory::loadImages ( const QString &dir )
{
    QDir d ( dir );
    QStringList l = d.entryList ( QDir::Files );
    for ( QStringList::Iterator it = l.begin(); it != l.end(); ++it )
        ;//Q3MimeSourceFactory::defaultFactory()->setPixmap(*it, QPixmap(d.path() + "/" + *it, "PNG"));
}

QString KommanderFactory::translate ( const QString& sourceText, const QString& comment )
{
//FIXME: why not use i18n ??
    QString result;
    if ( !sourceText.isEmpty() && !comment.isEmpty() )
        result = KGlobal::locale()->translateQt ( comment.toUtf8(), sourceText.toUtf8(),"" );
    else if ( !sourceText.isEmpty() )
        result =  KGlobal::locale()->translateQt ( "",sourceText.toUtf8(),"" );
    else
        return sourceText;
    if (result.isEmpty())
      return sourceText;
}


