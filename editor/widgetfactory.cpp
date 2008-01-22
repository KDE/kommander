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

#include <klocale.h>
#include <kommanderfactory.h>

#include <qvariant.h> // HP-UX compiler need this here
#include "widgetfactory.h"
#include "widgetdatabase.h"
#include "metadatabase.h"
#include "mainwindow.h"
#include "formwindow.h"
#include "pixmapchooser.h"
#include "layout.h"
#include "listboxeditorimpl.h"
#include "listvieweditorimpl.h"
#include "iconvieweditorimpl.h"
#include "multilineeditorimpl.h"
#ifndef KOMMANDER
#include "widgetinterface.h"
#endif
#ifndef QT_NO_TABLE
#include "tableeditorimpl.h"
#endif

#include <qfeatures.h>

#include <qpixmap.h>
#include <qgroupbox.h>
#include <qiconview.h>
#ifndef QT_NO_TABLE
#include <qtable.h>
#endif
#ifndef QT_NO_SQL
#include <qdatatable.h>
#endif
#include <qdatetimeedit.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qmultilineedit.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwidgetstack.h>
#include <qcombobox.h>
#include <qtabbar.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qobjectlist.h>
#include <qlcdnumber.h>
#include <qslider.h>
#include <qdial.h>
#include <qprogressbar.h>
#include <qtextview.h>
#include <qtextbrowser.h>
#include <qframe.h>
#include <qmetaobject.h>
#include <qwizard.h>
#include <qvaluelist.h>
#include <qtimer.h>
#include <qscrollbar.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qsplitter.h>
#ifndef QT_NO_SQL
#include "database.h"
#endif

//#define NO_STATIC_COLORS
#include "globaldefs.h"

/* KOMMANDER INCLUDES */
#include <lineedit.h>
#include <dialog.h>
#include <execbutton.h>
#include <closebutton.h>
#include <textedit.h>
#include <fileselector.h>
#include <textedit.h>
#include <radiobutton.h>
#include <groupbox.h>
#include <buttongroup.h>
#include <checkbox.h>
#include <combobox.h>
#include <spinboxint.h>
#include <wizard.h>
#include <tabwidget.h>
#include <subdialog.h>
#include <listbox.h>
#include <scriptobject.h>
#include <richtexteditor.h>
#include <treewidget.h>
#include <slider.h>
#include <datepicker.h>
#include <popupmenu.h>
#include "toolbox.h"
#include "colordialog.h"
#include "aboutdialog.h"

FormWindow *find_formwindow( QWidget *w )
{
    if ( !w )
	return 0;
    for (;;) {
	if ( w->inherits( "FormWindow" ) )
	    return (FormWindow*)w;
	if ( !w->parentWidget() )
	    return 0;
	w = w->parentWidget();
    }
}

void QLayoutWidget::paintEvent( QPaintEvent* )
{
    QPainter p ( this );
    p.setPen( red );
    p.drawRect( rect() );
}


QDesignerTabWidget::QDesignerTabWidget( QWidget *parent, const char *name )
    : QTabWidget( parent, name ), dropIndicator( 0 ), dragPage( 0 ), mousePressed( false )
{
    tabBar()->setAcceptDrops( true );
    tabBar()->installEventFilter( this );
}

int QDesignerTabWidget::currentPage() const
{
    return tabBar()->currentTab();
}

void QDesignerTabWidget::setCurrentPage( int i )
{
    tabBar()->setCurrentTab( i );
}

QString QDesignerTabWidget::pageTitle() const
{
    return ((QTabWidget*)this)->tabLabel( QTabWidget::currentPage() );
}

void QDesignerTabWidget::setPageTitle( const QString& title )
{
    changeTab( QTabWidget::currentPage(), title );
}

void QDesignerTabWidget::setPageName( const QCString& name )
{
    if ( QTabWidget::currentPage() )
	QTabWidget::currentPage()->setName( name );
}

QCString QDesignerTabWidget::pageName() const
{
    if ( !QTabWidget::currentPage() )
	return 0;
    return QTabWidget::currentPage()->name();
}

int QDesignerTabWidget::count() const
{
    return tabBar()->count();
}

bool QDesignerTabWidget::eventFilter( QObject *o, QEvent *e )
{
    if ( o != tabBar() ) return false;

    switch ( e->type() ) {
    case QEvent::MouseButtonPress: {
	mousePressed = true;
	QMouseEvent *me = (QMouseEvent*)e;
	pressPoint = me->pos();
    }
    break;
    case QEvent::MouseMove: {
	QMouseEvent *me = (QMouseEvent*)e;
	if ( mousePressed && ( pressPoint - me->pos()).manhattanLength() > QApplication::startDragDistance() ) {
	    QTextDrag *drg = new QTextDrag( QString::number( (long) this ) , this );
	    mousePressed = false;
	    dragPage = QTabWidget::currentPage();
	    dragLabel = QTabWidget::tabLabel( dragPage );

	    int index = indexOf( dragPage );

	    removePage( dragPage );
	    if ( !drg->dragMove() ) {
		insertTab( dragPage, dragLabel, index );
		showPage( dragPage );
	    }
	    if ( dropIndicator )
		dropIndicator->hide();
	}
    }
    break;
    case QEvent::DragLeave:	{
	if ( dropIndicator )
	    dropIndicator->hide();
    }
    break;
    case QEvent::DragMove: {
	QDragEnterEvent *de = (QDragEnterEvent*) e;
	if ( QTextDrag::canDecode( de ) ) {
	    QString text;
	    QTextDrag::decode( de, text );
	    if ( text == QString::number( (long)this ) )
		de->accept();
	    else
		return false;
	}

	int index = 0;
	QRect rect;
	for ( ; index < tabBar()->count(); index++ ) {
	    if ( tabBar()->tabAt( index )->rect().contains( de->pos() ) ) {
		rect = tabBar()->tabAt( index )->rect();
		break;
	    }
	}

	if ( index == tabBar()->count() -1 ) {
	    QRect rect2 = rect;
	    rect2.setLeft( rect2.left() + rect2.width() / 2 );
	    if ( rect2.contains( de->pos() ) )
		index++;
	}

	if ( ! dropIndicator ) {
	    dropIndicator = new QWidget( this );
	    dropIndicator->setBackgroundColor( red );
	}

	QPoint pos;
	if ( index == tabBar()->count() )
	    pos = tabBar()->mapToParent( QPoint( rect.x() + rect.width(), rect.y() ) );
	else
	    pos = tabBar()->mapToParent( QPoint( rect.x(), rect.y() ) );

	dropIndicator->setGeometry( pos.x(), pos.y() , 3, rect.height() );
	dropIndicator->show();
    }
    break;
    case QEvent::Drop: {
	QDragEnterEvent *de = (QDragEnterEvent*) e;
	if ( QTextDrag::canDecode( de ) ) {
	    QString text;
	    QTextDrag::decode( de, text );
	    if ( text == QString::number( (long)this ) ) {

		int newIndex = 0;
		for ( ; newIndex < tabBar()->count(); newIndex++ ) {
		    if ( tabBar()->tabAt( newIndex )->rect().contains( de->pos() ) )
			break;
		}

		if ( newIndex == tabBar()->count() -1 ) {
		    QRect rect2 = tabBar()->tabAt( newIndex )->rect();
		    rect2.setLeft( rect2.left() + rect2.width() / 2 );
		    if ( rect2.contains( de->pos() ) )
			newIndex++;
		}

		int oldIndex = 0;
		for ( ; oldIndex < tabBar()->count(); oldIndex++ ) {
		    if ( tabBar()->tabAt( oldIndex )->rect().contains( pressPoint ) )
			break;
		}

		FormWindow *fw = find_formwindow( this );
		MoveTabPageCommand *cmd =
		    new MoveTabPageCommand( i18n("Move Tab Page" ), fw, this,
					    dragPage, dragLabel, newIndex, oldIndex );
		fw->commandHistory()->addCommand( cmd );
		cmd->execute();
		de->accept();
	    }
	}
    }
    break;
    default:
	break;
    }
    return false;
}

int QDesignerWizard::currentPageNum() const
{
    for ( int i = 0; i < pageCount(); ++i ) {
	if ( page( i ) == currentPage() )
	    return i;
    }
    return 0;
}

void QDesignerWizard::setCurrentPage( int i )
{
    if ( i < currentPageNum() ) {
	while ( i < currentPageNum() ) {
	    if ( currentPageNum() == 0 )
		break;
	    back();
	}

    } else {
	while ( i > currentPageNum() ) {
	    if ( currentPageNum() == pageCount() - 1 )
		break;
	    next();
	}
    }
}

QString QDesignerWizard::pageTitle() const
{
    return title( currentPage() );
}

void QDesignerWizard::setPageTitle( const QString& title )
{
    setTitle( currentPage(), title );
}

void QDesignerWizard::setPageName( const QCString& name )
{
    if ( QWizard::currentPage() )
	QWizard::currentPage()->setName( name );
}

QCString QDesignerWizard::pageName() const
{
    if ( !QWizard::currentPage() )
	return 0;
    return QWizard::currentPage()->name();
}

int QDesignerWizard::pageNum( QWidget *p )
{
    for ( int i = 0; i < pageCount(); ++i ) {
	if ( page( i ) == p )
	    return i;
    }
    return -1;
}

void QDesignerWizard::addPage( QWidget *p, const QString &t )
{
    QWizard::addPage( p, t );
    if ( removedPages.find( p ) )
	removedPages.remove( p );
}

void QDesignerWizard::removePage( QWidget *p )
{
    QWizard::removePage( p );
    removedPages.insert( p, p );
}

void QDesignerWizard::insertPage( QWidget *p, const QString &t, int index )
{
    QWizard::insertPage( p, t, index );
    if ( removedPages.find( p ) )
	removedPages.remove( p );
}

QMap< int, QMap< QString, QVariant> > *defaultProperties = 0;
QMap< int, QStringList > *changedProperties = 0;

/*!
  \class WidgetFactory widgetfactory.h
  \brief Set of static functions for creating widgets, layouts and do other stuff

  The widget factory offers functions to create widgets, create and
  delete layouts find out other details - all based on the
  WidgetDatabase's data. So the functions that use ids use the same
  ids as in the WidgetDatabase.
*/


void WidgetFactory::saveDefaultProperties( QWidget *w, int id )
{
    QMap< QString, QVariant> propMap;
    QStrList lst = w->metaObject()->propertyNames( true );
    for ( uint i = 0; i < lst.count(); ++i ) {
	QVariant var = w->property( lst.at( i ) );
	if ( !var.isValid() && qstrcmp( "pixmap", lst.at( i ) ) == 0 )
	    var = QVariant( QPixmap() );
	else if ( !var.isValid() && qstrcmp( "iconSet", lst.at( i ) ) == 0 )
	    var = QVariant( QIconSet() );
	propMap.replace( lst.at( i ), var );
    }
    defaultProperties->replace( id, propMap );
}

static void saveChangedProperties( QWidget *w, int id )
{
    QStringList l = MetaDataBase::changedProperties( w );
    changedProperties->insert( id, l );
}

EditorTabWidget::EditorTabWidget( QWidget *parent, const char *name )
    : TabWidget( parent, name ), dropIndicator( 0 ), dragPage( 0 ), mousePressed( false )
{
    tabBar()->setAcceptDrops( true );
    tabBar()->installEventFilter( this );
}

int EditorTabWidget::currentPage() const
{
    return tabBar()->currentTab();
}

void EditorTabWidget::setCurrentPage( int i )
{
    tabBar()->setCurrentTab( i );
}

QString EditorTabWidget::pageTitle() const
{
    return ((QTabWidget*)this)->tabLabel( QTabWidget::currentPage() );
}

void EditorTabWidget::setPageTitle( const QString& title )
{
    changeTab( QTabWidget::currentPage(), title );
}

void EditorTabWidget::setPageName( const QCString& name )
{
    if ( QTabWidget::currentPage() )
	QTabWidget::currentPage()->setName( name );
}

QCString EditorTabWidget::pageName() const
{
    if ( !QTabWidget::currentPage() )
	return 0;
    return QTabWidget::currentPage()->name();
}

int EditorTabWidget::count() const
{
    return tabBar()->count();
}

bool EditorTabWidget::eventFilter( QObject *o, QEvent *e )
{
    if ( o != tabBar() ) return false;

    switch ( e->type() ) {
    case QEvent::MouseButtonPress: {
	mousePressed = true;
	QMouseEvent *me = (QMouseEvent*)e;
	pressPoint = me->pos();
    }
    break;
    case QEvent::MouseMove: {
	QMouseEvent *me = (QMouseEvent*)e;
	if ( mousePressed && ( pressPoint - me->pos()).manhattanLength() > QApplication::startDragDistance() ) {
	    QTextDrag *drg = new QTextDrag( QString::number( (long) this ) , this );
	    mousePressed = false;
	    dragPage = QTabWidget::currentPage();
	    dragLabel = QTabWidget::tabLabel( dragPage );

	    int index = indexOf( dragPage );

	    removePage( dragPage );
	    if ( !drg->dragMove() ) {
		insertTab( dragPage, dragLabel, index );
		showPage( dragPage );
	    }
	    if ( dropIndicator )
		dropIndicator->hide();
	}
    }
    break;
    case QEvent::DragLeave:	{
	if ( dropIndicator )
	    dropIndicator->hide();
    }
    break;
    case QEvent::DragMove: {
	QDragEnterEvent *de = (QDragEnterEvent*) e;
	if ( QTextDrag::canDecode( de ) ) {
	    QString text;
	    QTextDrag::decode( de, text );
	    if ( text == QString::number( (long)this ) )
		de->accept();
	    else
		return false;
	}

	int index = 0;
	QRect rect;
	for ( ; index < tabBar()->count(); index++ ) {
	    if ( tabBar()->tabAt( index )->rect().contains( de->pos() ) ) {
		rect = tabBar()->tabAt( index )->rect();
		break;
	    }
	}

	if ( index == tabBar()->count() -1 ) {
	    QRect rect2 = rect;
	    rect2.setLeft( rect2.left() + rect2.width() / 2 );
	    if ( rect2.contains( de->pos() ) )
		index++;
	}

	if ( ! dropIndicator ) {
	    dropIndicator = new QWidget( this );
	    dropIndicator->setBackgroundColor( red );
	}

	QPoint pos;
	if ( index == tabBar()->count() )
	    pos = tabBar()->mapToParent( QPoint( rect.x() + rect.width(), rect.y() ) );
	else
	    pos = tabBar()->mapToParent( QPoint( rect.x(), rect.y() ) );

	dropIndicator->setGeometry( pos.x(), pos.y() , 3, rect.height() );
	dropIndicator->show();
    }
    break;
    case QEvent::Drop: {
	QDragEnterEvent *de = (QDragEnterEvent*) e;
	if ( QTextDrag::canDecode( de ) ) {
	    QString text;
	    QTextDrag::decode( de, text );
	    if ( text == QString::number( (long)this ) ) {

		int newIndex = 0;
		for ( ; newIndex < tabBar()->count(); newIndex++ ) {
		    if ( tabBar()->tabAt( newIndex )->rect().contains( de->pos() ) )
			break;
		}

		if ( newIndex == tabBar()->count() -1 ) {
		    QRect rect2 = tabBar()->tabAt( newIndex )->rect();
		    rect2.setLeft( rect2.left() + rect2.width() / 2 );
		    if ( rect2.contains( de->pos() ) )
			newIndex++;
		}

		int oldIndex = 0;
		for ( ; oldIndex < tabBar()->count(); oldIndex++ ) {
		    if ( tabBar()->tabAt( oldIndex )->rect().contains( pressPoint ) )
			break;
		}

		FormWindow *fw = find_formwindow( this );
		MoveTabPageCommand *cmd =
		    new MoveTabPageCommand( i18n("Move Tab Page" ), fw, this,
					    dragPage, dragLabel, newIndex, oldIndex );
		fw->commandHistory()->addCommand( cmd );
		cmd->execute();
		de->accept();
	    }
	}
    }
    break;
    default:
	break;
    }
    return false;
}


EditorToolBox::EditorToolBox( QWidget *parent, const char *name )
    : ToolBox( parent, name )
{
    setAcceptDrops( true );
}

int EditorToolBox::currentPage() const
{
    return currentIndex();
}

void EditorToolBox::setCurrentPage( int i )
{
    setCurrentItem( item( i ) );
}

QString EditorToolBox::pageTitle() const
{
    return itemLabel( currentIndex() );
}

void EditorToolBox::setPageTitle( const QString& title )
{
    setItemLabel( currentIndex(), title );
}

void EditorToolBox::setPageName( const QCString& name )
{
    if ( currentItem() )
    currentItem()->setName( name );
}

QCString EditorToolBox::pageName() const
{
    if ( !currentItem() )
    return 0;
    return currentItem()->name();
}

int EditorToolBox::count() const
{
    return QToolBox::count();
}



/*!  Creates a widget of the type which is registered as \a id as
  child of \a parent. The \a name is optional. If \a init is true, the
  widget is initialized with some defaults, else the plain widget is
  created.
*/

QWidget *WidgetFactory::create( int id, QWidget *parent, const char *name, bool init, const QRect *r, Qt::Orientation orient )
{
  QString n = WidgetDatabase::className(id);
  //qDebug("Trying to create '%s'", n.latin1());
  if (n.isEmpty())
    return 0;

  if (!defaultProperties)
  {
    defaultProperties = new QMap < int, QMap < QString, QVariant > >();
    changedProperties = new QMap < int, QStringList > ();
  }

  QWidget *w = 0;
  QString str = WidgetDatabase::createWidgetName(id);
  const char *s = str.latin1();
  w = createWidget(n, parent, name ? name : s, init, r, orient);
  //qDebug("Trying to create '%s', widget (id=%d) - %s", s, id, w ? "successful" : "failure");
  if (!w && WidgetDatabase::isCustomWidget(id))
    w = createCustomWidget(parent, name ? name : s, MetaDataBase::customWidget(id));
  if (!w)
    return 0;
  MetaDataBase::addEntry(w);

  if (!defaultProperties->contains(id))
    saveDefaultProperties(w, id);
  if (!changedProperties->contains(id))
    saveChangedProperties(w, id);

  return w;
}

/*!  Creates a layout on the widget \a widget of the type \a type
  which can be \c HBox, \c VBox or \c Grid.
*/

QLayout *WidgetFactory::createLayout( QWidget *widget, QLayout *layout, LayoutType type )
{
    int spacing = MainWindow::self->currentLayoutDefaultSpacing();
    int margin = 0;

    if ( widget && !widget->inherits( "QLayoutWidget" ) &&
	 ( WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( widget ) ) ) ||
	   widget && widget->parentWidget() && widget->parentWidget()->inherits( "FormWindow" ) ) )
	margin = MainWindow::self->currentLayoutDefaultMargin();

    if ( !layout && widget && widget->inherits( "QTabWidget" ) )
	widget = ((QTabWidget*)widget)->currentPage();

    if ( !layout && widget && widget->inherits( "QToolBox" ) )
    widget = ((QToolBox*)widget)->currentItem();

    if ( !layout && widget && widget->inherits( "QWizard" ) )
	widget = ((QWizard*)widget)->currentPage();

    if ( !layout && widget && widget->inherits( "QMainWindow" ) )
	widget = ((QMainWindow*)widget)->centralWidget();

    if ( !layout && widget && widget->inherits( "QWidgetStack" ) )
	widget = ((QWidgetStack*)widget)->visibleWidget();

    MetaDataBase::addEntry( widget );

    if ( !layout && widget && widget->inherits( "QGroupBox" ) ) {
	QGroupBox *gb = (QGroupBox*)widget;
	gb->setColumnLayout( 0, Qt::Vertical );
	gb->layout()->setMargin( 0 );
	gb->layout()->setSpacing( 0 );
	QLayout *l;
	switch ( type ) {
	case HBox:
	    l = new QHBoxLayout( gb->layout() );
	    MetaDataBase::setMargin( gb, margin );
	    MetaDataBase::setSpacing( gb, spacing );
	    l->setAlignment( AlignTop );
	    MetaDataBase::addEntry( l );
	    return l;
	case VBox:
	    l = new QVBoxLayout( gb->layout(), spacing );
	    MetaDataBase::setMargin( gb, margin );
	    MetaDataBase::setSpacing( gb, spacing );
	    l->setAlignment( AlignTop );
	    MetaDataBase::addEntry( l );
	    return l;
	case Grid:
	    l = new QDesignerGridLayout( gb->layout() );
	    MetaDataBase::setMargin( gb, margin );
	    MetaDataBase::setSpacing( gb, spacing );
	    l->setAlignment( AlignTop );
	    MetaDataBase::addEntry( l );
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
		MetaDataBase::addEntry( l );
		l->setSpacing( spacing );
		l->setMargin( margin );
		MetaDataBase::addEntry( l );
		return l;
	    case VBox:
		l = new QVBoxLayout( layout );
		MetaDataBase::addEntry( l );
		l->setSpacing( spacing );
		l->setMargin( margin );
		MetaDataBase::addEntry( l );
		return l;
	    case Grid: {
		l = new QDesignerGridLayout( layout );
		MetaDataBase::addEntry( l );
		l->setSpacing( spacing );
		l->setMargin( margin );
		MetaDataBase::addEntry( l );
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
		MetaDataBase::addEntry( l );
		if ( widget ) {
		    MetaDataBase::setMargin( widget, margin );
		    MetaDataBase::setSpacing( widget, spacing );
		} else {
		    l->setMargin( margin );
		    l->setSpacing( margin );
		}
		MetaDataBase::addEntry( l );
		return l;
	    case VBox:
		l = new QVBoxLayout( widget );
		MetaDataBase::addEntry( l );
		if ( widget ) {
		    MetaDataBase::setMargin( widget, margin );
		    MetaDataBase::setSpacing( widget, spacing );
		} else {
		    l->setMargin( margin );
		    l->setSpacing( margin );
		}
		MetaDataBase::addEntry( l );
		return l;
	    case Grid: {
		l = new QDesignerGridLayout( widget );
		MetaDataBase::addEntry( l );
		if ( widget ) {
		    MetaDataBase::setMargin( widget, margin );
		    MetaDataBase::setSpacing( widget, spacing );
		} else {
		    l->setMargin( margin );
		    l->setSpacing( margin );
		}
		MetaDataBase::addEntry( l );
		return l;
	    }
	    default:
		return 0;
	    }
	}
    }
}

void WidgetFactory::deleteLayout( QWidget *widget )
{
    if ( !widget )
	return;

    if ( widget->inherits( "QTabWidget" ) )
	widget = ((QTabWidget*)widget)->currentPage();
    if ( widget->inherits( "QWizard" ) )
	widget = ((QWizard*)widget)->currentPage();
    if ( widget->inherits( "QMainWindow" ) )
	widget = ((QMainWindow*)widget)->centralWidget();
    if ( widget->inherits( "QWidgetStack" ) )
	widget = ((QWidgetStack*)widget)->visibleWidget();
    delete widget->layout();
}

/*!  Factory functions for creating a widget of the type \a className
  as child of \a parent with the name \a name.

  If \a init is true, some initial default properties are set. This
  has to be in sync with the initChangedProperties() function!
*/

QWidget *WidgetFactory::createWidget( const QString &className, QWidget *parent, const char *name, bool init,
  const QRect *r, Qt::Orientation orient )
{
  if (className == "QPushButton")
  {
    QPushButton *b = 0;
    if (init)
    {
      b = new QDesignerPushButton(parent, name);
      b->setText(QString::fromLatin1(name));
    } else
    {
      b = new QDesignerPushButton(parent, name);
    }
    QWidget *w = find_formwindow(b);
    b->setAutoDefault(w && ((FormWindow *) w)->mainContainer()->inherits("QDialog"));
    return b;
  } else if (className == "QToolButton")
  {
    if (init)
    {
      QDesignerToolButton *tb = new QDesignerToolButton(parent, name);
      tb->setText("...");
      return tb;
    }
    return new QDesignerToolButton(parent, name);
  } else if (className == "QCheckBox")
  {
    if (init)
    {
      QDesignerCheckBox *cb = new QDesignerCheckBox(parent, name);
      cb->setText(QString::fromLatin1(name));
      return cb;
    }
    return new QDesignerCheckBox(parent, name);
  } else if (className == "QRadioButton")
  {
    if (init)
    {
      QDesignerRadioButton *rb = new QDesignerRadioButton(parent, name);
      rb->setText(QString::fromLatin1(name));
      return rb;
    }
    return new QDesignerRadioButton(parent, name);
  } else if (className == "QGroupBox")
  {
    if (init)
      return new QGroupBox(QString::fromLatin1(name), parent, name);
    return new QGroupBox(parent, name);
  } else if (className == "QButtonGroup")
  {
    if (init)
      return new QButtonGroup(QString::fromLatin1(name), parent, name);
    return new QButtonGroup(parent, name);
  } else if (className == "QIconView")
  {
#if !defined(QT_NO_ICONVIEW)
    QIconView *iv = new QIconView(parent, name);
    if (init)
      (void) new QIconViewItem(iv, i18n("New Item"));
    return iv;
#else
    return 0;
#endif
  } else if (className == "QTable")
  {
#if !defined(QT_NO_TABLE)
    if (init)
      return new QTable(3, 3, parent, name);
    return new QTable(parent, name);
#else
    return 0;
#endif
#ifndef QT_NO_SQL
  } else if (className == "QDataTable")
  {
    return new QDataTable(parent, name);
#endif //QT_NO_SQL
  } else if (className == "QDateEdit")
  {
    return new QDateEdit(parent, name);
  } else if (className == "QTimeEdit")
  {
    return new QTimeEdit(parent, name);
  } else if (className == "QDateTimeEdit")
  {
    return new QDateTimeEdit(parent, name);
  } else if (className == "QListBox")
  {
    QListBox *lb = new QListBox(parent, name);
    if (init)
    {
      lb->insertItem(i18n("New Item"));
      lb->setCurrentItem(0);
    }
    return lb;
  } else if (className == "QListView")
  {
    QListView *lv = new QListView(parent, name);
    lv->setSorting(-1);
    if (init)
    {
      lv->addColumn(i18n("Column 1"));
      lv->setCurrentItem(new QListViewItem(lv, i18n("New Item")));
    }
    return lv;
  } else if (className == "QLineEdit")
    return new QLineEdit(parent, name);
  else if (className == "QSpinBox")
    return new QSpinBox(parent, name);
  else if (className == "QSplitter")
    return new QSplitter(parent, name);
  else if (className == "QMultiLineEdit")
    return new QMultiLineEdit(parent, name);
  else if (className == "QTextEdit")
    return new QTextEdit(parent, name);
  else if (className == "QLabel")
  {
    QDesignerLabel *l = new QDesignerLabel(parent, name);
    if (init)
    {
      l->setText(QString::fromLatin1(name));
      MetaDataBase::addEntry(l);
      MetaDataBase::setPropertyChanged(l, "text", true);
    }
    return l;
  } else if (className == "QLayoutWidget")
    return new QLayoutWidget(parent, name);
  else if (className == "QTabWidget")
  {
    QTabWidget *tw = new QDesignerTabWidget(parent, name);
    if (init)
    {
      FormWindow *fw = find_formwindow(parent);
      QWidget *w = fw ? new QDesignerWidget(fw, tw, "tab") : new QWidget(tw, "tab");
      tw->addTab(w, i18n("Tab 1"));
      MetaDataBase::addEntry(w);
      w = fw ? new QDesignerWidget(fw, tw, "tab") : new QWidget(tw, "tab");
      tw->addTab(w, i18n("Tab 2"));
      MetaDataBase::addEntry(tw);
      MetaDataBase::addEntry(w);
    }
    return tw;
  } else if (className == "QComboBox")
  {
    return new QComboBox(false, parent, name);
  } else if (className == "QWidget")
  {
    if (parent &&
        (parent->inherits("FormWindow") || parent->inherits("QWizard")
            || parent->inherits("QTabWidget") || parent->inherits("QToolBox") || parent->inherits("QMainWindow")))
    {
      FormWindow *fw = find_formwindow(parent);
      if (fw)
      {
        QDesignerWidget *dw = new QDesignerWidget(fw, parent, name);
        MetaDataBase::addEntry(dw);
        return dw;
      }
    }
    return new QWidget(parent, name);
  } else if (className == "QDialog")
  {
    QDialog *dia = 0;
    if (parent && parent->inherits("FormWindow"))
      dia = new QDesignerDialog((FormWindow *) parent, parent, name);
    else
      dia = new QDialog(parent, name);
    if (parent && !parent->inherits("MainWindow"))
      dia->reparent(parent, QPoint(0, 0), true);
    return dia;
  } else if (className == "QWizard")
  {
    QWizard *wiz = new QDesignerWizard(parent, name);
    if (parent && !parent->inherits("MainWindow"))
    {
      wiz->reparent(parent, QPoint(0, 0), true);
    }
    if (init && parent && parent->inherits("FormWindow"))
    {
      QDesignerWidget *dw = new QDesignerWidget((FormWindow *) parent, wiz, "page");
      MetaDataBase::addEntry(dw);
      wiz->addPage(dw, i18n("Page"));
      QTimer::singleShot(0, wiz, SLOT(next()));
    }
    return wiz;
  } else if (className == "Spacer")
  {
    Spacer *s = new Spacer(parent, name);
    MetaDataBase::addEntry(s);
    MetaDataBase::setPropertyChanged(s, "orientation", true);
    MetaDataBase::setPropertyChanged(s, "sizeType", true);
    if (!r)
      return s;
    if (!r->isValid() || r->width() < 2 && r->height() < 2)
      s->setOrientation(orient);
    else if (r->width() < r->height())
      s->setOrientation(Qt::Vertical);
    else
      s->setOrientation(Qt::Horizontal);
    return s;
  } else if (className == "QLCDNumber")
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
  {
    QSlider *s = new QSlider(parent, name);
    if (!r)
      return s;
    if (!r->isValid() || r->width() < 2 && r->height() < 2)
      s->setOrientation(orient);
    else if (r->width() > r->height())
      s->setOrientation(Qt::Horizontal);
    MetaDataBase::addEntry(s);
    MetaDataBase::setPropertyChanged(s, "orientation", true);
    return s;
  } else if (className == "QScrollBar")
  {
    QScrollBar *s = new QScrollBar(parent, name);
    if (!r)
      return s;
    if (!r->isValid() || r->width() < 2 && r->height() < 2)
      s->setOrientation(orient);
    else if (r->width() > r->height())
      s->setOrientation(Qt::Horizontal);
    MetaDataBase::addEntry(s);
    MetaDataBase::setPropertyChanged(s, "orientation", true);
    return s;
  } else if (className == "QFrame")
  {
    if (!init)
      return new QFrame(parent, name);
    QFrame *f = new QFrame(parent, name);
    f->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    return f;
  } else if (className == "Line")
  {
    Line *l = new Line(parent, name);
    MetaDataBase::addEntry(l);
    MetaDataBase::setPropertyChanged(l, "orientation", true);
    MetaDataBase::setPropertyChanged(l, "frameShadow", true);
    MetaDataBase::setPropertyChanged(l, "frameShape", true);
    if (!r)
      return l;
    if (!r->isValid() || r->width() < 2 && r->height() < 2)
      l->setOrientation(orient);
    else if (r->width() < r->height())
      l->setOrientation(Qt::Vertical);
    return l;
  } else if (className == "QMainWindow")
  {
    QMainWindow *mw = new QMainWindow(parent, name, 0);
    mw->setDockEnabled(Qt::DockMinimized, false);
    QDesignerWidget *dw = new QDesignerWidget((FormWindow *) parent, mw, "central widget");
    mw->setDockMenuEnabled(false);
    MetaDataBase::addEntry(dw);
    mw->setCentralWidget(dw);
    (void) mw->statusBar();
    dw->show();
    return mw;
  }
#ifndef QT_NO_SQL
  else if (className == "QDataBrowser")
  {
    QWidget *w = new QDesignerDataBrowser(parent, name);
    if (parent)
      w->reparent(parent, QPoint(0, 0), true);
    return w;
  } else if (className == "QDataView")
  {
    QWidget *w = new QDesignerDataView(parent, name);
    if (parent)
      w->reparent(parent, QPoint(0, 0), true);
    return w;
  }
#endif
#ifdef KOMMANDER

  if (className == "LineEdit")
    return new LineEdit(parent, name);
  else if (className == "ListView")
  {
    QListView *lv = new QListView(parent, name);
    lv->setSorting(-1);
    if (init)
    {
      lv->addColumn(i18n("Column 1"));
      lv->setCurrentItem(new QListViewItem(lv, i18n("New Item")));
    }
    return lv;
  } 
  else if (className == "Dialog")
  {
    QDialog *dia = 0;
    if (parent && parent->inherits("FormWindow"))
      dia = new EditorDialog((FormWindow *) parent, parent, name);
    else
      dia = new Dialog(parent, name, false);

    if (parent)
      dia->reparent(parent, QPoint(0, 0), true);
    return dia;
  } 
  else if (className == "Wizard")
  {
    //qDebug("Creating Wizard...");
    QWizard *wiz;
    if (parent && parent->inherits("FormWindow"))
      wiz = new QDesignerWizard(parent, name);
    else
      wiz = new Wizard(parent, name);
    if (parent)
      wiz->reparent(parent, QPoint(0, 0), true);
    if (init && parent && parent->inherits("FormWindow"))
    {
      QDesignerWidget *dw = new QDesignerWidget((FormWindow *) parent, wiz, "page");
      MetaDataBase::addEntry(dw);
      wiz->addPage(dw, i18n("Page 1"));
      wiz->addPage(dw, i18n("Page 2"));
      QTimer::singleShot(0, wiz, SLOT(next()));
    }
    return wiz;
  } 
  else if (className == "TabWidget")
  {
    QTabWidget *tw = new EditorTabWidget(parent, name);
    if (init)
    {
      FormWindow *fw = find_formwindow(parent);
      QWidget *w = fw ? new QDesignerWidget(fw, tw, "tab") : new QWidget(tw, "tab");
      tw->addTab(w, i18n("Tab 1"));
      MetaDataBase::addEntry(w);
      w = fw ? new QDesignerWidget(fw, tw, "tab") : new QWidget(tw, "tab");
      tw->addTab(w, i18n("Tab 2"));
      MetaDataBase::addEntry(tw);
      MetaDataBase::addEntry(w);
    }
    return tw;
  } else if (className == "ToolBox")
  {
    QToolBox *tw = new EditorToolBox(parent, name);
    if (init)
    {
      FormWindow *fw = find_formwindow(parent);
      QWidget *w = fw ? new QDesignerWidget(fw, tw, "toolbox") : new QWidget(tw, "toolbox");
      tw->addItem(w, i18n("Page 1"));
      MetaDataBase::addEntry(w);
      w = fw ? new QDesignerWidget(fw, tw, "toolbox") : new QWidget(tw, "toolbox");
      tw->addItem(w, i18n("Page 2"));
      MetaDataBase::addEntry(tw);
      MetaDataBase::addEntry(w);
    }
    return tw;
  }
  else if (className == "ExecButton")
    return new ExecButton(parent, name);
  else if (className == "CloseButton")
    return new CloseButton(parent, name);
  else if (className == "SubDialog")
    return new SubDialog(parent, name);
  else if (className == "FileSelector")
    return new FileSelector(parent, name);
  else if (className == "TextEdit")
    return new TextEdit(parent, name);
  else if (className == "RadioButton")
    return new RadioButton(parent, name);
  else if (className == "ButtonGroup")
    return new ButtonGroup(parent, name);
  else if (className == "GroupBox")
    return new GroupBox(parent, name);
  else if (className == "CheckBox")
    return new CheckBox(parent, name);
  else if (className == "ComboBox")
    return new ComboBox(parent, name);
  else if (className == "SpinBoxInt")
    return new SpinBoxInt(parent, name);
  else if (className == "ListBox")
    return new ListBox(parent, name);
  else if (className == "ScriptObject")
    return new ScriptObject(parent, name);
  else if (className == "RichTextEditor")
    return new RichTextEditor(parent, name);
  else if (className == "DatePicker")
    return new DatePicker(parent, name);
  else if (className == "PopupMenu")
    return new PopupMenu(parent, name);
  else if (className == "ColorDialog")
    return new ColorDialog(parent, name);
  else if (className == "AboutDialog")
    return new AboutDialog(parent, name);
  else if (className == "TreeWidget")
  {
    QListView *lv = new TreeWidget(parent, name);
    lv->setSorting(-1);
    if (init)
    {
      lv->addColumn(i18n("Column 1"));
      lv->setCurrentItem(new QListViewItem(lv, i18n("New Item")));
    }
    return lv;
  } else if (className == "Slider")
  {
    Slider *s = new Slider(parent, name);
    if (!r)
      return s;
    if (!r->isValid() || r->width() < 2 && r->height() < 2)
      s->setOrientation(orient);
    else if (r->width() > r->height())
      s->setOrientation(Qt::Horizontal);
    MetaDataBase::addEntry(s);
    MetaDataBase::setPropertyChanged(s, "orientation", true);
    return s;
  }
#endif

  QWidget *w = KommanderFactory::createWidget(className, parent, name);
  return w;
}



/*!  Find out which type the layout of the widget is. Returns \c HBox,
  \c VBox, \c Grid or \c NoLayout.  \a layout points to this
  QWidget::layout() of \a w or to 0 after the function call.
*/

WidgetFactory::LayoutType WidgetFactory::layoutType( QWidget *w, QLayout *&layout )
{
    layout = 0;

    if ( w && w->inherits( "QTabWidget" ) )
	w = ((QTabWidget*)w)->currentPage();
    if ( w->inherits( "QToolBox" ) )
    w = ((QToolBox*)w)->currentItem();
    if ( w && w->inherits( "QWizard" ) )
	w = ((QWizard*)w)->currentPage();
    if ( w && w->inherits( "QMainWindow" ) )
	w = ((QMainWindow*)w)->centralWidget();
    if ( w && w->inherits( "QWidgetStack" ) )
	w = ((QWidgetStack*)w)->visibleWidget();

    if ( w && w->inherits( "QSplitter" ) )
	return ( (QSplitter*)w )->orientation() == Horizontal ? HBox : VBox;

    if ( !w || !w->layout() )
	return NoLayout;
    QLayout *lay = w->layout();

    if ( w->inherits( "QGroupBox" ) ) {
	QObjectList *l = lay->queryList( "QLayout" );
	if ( l && l->first() )
	    lay = (QLayout*)l->first();
	delete l;
    }
    layout = lay;

    if ( lay->inherits( "QHBoxLayout" ) )
	return HBox;
    else if ( lay->inherits( "QVBoxLayout" ) )
	return VBox;
    else if ( lay->inherits( "QGridLayout" ) )
	return Grid;
    return NoLayout;
}

/*!
  \overload
*/
WidgetFactory::LayoutType WidgetFactory::layoutType( QLayout *layout )
{
    if ( layout->inherits( "QHBoxLayout" ) )
	return HBox;
    else if ( layout->inherits( "QVBoxLayout" ) )
	return VBox;
    else if ( layout->inherits( "QGridLayout" ) )
	return Grid;
    return NoLayout;
}

/*!
  \overload
*/
WidgetFactory::LayoutType WidgetFactory::layoutType( QWidget *w )
{
    QLayout *l = 0;
    return layoutType( w, l );
}


QWidget *WidgetFactory::layoutParent( QLayout *layout )
{
    QObject *o = layout;
    while ( o ) {
	if ( o->isWidgetType() )
	    return (QWidget*)o;
	o = o->parent();
    }
    return 0;
}

/*!  Returns the widget into which children should be inserted when \a
  w is a container known to the designer.

  Usually that is \a w itself, sometimes it is different (e.g. a
  tabwidget is known to the designer as a container but the child
  widgets should be inserted into the current page of the
  tabwidget. So in this case this function returns the current page of
  the tabwidget.)
 */
QWidget* WidgetFactory::containerOfWidget( QWidget *w )
{
    if ( !w )
	return w;
    if ( w->inherits( "QTabWidget" ) )
	return ((QTabWidget*)w)->currentPage();
    if ( w->inherits( "QToolBox" ) )
    return ((QToolBox*)w)->currentItem();
    if ( w->inherits( "QWizard" ) )
	return ((QWizard*)w)->currentPage();
    if ( w->inherits( "QWidgetStack" ) )
	return ((QWidgetStack*)w)->visibleWidget();
    if ( w->inherits( "QMainWindow" ) )
	return ((QMainWindow*)w)->centralWidget();
    return w;
}

/*!  Returns the actual designer widget of the container \a w. This is
  normally \a w itself, but might be a parent or grand parent of \a w
  (e.g. when working with a tabwidget and \a w is the container which
  contains and layouts childs, but the actual widget known to the
  designer is the tabwidget which is the parent of \a w. So this
  function returns the tabwidget then.)
*/

QWidget* WidgetFactory::widgetOfContainer( QWidget *w )
{
    if ( w->parentWidget() && w->parentWidget()->inherits( "QWidgetStack" ) )
	w = w->parentWidget();
    while ( w ) {
	if ( WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) ||
	     w && w->parentWidget() && w->parentWidget()->inherits( "FormWindow" ) )
	    return w;
	w = w->parentWidget();
    }
    return w;
}

/*!
  Returns whether \a o is a passive interactor or not.
 */
bool WidgetFactory::isPassiveInteractor( QObject* o )
{
    if ( QApplication::activePopupWidget() ) // if a popup is open, we have to make sure that this one is closed, else X might do funny things
	return true;

    if ( o->inherits( "QTabBar" ) || ::qt_cast<QToolBox*>(o->parent()) )
	return true;
    else if ( o->inherits( "QSizeGrip" ) )
	return true;
    else if ( o->inherits( "QToolButton" ) && o->parent() && o->parent()->inherits( "QTabBar" ) )
	return true;
    else if ( o->parent() && o->parent()->inherits( "QWizard" ) && o->inherits( "QPushButton" ) )
	return true;
    else if ( o->parent() && o->parent()->inherits( "QMainWindow" ) && o->inherits( "QMenuBar" ) )
	return true;
    else if ( o->inherits( "QDockWindowHandle" ) )
	return true;
    else if ( o->inherits( "QHideDock" ) )
	return true;

    return false;
}


/*!
  Returns the class name of object \a o that should be used for externally (i.e. for saving)
 */
const char* WidgetFactory::classNameOf( QObject* o )
{
  if (o->inherits("QDesignerTabWidget"))
    return "QTabWidget";
#ifdef KOMMANDER
  else if (o->inherits("EditorTabWidget"))
    return "TabWidget";
  else if (o->inherits("EditorToolBox"))
    return "ToolBox";
#endif
  else if (o->inherits("QDesignerDialog"))
    return "QDialog";
  else if (o->inherits("QDesignerWidget"))
    return "QWidget";
  else if (o->inherits("CustomWidget"))
    return ((CustomWidget *) o)->realClassName().latin1();
  else if (o->inherits("QDesignerLabel"))
    return "QLabel";
  else if (o->inherits("QDesignerWizard"))
    return "QWizard";
  else if (o->inherits("EditorWizard"))
    return "Wizard";
  else if (o->inherits("QDesignerPushButton"))
    return "QPushButton";
  else if (o->inherits("QDesignerToolButton"))
    return "QToolButton";
  else if (o->inherits("QDesignerRadioButton"))
    return "QRadioButton";
  else if (o->inherits("QDesignerCheckBox"))
    return "QCheckBox";
  else if (o->inherits("QDesignerMenuBar"))
    return "QMenuBar";
  else if (o->inherits("QDesignerToolBar"))
    return "QToolBar";
#ifndef QT_NO_SQL
  else if (o->inherits("QDesignerDataBrowser"))
    return "QDataBrowser";
  else if (o->inherits("QDesignerDataView"))
    return "QDataView";
#endif
  else if (o->inherits("EditorDialog"))
    return "Dialog";
  return o->className();
}

/*!  As some properties are set by default when creating a widget this
  functions markes this properties as changed. Has to be in sync with
  createWidget()!
*/

void WidgetFactory::initChangedProperties( QObject *o )
{
    MetaDataBase::setPropertyChanged( o, "name", true );
    if ( !o->inherits( "QDesignerToolBar" ) && !o->inherits( "QDesignerMenuBar" ) )
	MetaDataBase::setPropertyChanged( o, "geometry", true );

    if ( o->inherits( "QPushButton" ) || o->inherits("QRadioButton") || o->inherits( "QCheckBox" ) || o->inherits( "QToolButton" ) )
	MetaDataBase::setPropertyChanged( o, "text", true );
    else if (::qt_cast<QToolButton*>(o) && ::qt_cast<QToolBox*>(widgetOfContainer((QWidget*)o->parent()))) {
        MetaDataBase::setPropertyChanged( o, "usesTextLabel", TRUE );
        MetaDataBase::setPropertyChanged( o, "textLabel", TRUE );
        MetaDataBase::setPropertyChanged( o, "autoRaise", TRUE );
        MetaDataBase::setPropertyChanged( o, "textPosition", TRUE );
    }    else if ( o->inherits( "QGroupBox" ) )
	MetaDataBase::setPropertyChanged( o, "title", true );
    else if ( o->isA( "QFrame" ) ) {
	MetaDataBase::setPropertyChanged( o, "frameShadow", true );
	MetaDataBase::setPropertyChanged( o, "frameShape", true );
    } else if ( o->inherits( "QTabWidget" ) || o->inherits( "QWizard" ) ) {
	MetaDataBase::setPropertyChanged( o, "pageTitle", true );
	MetaDataBase::setPropertyChanged( o, "pageName", true );
#ifndef QT_NO_TABLE
    } else if ( o->inherits( "QTable" ) && !o->inherits( "QDataTable" ) ) {
	MetaDataBase::setPropertyChanged( o, "numRows", true );
	MetaDataBase::setPropertyChanged( o, "numCols", true );
	QTable *t = (QTable*)o;
	for ( int i = 0; i < 3; ++i ) {
	    t->horizontalHeader()->setLabel( i, QString::number( i + 1 ) );
	    t->verticalHeader()->setLabel( i, QString::number( i + 1 ) );
	}
#endif
    } else if ( ::qt_cast<QToolBox*>(o) ) {
        MetaDataBase::setPropertyChanged( o, "currentIndex", true );
        MetaDataBase::setPropertyChanged( o, "itemName", true );
        MetaDataBase::setPropertyChanged( o, "itemLabel", true );
        MetaDataBase::setPropertyChanged( o, "itemIconSet", true );
        MetaDataBase::setPropertyChanged( o, "itemToolTip", true );
        MetaDataBase::setPropertyChanged( o, "itemBackgroundMode", true );
    } else if ( o->inherits( "QSplitter" )  ) {
	MetaDataBase::setPropertyChanged( o, "orientation", true );
    } else if ( o->inherits( "QDesignerToolBar" )  ) {
	MetaDataBase::setPropertyChanged( o, "label", true );
    } else if ( o->inherits( "QDesignerMenuBar" )  ) {
	MetaDataBase::setPropertyChanged( o, "itemName", true );
	MetaDataBase::setPropertyChanged( o, "itemNumber", true );
	MetaDataBase::setPropertyChanged( o, "itemText", true );
    }
}

bool WidgetFactory::hasSpecialEditor( int id )
{
  QString className = WidgetDatabase::className(id);

  if (className == "TextEdit" || className == "ComboBox" || className == "ListBox" || className ==
      "TreeWidget" || className == "TextBrowser")
    return true;

  if (className.mid(1) == "ListBox")
    return true;
  if (className.mid(1) == "ComboBox")
    return true;
  if (className.mid(1) == "ListView")
    return true;
  if (className.mid(1) == "IconView")
    return true;
  if (className == "QTextEdit" || className == "QMultiLineEdit")
    return true;
  if (className.contains("Table"))
    return true;

  return false;
}

bool WidgetFactory::hasItems( int id )
{
  QString className = WidgetDatabase::className(id);

  if (className == "ComboBox" || className == "ListBox" || className == "TreeWidget")
    return true;
  if (className.mid(1) == "ListBox" || className.mid(1) == "ListView" ||
      className.mid(1) == "IconView" || className.mid(1) == "ComboBox" ||
      className.contains("Table"))
    return true;

  return false;
}

void WidgetFactory::editWidget( int id, QWidget *parent, QWidget *editWidget, FormWindow *fw )
{
  QString className = WidgetDatabase::className(id);

#ifdef KOMMANDER
  if (className == "ComboBox")
  {
    if (!editWidget->inherits("QComboBox"))
      return;

    QComboBox *cb = (QComboBox *) editWidget;

    ListBoxEditor *e = new ListBoxEditor(parent, cb->listBox(), fw);
    e->exec();
    delete e;

    cb->update();

    return;
  }
  if (className == "TextEdit" || className == "TextBrowser")
  {
    MultiLineEditor *e = new MultiLineEditor(parent, editWidget, fw);
    e->exec();
    delete e;
    return;
  }
  if (className == "TreeWidget")
  {
    if (!editWidget->inherits("QListView"))
      return;
    QListView *lv = (QListView *) editWidget;
    ListViewEditor *e = new ListViewEditor(parent, lv, fw);
    e->exec();
    delete e;
    return;
  }
  if (className == "ListBox")
  {
    if (!editWidget->inherits("QListBox"))
      return;
    ListBoxEditor *e = new ListBoxEditor(parent, editWidget, fw);
    e->exec();
    delete e;
    return;
  }
#endif
  if (className.mid(1) == "ListBox")
  {
    if (!editWidget->inherits("QListBox"))
      return;
    ListBoxEditor *e = new ListBoxEditor(parent, editWidget, fw);
    e->exec();
    delete e;
    return;
  }

  if (className.mid(1) == "ComboBox")
  {
    if (!editWidget->inherits("QComboBox"))
      return;
    QComboBox *cb = (QComboBox *) editWidget;
    ListBoxEditor *e = new ListBoxEditor(parent, cb->listBox(), fw);
    e->exec();
    delete e;
    cb->update();
    return;
  }

  if (className.mid(1) == "ListView")
  {
    if (!editWidget->inherits("QListView"))
      return;
    QListView *lv = (QListView *) editWidget;
    ListViewEditor *e = new ListViewEditor(parent, lv, fw);
    e->exec();
    delete e;
    return;
  }

  if (className.mid(1) == "IconView")
  {
    if (!editWidget->inherits("QIconView"))
      return;
    IconViewEditor *e = new IconViewEditor(parent, editWidget, fw);
    e->exec();
    delete e;
    return;
  }

  if (className == "QMultiLineEdit" || className == "QTextEdit")
  {
    MultiLineEditor *e = new MultiLineEditor(parent, editWidget, fw);
    e->exec();
    delete e;
    return;
  }
#ifndef QT_NO_TABLE
  if (className.contains("Table"))
  {
    TableEditor *e = new TableEditor(parent, editWidget, fw);
    e->exec();
    delete e;
    return;
  }
#endif
}

bool WidgetFactory::canResetProperty( QObject *w, const QString &propName )
{
    if ( propName == "name" || propName == "geometry" )
	return false;
    QStringList l = *changedProperties->find( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) );
    return l.findIndex( propName ) == -1;
}

bool WidgetFactory::resetProperty( QObject *w, const QString &propName )
{
    const QMetaProperty *p = w->metaObject()->property( w->metaObject()->
							findProperty( propName, true ), true );
    if (!p )
	return false;
    return p->reset( w );
}

QVariant WidgetFactory::defaultValue( QObject *w, const QString &propName )
{
    if ( propName == "wordwrap" ) {
	int v = defaultValue( w, "alignment" ).toInt();
	return QVariant( ( v & WordBreak ) == WordBreak, 0 );
    } else if ( propName == "toolTip" || propName == "whatsThis" ) {
	return QVariant( QString::fromLatin1( "" ) );
    } else if ( w->inherits( "CustomWidget" ) ) {
	return QVariant();
    } else if ( propName == "frameworkCode" ) {
	return QVariant( true, 0 );
    } else if ( propName == "layoutMargin" ) {
	if ( w->inherits( "QLayoutWidget" ) )
	    return QVariant( 0 );
	else if ( MainWindow::self->formWindow() )
	    return QVariant( MainWindow::self->formWindow()->layoutDefaultMargin() );
    } else if ( propName == "layoutSpacing" ) {
	if ( MainWindow::self->formWindow() )
	    return QVariant( MainWindow::self->formWindow()->layoutDefaultSpacing() );
    }

    return *( *defaultProperties->find( WidgetDatabase::idFromClassName( classNameOf( w ) ) ) ).find( propName );
}

QString WidgetFactory::defaultCurrentItem( QObject *w, const QString &propName )
{
    const QMetaProperty *p = w->metaObject()->
			     property( w->metaObject()->findProperty( propName, true ), true );
    if ( !p ) {
	int v = defaultValue( w, "alignment" ).toInt();
	if ( propName == "hAlign" ) {
	    if ( ( v & AlignAuto ) == AlignAuto )
		return "AlignAuto";
	    if ( ( v & AlignLeft ) == AlignLeft )
		return "AlignLeft";
	    if ( ( v & AlignCenter ) == AlignCenter || ( v & AlignHCenter ) == AlignHCenter )
		return "AlignHCenter";
	    if ( ( v & AlignRight ) == AlignRight )
		return "AlignRight";
	    if ( ( v & AlignJustify ) == AlignJustify )
		return "AlignJustify";
	} else if ( propName == "vAlign" ) {
	    if ( ( v & AlignTop ) == AlignTop )
		return "AlignTop";
	    if ( ( v & AlignCenter ) == AlignCenter || ( v & AlignVCenter ) == AlignVCenter )
		return "AlignVCenter";
	    if ( ( v & AlignBottom ) == AlignBottom )
		return "AlignBottom";
	}
	return QString::null;

    }
    return p->valueToKey( defaultValue( w, propName ).toInt() );
}

QWidget *WidgetFactory::createCustomWidget( QWidget *parent, const char *name, MetaDataBase::CustomWidget *w )
{
    if ( !w )
	return 0;
    return new CustomWidget( parent, name, w );
}

QVariant WidgetFactory::property( QObject *w, const char *name )
{
    QVariant v = w->property( name );
    if ( v.isValid() )
	return v;
    return MetaDataBase::fakeProperty( w, name );
}

void QDesignerLabel::updateBuddy()
{

    if ( myBuddy.isEmpty() )
	return;

    QObjectList *l = topLevelWidget()->queryList( "QWidget", myBuddy, false, true );
    if ( !l || !l->first() ) {
	delete l;
	return;
    }

    QLabel::setBuddy( (QWidget*)l->first() );
    delete l;
}

void QDesignerWidget::paintEvent( QPaintEvent *e )
{
    formwindow->paintGrid( this, e );
}

void QDesignerDialog::paintEvent( QPaintEvent *e )
{
    formwindow->paintGrid( this, e );
}

void EditorDialog::paintEvent( QPaintEvent *e )
{
    formwindow->paintGrid( this, e );
}

QSizePolicy QLayoutWidget::sizePolicy() const
{
    return sp;
}

bool QLayoutWidget::event( QEvent *e )
{
    if ( e && ( e->type() == QEvent::ChildInserted ||
		e->type() == QEvent::ChildRemoved ||
		e->type() == QEvent::LayoutHint ||
		e->type() == QEvent::Reparent ) )
	updateSizePolicy();
    return QWidget::event( e );
}

/*
  This function must be called on QLayoutWidget creation and whenever
  the QLayoutWidget's parent layout changes (e.g., from a QHBoxLayout
  to a QVBoxLayout), because of the (illogical) way layouting works.
*/
void QLayoutWidget::updateSizePolicy()
{
    if ( !children() || children()->count() == 0 ) {
	sp = QWidget::sizePolicy();
	return;
    }

    /*
      QSizePolicy::MayShrink & friends are private. Here we assume the
      following:

	  Fixed = 0
	  Maximum = MayShrink
	  Minimum = MayGrow
	  Preferred = MayShrink | MayGrow
    */

    int ht = (int) QSizePolicy::Preferred;
    int vt = (int) QSizePolicy::Preferred;

    if ( layout() ) {
	/*
	  parentLayout is set to the parent layout if there is one and if it is
	  top level, in which case layouting is illogical.
	*/
	QLayout *parentLayout = 0;
	if ( parent() && parent()->isWidgetType() ) {
	    parentLayout = ((QWidget *)parent())->layout();
	    if ( parentLayout && parentLayout->mainWidget()->inherits("QLayoutWidget") )
		parentLayout = 0;
	}

	QObjectListIt it( *children() );
	QObject *o;

	if ( layout()->inherits("QVBoxLayout") ) {
	    if ( parentLayout && parentLayout->inherits("QHBoxLayout") )
		vt = QSizePolicy::Minimum;
	    else
		vt = QSizePolicy::Fixed;

	    while ( ( o = it.current() ) ) {
		++it;
		if ( !o->isWidgetType() || ( (QWidget*)o )->testWState( WState_ForceHide ) )
		    continue;
		QWidget *w = (QWidget*)o;

		if ( !w->sizePolicy().mayGrowHorizontally() )
		    ht &= ~QSizePolicy::Minimum;
		if ( !w->sizePolicy().mayShrinkHorizontally() )
		    ht &= ~QSizePolicy::Maximum;
		if ( w->sizePolicy().mayGrowVertically() )
		    vt |= QSizePolicy::Minimum;
		if ( w->sizePolicy().mayShrinkVertically() )
		    vt |= QSizePolicy::Maximum;
	    }
	} else if ( layout()->inherits("QHBoxLayout") ) {
	    if ( parentLayout && parentLayout->inherits("QVBoxLayout") )
		ht = QSizePolicy::Minimum;
	    else
		ht = QSizePolicy::Fixed;

	    while ( ( o = it.current() ) ) {
		++it;
		if ( !o->isWidgetType() || ( (QWidget*)o )->testWState( WState_ForceHide ) )
		    continue;
		QWidget *w = (QWidget*)o;

		if ( w->sizePolicy().mayGrowHorizontally() )
		    ht |= QSizePolicy::Minimum;
		if ( w->sizePolicy().mayShrinkHorizontally() )
		    ht |= QSizePolicy::Maximum;
		if ( !w->sizePolicy().mayGrowVertically() )
		    vt &= ~QSizePolicy::Minimum;
		if ( !w->sizePolicy().mayShrinkVertically() )
		    vt &= ~QSizePolicy::Maximum;
	    }
	} else if ( layout()->inherits("QGridLayout") ) {
	    ht = QSizePolicy::Fixed;
	    vt = QSizePolicy::Fixed;
	    if ( parentLayout ) {
		if ( parentLayout->inherits("QVBoxLayout") )
		    ht = QSizePolicy::Minimum;
		else if ( parentLayout->inherits("QHBoxLayout") )
		    vt = QSizePolicy::Minimum;
	    }

	    while ( ( o = it.current() ) ) {
		++it;
		if ( !o->isWidgetType() || ( (QWidget*)o )->testWState( WState_ForceHide ) )
		    continue;
		QWidget *w = (QWidget*)o;

		if ( w->sizePolicy().mayGrowHorizontally() )
		    ht |= QSizePolicy::Minimum;
		if ( w->sizePolicy().mayShrinkHorizontally() )
		    ht |= QSizePolicy::Maximum;
		if ( w->sizePolicy().mayGrowVertically() )
		    vt |= QSizePolicy::Minimum;
		if ( w->sizePolicy().mayShrinkVertically() )
		    vt |= QSizePolicy::Maximum;
	    }
	}
	if ( layout()->expanding() & QSizePolicy::Horizontally )
	    ht = QSizePolicy::Expanding;
	if ( layout()->expanding() & QSizePolicy::Vertically )
	    vt = QSizePolicy::Expanding;

	layout()->invalidate();
    }

    sp = QSizePolicy( (QSizePolicy::SizeType) ht, (QSizePolicy::SizeType) vt );
    updateGeometry();
}

void CustomWidget::paintEvent( QPaintEvent *e )
{
    if ( parentWidget() && parentWidget()->inherits( "FormWindow" ) ) {
	( (FormWindow*)parentWidget() )->paintGrid( this, e );
    } else {
	QPainter p( this );
	p.fillRect( rect(), colorGroup().dark() );
	p.drawPixmap( ( width() - cusw->pixmap->width() ) / 2,
		      ( height() - cusw->pixmap->height() ) / 2,
		      *cusw->pixmap );
    }
}


#include "widgetfactory.moc"
