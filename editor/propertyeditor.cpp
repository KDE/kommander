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

#include <qvariant.h> // HP-UX compiler needs this here
//Added by qt3to4:
#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QCloseEvent>
#include <Q3ValueList>
#include <QDragEnterEvent>
#include <QKeyEvent>
#include <Q3Frame>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <Q3CString>
#include <Q3PopupMenu>

#include "propertyeditor.h"
#include "pixmapchooser.h"
#include "formwindow.h"
#include "command.h"
#include "metadatabase.h"
#include "widgetdatabase.h"
#include "widgetfactory.h"
#include "globaldefs.h"
#include "defs.h"
#include "asciivalidator.h"
#include "paletteeditorimpl.h"
#include "multilineeditorimpl.h"
#include "mainwindow.h"
#ifndef KOMMANDER
#include "project.h"
#endif
#include "hierarchyview.h"

#include <limits.h>

#include <qpainter.h>
#include <qpalette.h>
#include <qapplication.h>
#include <q3header.h>
#include <qlineedit.h>
#include <q3strlist.h>
#include <qmetaobject.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <q3hbox.h>
#include <qfontdialog.h>
#include <qspinbox.h>
#include <qevent.h>
#include <qobject.h>
#include <q3listbox.h>
#include <qfontdatabase.h>
#include <qcolor.h>
#include <kcolordialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qsizepolicy.h>
#include <qbitmap.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <q3accel.h>
#include <qworkspace.h>
#include <qtimer.h>
#include <q3dragobject.h>
#include <qdom.h>
#include <q3process.h>
#include <qstyle.h>
#include <q3datetimeedit.h>

#include "pics/arrow.xbm"
#include "pics/uparrow.xbm"
#include "pics/cross.xbm"
#include "pics/wait.xbm"
#include "pics/ibeam.xbm"
#include "pics/sizeh.xbm"
#include "pics/sizev.xbm"
#include "pics/sizeb.xbm"
#include "pics/sizef.xbm"
#include "pics/sizeall.xbm"
#include "pics/vsplit.xbm"
#include "pics/hsplit.xbm"
#include "pics/hand.xbm"
#include "pics/no.xbm"

#include <klocale.h>

static QFontDatabase *fontDataBase = 0;
QString assistantPath();

static void cleanupFontDatabase()
{
    delete fontDataBase;
    fontDataBase = 0;
}

static QStringList getFontList()
{
    if ( !fontDataBase ) {
	fontDataBase = new QFontDatabase;
	qAddPostRoutine( cleanupFontDatabase );
    }
    return fontDataBase->families();
}


class PropertyWhatsThis : public Q3WhatsThis
{
public:
    PropertyWhatsThis( PropertyList *l );
    QString text( const QPoint &pos );
    bool clicked( const QString& href );

private:
    PropertyList *propertyList;

};

PropertyWhatsThis::PropertyWhatsThis( PropertyList *l )
    : Q3WhatsThis( l->viewport() ), propertyList( l )
{
}

QString PropertyWhatsThis::text( const QPoint &pos )
{
    return propertyList->whatsThisAt( pos );
}

bool PropertyWhatsThis::clicked( const QString& href )
{
    if ( !href.isEmpty() ) {
	QStringList lst;
	lst << assistantPath() << (QString( "d:" ) + QString( href ));
	Q3Process proc( lst );
	proc.start();
    }
    return false; // do not hide window
}


/*!
  \class PropertyItem propertyeditor.h
  \brief Base class for all property items

  This is the base class for each property item for the
  PropertyList. A simple property item has just a name and a value to
  provide an editor for a datatype. But more complex datatypes might
  provide an expandable item for editing single parts of the
  datatype. See hasSubItems(), initChildren() for that.
*/

/*!  If this item should be a child of another property item, specify
  \a prop as the parent item.
*/

PropertyItem::PropertyItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName )
    : Q3ListViewItem( l, after ), listview( l ), property( prop ), propertyName( propName )
{
    setSelectable( false );
    open = false;
    setText( 0, propertyName );
    changed = false;
    setText( 1, "" );
    resetButton = 0;
}

PropertyItem::~PropertyItem()
{
    if ( resetButton )
	delete resetButton->parentWidget();
    resetButton = 0;
}

void PropertyItem::toggle()
{
}

void PropertyItem::updateBackColor()
{
    if ( itemAbove() && this != listview->firstChild() ) {
	if ( ( ( PropertyItem*)itemAbove() )->backColor == *backColor1 )
	    backColor = *backColor2;
	else
	    backColor = *backColor1;
    } else {
	backColor = *backColor1;
    }
    if ( listview->firstChild() == this )
	backColor = *backColor1;
}

QColor PropertyItem::backgroundColor()
{
    updateBackColor();
    if ( (Q3ListViewItem*)this == listview->currentItem() )
	return *selectedBack;
    return backColor;
}

/*!  If a subclass is a expandable item, this is called when the child
items should be created.
*/

void PropertyItem::createChildren()
{
}

/*!  If a subclass is a expandable item, this is called when the child
items should be initialized.
*/

void PropertyItem::initChildren()
{
}

void PropertyItem::paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align )
{
    QColorGroup g( cg );
    g.setColor( QPalette::Base, backgroundColor() );
    g.setColor( QPalette::Foreground, Qt::black );
    g.setColor( QPalette::Text, Qt::black );
    int indent = 0;
    if ( column == 0 ) {
	indent = 20 + ( property ? 20 : 0 );
	p->fillRect( 0, 0, width, height(), backgroundColor() );
	p->save();
	p->translate( indent, 0 );
    }

    if ( isChanged() && column == 0 ) {
	p->save();
	QFont f = p->font();
	f.setBold( true );
	p->setFont( f );
    }

    if ( !hasCustomContents() || column != 1 ) {
	Q3ListViewItem::paintCell( p, g, column, width - indent, align  );
    } else {
	p->fillRect( 0, 0, width, height(), backgroundColor() );
	drawCustomContents( p, QRect( 0, 0, width, height() ) );
    }

    if ( isChanged() && column == 0 )
	p->restore();
    if ( column == 0 )
	p->restore();
    if ( hasSubItems() && column == 0 ) {
	p->save();
	p->setPen( cg.foreground() );
	p->setBrush( cg.base() );
	p->drawRect( 5, height() / 2 - 4, 9, 9 );
	p->drawLine( 7, height() / 2, 11, height() / 2 );
	if ( !isOpen() )
	    p->drawLine( 9, height() / 2 - 2, 9, height() / 2 + 2 );
	p->restore();
    }
    p->save();
    p->setPen( QPen( cg.dark(), 1 ) );
    p->drawLine( 0, height() - 1, width, height() - 1 );
    p->drawLine( width - 1, 0, width - 1, height() );
    p->restore();

    if ( listview->currentItem() == this && column == 0 &&
	 !listview->hasFocus() && !listview->viewport()->hasFocus() )
	paintFocus( p, cg, QRect( 0, 0, width, height() ) );
}

void PropertyItem::paintBranches( QPainter * p, const QColorGroup & cg,
				  int w, int y, int h )
{
    QColorGroup g( cg );
    g.setColor( QPalette::Base, backgroundColor() );
    Q3ListViewItem::paintBranches( p, g, w, y, h );
}

void PropertyItem::paintFocus( QPainter *p, const QColorGroup &cg, const QRect &r )
{
    p->save();
    QApplication::style().drawPrimitive(QStyle::PE_Panel, p, r, cg,
					QStyle::Style_Sunken, QStyleOption(1,1) );
    p->restore();
}

/*!  Subclasses which are expandable items have to return true
  here. Default is false.
*/

bool PropertyItem::hasSubItems() const
{
    return false;
}

/*!  Returns the parent property item here if this is a child or 0
 otherwise.
 */

PropertyItem *PropertyItem::propertyParent() const
{
    return property;
}

bool PropertyItem::isOpen() const
{
    return open;
}

void PropertyItem::setOpen( bool b )
{
    if ( b == open )
	return;
    open = b;

    if ( !open ) {
	children.setAutoDelete( true );
	children.clear();
	children.setAutoDelete( false );
	qApp->processEvents();
	listview->updateEditorSize();
	return;
    }

    createChildren();
    initChildren();
    qApp->processEvents();
    listview->updateEditorSize();
}

/*!  Subclasses have to show the editor of the item here
*/

void PropertyItem::showEditor()
{
    createResetButton();
    resetButton->parentWidget()->show();
}

/*!  Subclasses have to hide the editor of the item here
*/

void PropertyItem::hideEditor()
{
    createResetButton();
    resetButton->parentWidget()->hide();
}

/*!  This is called to init the value of the item. Reimplement in
  subclasses to init the editor
*/

void PropertyItem::setValue( const QVariant &v )
{
    val = v;
}

QVariant PropertyItem::value() const
{
    return val;
}

bool PropertyItem::isChanged() const
{
    return changed;
}

void PropertyItem::setChanged( bool b, bool updateDb )
{
    if ( propertyParent() )
	return;
    if ( changed == b )
	return;
    changed = b;
    repaint();
    if ( updateDb )
	MetaDataBase::setPropertyChanged( listview->propertyEditor()->widget(), name(), changed );
    updateResetButtonState();
}

QString PropertyItem::name() const
{
    return propertyName;
}

void PropertyItem::createResetButton()
{
    if ( resetButton ) {
	resetButton->parentWidget()->lower();
	return;
    }
    Q3HBox *hbox = new Q3HBox( listview->viewport() );
    hbox->setFrameStyle( Q3Frame::StyledPanel | Q3Frame::Sunken );
    hbox->setLineWidth( 1 );
    resetButton = new QPushButton( hbox );
    resetButton->setPixmap( PixmapChooser::loadPixmap( "resetproperty.xpm", PixmapChooser::Mini ) );
    resetButton->setFixedWidth( resetButton->sizeHint().width() );
    hbox->layout()->setAlignment( Qt::AlignRight );
    listview->addChild( hbox );
    hbox->hide();
    QObject::connect( resetButton, SIGNAL( clicked() ),
		      listview, SLOT( resetProperty() ) );
    QToolTip::add( resetButton, i18n("Reset the property to its default value" ) );
    Q3WhatsThis::add( resetButton, i18n("Click this button to reset the property to its default value" ) );
    updateResetButtonState();
}

void PropertyItem::updateResetButtonState()
{
    if ( !resetButton )
	return;
    if ( propertyParent() || !WidgetFactory::canResetProperty( listview->propertyEditor()->widget(), name() ) )
	resetButton->setEnabled( false );
    else
	resetButton->setEnabled( isChanged() );
}

/*!  Call this to place/resize the item editor correctly (normally
  call it from showEditor())
*/

void PropertyItem::placeEditor( QWidget *w )
{
    createResetButton();
    QRect r = listview->itemRect( this );
    if ( !r.size().isValid() ) {
	listview->ensureItemVisible( this );
#if defined(Q_WS_WIN)
	listview->repaintContents( false );
#endif
	r = listview->itemRect( this );
    }
    r.setX( listview->header()->sectionPos( 1 ) );
    r.setWidth( listview->header()->sectionSize( 1 ) - 1 );
    r.setWidth( r.width() - resetButton->width() - 2 );
    r = QRect( listview->viewportToContents( r.topLeft() ), r.size() );
    w->resize( r.size() );
    listview->moveChild( w, r.x(), r.y() );
    resetButton->parentWidget()->resize( resetButton->sizeHint().width() + 10, r.height() );
    listview->moveChild( resetButton->parentWidget(), r.x() + r.width() - 8, r.y() );
    resetButton->setFixedHeight( r.height() - 3 );
}

/*!  This should be called by subclasses if the use changed the value
  of the property and this value should be applied to the widget property
*/

void PropertyItem::notifyValueChange()
{
    if ( !propertyParent() ) {
	listview->valueChanged( this );
	setChanged( true );
	if ( hasSubItems() )
	    initChildren();
    } else {
	propertyParent()->childValueChanged( this );
	setChanged( true );
    }
}

/*!  If a subclass is a expandable item reimplement this as this is
  always called if a child item changed its value. So update the
  display of the item here then.
*/

void PropertyItem::childValueChanged( PropertyItem * )
{
}

/*!  When adding a child item, call this (normally from addChildren()
*/

void PropertyItem::addChild( PropertyItem *i )
{
    children.append( i );
}

int PropertyItem::childCount() const
{
    return children.count();
}

PropertyItem *PropertyItem::child( int i ) const
{
    // ARRRRRRRRG
    return ( (PropertyItem*)this )->children.at( i );
}

/*!  If the contents of the item is not displayable with a text, but
  you want to draw it yourself (using drawCustomContents()), return
  true here.
*/

bool PropertyItem::hasCustomContents() const
{
    return false;
}

/*!
  \sa hasCustomContents()
*/

void PropertyItem::drawCustomContents( QPainter *, const QRect & )
{
}

QString PropertyItem::currentItem() const
{
    return QString();
}

int PropertyItem::currentIntItem() const
{
    return -1;
}

void PropertyItem::setCurrentItem( const QString & )
{
}

void PropertyItem::setCurrentItem( int )
{
}

int PropertyItem::currentIntItemFromObject() const
{
    return -1;
}

QString PropertyItem::currentItemFromObject() const
{
    return QString();
}

void PropertyItem::setFocus( QWidget *w )
{
    if ( !qApp->focusWidget() ||
	 listview->propertyEditor()->formWindow() &&
	 ( !MainWindow::self->isAFormWindowChild( qApp->focusWidget() ) &&
	   !qApp->focusWidget()->inherits( "Editor" ) ) )
	w->setFocus();
}

void PropertyItem::setText( int col, const QString &t )
{
    QString txt( t );
    if ( col == 1 )
	txt = txt.replace( QRegExp( "\n" ), " " );
    Q3ListViewItem::setText( col, txt );
}

// --------------------------------------------------------------

PropertyTextItem::PropertyTextItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const QString &propName, bool comment, bool multiLine, bool ascii, bool a )
    : PropertyItem( l, after, prop, propName ), withComment( comment ),
      hasMultiLines( multiLine ), asciiOnly( ascii ), accel( a )
{
    lin = 0;
    box = 0;
}

QLineEdit *PropertyTextItem::lined()
{
    if ( lin )
	return lin;
    if ( hasMultiLines ) {
	box = new Q3HBox( listview->viewport() );
	box->setFrameStyle( Q3Frame::StyledPanel | Q3Frame::Sunken );
	box->setLineWidth( 2 );
	box->hide();
    }

    lin = 0;
    if ( hasMultiLines )
	lin = new QLineEdit( box );
    else
	lin = new QLineEdit( listview->viewport() );

    if ( asciiOnly ) {
	if ( PropertyItem::name() == "name" )
	    lin->setValidator( new AsciiValidator( lin, "ascii_validator" ) );
	else
	    lin->setValidator( new AsciiValidator( QString("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"
							   "\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9"
							   "\xaa\xab\xac\xad\xae\xaf\xb1\xb2\xb3"
							   "\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc"
							   "\xbd\xbe\xbf"), lin, "ascii_validator" ) );
    } if ( !hasMultiLines ) {
	lin->hide();
    } else {
	button = new QPushButton( i18n("..."), box );
	button->setFixedWidth( 20 );
	connect( button, SIGNAL( clicked() ),
		 this, SLOT( getText() ) );
	lin->setFrame( false );
    }
    connect( lin, SIGNAL( returnPressed() ),
	     this, SLOT( setValue() ) );
    connect( lin, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( setValue() ) );
    if ( PropertyItem::name() == "name" || PropertyItem::name() == "itemName" )
	connect( lin, SIGNAL( returnPressed() ),
		 listview->propertyEditor()->formWindow()->commandHistory(),
		 SLOT( checkCompressedCommand() ) );
    lin->installEventFilter( listview );
    return lin;
}

PropertyTextItem::~PropertyTextItem()
{
    delete (QLineEdit*)lin;
    lin = 0;
    delete (Q3HBox*)box;
    box = 0;
}

void PropertyTextItem::setChanged( bool b, bool updateDb )
{
    PropertyItem::setChanged( b, updateDb );
    if ( withComment && childCount() > 0 )
	( (PropertyTextItem*)PropertyItem::child( 0 ) )->lined()->setEnabled( b );
}

bool PropertyTextItem::hasSubItems() const
{
    return withComment;
}

void PropertyTextItem::childValueChanged( PropertyItem *child )
{
    if ( PropertyItem::name() != "name" )
	MetaDataBase::setPropertyComment( listview->propertyEditor()->widget(),
					  PropertyItem::name(), child->value().toString() );
    else
	MetaDataBase::setExportMacro( listview->propertyEditor()->widget(), child->value().toString() );
    listview->propertyEditor()->formWindow()->commandHistory()->setModified( true );
}

static QString to_string( const QVariant &v, bool accel )
{
    if ( !accel )
	return v.toString();
    return Q3Accel::keyToString( v.toInt() );
}

void PropertyTextItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin || lin->text().length() == 0 ) {
	lined()->blockSignals( true );
	lined()->setText( to_string( value(), accel ) );
	lined()->blockSignals( false );
    }

    QWidget* w;
    if ( hasMultiLines )
	w = box;
    else
	w= lined();

    placeEditor( w );
    if ( !w->isVisible() || !lined()->hasFocus() ) {
	w->show();
	setFocus( lined() );
    }
}

void PropertyTextItem::createChildren()
{
    PropertyTextItem *i = new PropertyTextItem( listview, this, this,
						PropertyItem::name() == "name" ?
						"export macro" : "comment", false, false,
						PropertyItem::name() == "name" );
    i->lined()->setEnabled( isChanged() );
    addChild( i );
}

void PropertyTextItem::initChildren()
{
    if ( !childCount() )
	return;
    PropertyItem *item = PropertyItem::child( 0 );
    if ( item ) {
	if ( PropertyItem::name() != "name" )
	    item->setValue( MetaDataBase::propertyComment( listview->propertyEditor()->widget(),
							   PropertyItem::name() ) );
	else
	    item->setValue( MetaDataBase::exportMacro( listview->propertyEditor()->widget() ) );
    }
}

void PropertyTextItem::hideEditor()
{
    PropertyItem::hideEditor();
    QWidget* w;
    if ( hasMultiLines )
	w = box;
    else
	w = lined();

    w->hide();
}

void PropertyTextItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;
    if ( lin ) {
	lined()->blockSignals( true );
	int oldCursorPos;
	oldCursorPos = lin->cursorPosition();
	lined()->setText( to_string( v, accel ) );
	if ( oldCursorPos < (int)lin->text().length() )
	    lin->setCursorPosition( oldCursorPos );
	lined()->blockSignals( false );
    }
    setText( 1, to_string( v, accel ) );
    PropertyItem::setValue( v );
}

void PropertyTextItem::setValue()
{
    setText( 1, lined()->text() );
    QVariant v;
    if ( accel )
	v = Q3Accel::stringToKey( lined()->text() );
    else
	v = lined()->text();
    PropertyItem::setValue( v );
    notifyValueChange();
}

void PropertyTextItem::getText()
{
    QString txt = TextEditor::getText( listview, value().toString() );
    if ( !txt.isEmpty() ) {
	setText( 1, txt );
	PropertyItem::setValue( txt );
	notifyValueChange();
	lined()->blockSignals( true );
	lined()->setText( txt );
	lined()->blockSignals( false );
    }
}

// --------------------------------------------------------------

PropertyDoubleItem::PropertyDoubleItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    lin = 0;
}

QLineEdit *PropertyDoubleItem::lined()
{
    if ( lin )
	return lin;
    lin = new QLineEdit( listview->viewport() );
    lin->setValidator( new QDoubleValidator( lin, "double_validator" ) );

    connect( lin, SIGNAL( returnPressed() ),
	     this, SLOT( setValue() ) );
    connect( lin, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( setValue() ) );
    lin->installEventFilter( listview );
    return lin;
}

PropertyDoubleItem::~PropertyDoubleItem()
{
    delete (QLineEdit*)lin;
    lin = 0;
}

void PropertyDoubleItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin ) {
	lined()->blockSignals( true );
	lined()->setText( QString::number( value().toDouble() ) );
	lined()->blockSignals( false );
    }
    QWidget* w = lined();

    placeEditor( w );
    if ( !w->isVisible() || !lined()->hasFocus() ) {
	w->show();
	setFocus( lined() );
    }
}


void PropertyDoubleItem::hideEditor()
{
    PropertyItem::hideEditor();
    QWidget* w = lined();
    w->hide();
}

void PropertyDoubleItem::setValue( const QVariant &v )
{
    if ( value() == v )
	return;
    if ( lin ) {
	lined()->blockSignals( true );
	int oldCursorPos;
	oldCursorPos = lin->cursorPosition();
	lined()->setText( QString::number( v.toDouble() ) );
	if ( oldCursorPos < (int)lin->text().length() )
	    lin->setCursorPosition( oldCursorPos );
	lined()->blockSignals( false );
    }
    setText( 1, QString::number( v.toDouble() ) );
    PropertyItem::setValue( v );
}

void PropertyDoubleItem::setValue()
{
    setText( 1, lined()->text() );
    QVariant v = lined()->text().toDouble();
    PropertyItem::setValue( v );
    notifyValueChange();
}


// --------------------------------------------------------------

PropertyDateItem::PropertyDateItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    lin = 0;
}

Q3DateEdit *PropertyDateItem::lined()
{
    if ( lin )
	return lin;
    lin = new Q3DateEdit( listview->viewport() );
    QObjectListl = lin->queryList( "QLineEdit" );
    for ( QObject *o = l->first(); o; o = l->next() )
	o->installEventFilter( listview );
    delete l;
    connect( lin, SIGNAL( valueChanged( const QDate & ) ),
	     this, SLOT( setValue() ) );
    return lin;
}

PropertyDateItem::~PropertyDateItem()
{
    delete (Q3DateEdit*)lin;
    lin = 0;
}

void PropertyDateItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin ) {
	lined()->blockSignals( true );
	lined()->setDate( value().toDate() );
	lined()->blockSignals( false );
    }
    placeEditor( lin );
    if ( !lin->isVisible() ) {
	lin->show();
	setFocus( lin );
    }
}

void PropertyDateItem::hideEditor()
{
    PropertyItem::hideEditor();
    if ( lin )
	lin->hide();
}

void PropertyDateItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( lin ) {
	lined()->blockSignals( true );
	if ( lined()->date() != v.toDate() )
	    lined()->setDate( v.toDate() );
	lined()->blockSignals( false );
    }
    setText( 1, v.toDate().toString( ::Qt::ISODate ) );
    PropertyItem::setValue( v );
}

void PropertyDateItem::setValue()
{
    setText( 1, lined()->date().toString( ::Qt::ISODate ) );
    QVariant v;
    v = lined()->date();
    PropertyItem::setValue( v );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyTimeItem::PropertyTimeItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    lin = 0;
}

Q3TimeEdit *PropertyTimeItem::lined()
{
    if ( lin )
	return lin;
    lin = new Q3TimeEdit( listview->viewport() );
    connect( lin, SIGNAL( valueChanged( const QTime & ) ),
	     this, SLOT( setValue() ) );
    QObjectListl = lin->queryList( "QLineEdit" );
    for ( QObject *o = l->first(); o; o = l->next() )
	o->installEventFilter( listview );
    delete l;
    return lin;
}

PropertyTimeItem::~PropertyTimeItem()
{
    delete (Q3TimeEdit*)lin;
    lin = 0;
}

void PropertyTimeItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin ) {
	lined()->blockSignals( true );
	lined()->setTime( value().toTime() );
	lined()->blockSignals( false );
    }
    placeEditor( lin );
    if ( !lin->isVisible() ) {
	lin->show();
	setFocus( lin );
    }
}

void PropertyTimeItem::hideEditor()
{
    PropertyItem::hideEditor();
    if ( lin )
	lin->hide();
}

void PropertyTimeItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( lin ) {
	lined()->blockSignals( true );
	if ( lined()->time() != v.toTime() )
	    lined()->setTime( v.toTime() );
	lined()->blockSignals( false );
    }
    setText( 1, v.toTime().toString( ::Qt::ISODate ) );
    PropertyItem::setValue( v );
}

void PropertyTimeItem::setValue()
{
    setText( 1, lined()->time().toString( ::Qt::ISODate ) );
    QVariant v;
    v = lined()->time();
    PropertyItem::setValue( v );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyDateTimeItem::PropertyDateTimeItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    lin = 0;
}

Q3DateTimeEdit *PropertyDateTimeItem::lined()
{
    if ( lin )
	return lin;
    lin = new Q3DateTimeEdit( listview->viewport() );
    connect( lin, SIGNAL( valueChanged( const QDateTime & ) ),
	     this, SLOT( setValue() ) );
    QObjectListl = lin->queryList( "QLineEdit" );
    for ( QObject *o = l->first(); o; o = l->next() )
	o->installEventFilter( listview );
    delete l;
    return lin;
}

PropertyDateTimeItem::~PropertyDateTimeItem()
{
    delete (Q3DateTimeEdit*)lin;
    lin = 0;
}

void PropertyDateTimeItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin ) {
	lined()->blockSignals( true );
	lined()->setDateTime( value().toDateTime() );
	lined()->blockSignals( false );
    }
    placeEditor( lin );
    if ( !lin->isVisible() ) {
	lin->show();
	setFocus( lin );
    }
}

void PropertyDateTimeItem::hideEditor()
{
    PropertyItem::hideEditor();
    if ( lin )
	lin->hide();
}

void PropertyDateTimeItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( lin ) {
	lined()->blockSignals( true );
	if ( lined()->dateTime() != v.toDateTime() )
	    lined()->setDateTime( v.toDateTime() );
	lined()->blockSignals( false );
    }
    setText( 1, v.toDateTime().toString( ::Qt::ISODate ) );
    PropertyItem::setValue( v );
}

void PropertyDateTimeItem::setValue()
{
    setText( 1, lined()->dateTime().toString( ::Qt::ISODate ) );
    QVariant v;
    v = lined()->dateTime();
    PropertyItem::setValue( v );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyBoolItem::PropertyBoolItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    comb = 0;
}

QComboBox *PropertyBoolItem::combo()
{
    if ( comb )
	return comb;
    comb = new QComboBox( false, listview->viewport() );
    comb->hide();
    comb->insertItem( i18n("False" ) );
    comb->insertItem( i18n("True" ) );
    connect( comb, SIGNAL( activated( int ) ),
	     this, SLOT( setValue() ) );
    comb->installEventFilter( listview );
    return comb;
}

PropertyBoolItem::~PropertyBoolItem()
{
    delete (QComboBox*)comb;
    comb = 0;
}

void PropertyBoolItem::toggle()
{
    bool b = value().toBool();
    setValue( QVariant( !b, 0 ) );
    setValue();
}

void PropertyBoolItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !comb ) {
	combo()->blockSignals( true );
	if ( value().toBool() )
	    combo()->setCurrentItem( 1 );
	else
	    combo()->setCurrentItem( 0 );
	combo()->blockSignals( false );
    }
    placeEditor( combo() );
    if ( !combo()->isVisible()  || !combo()->hasFocus() ) {
	combo()->show();
	setFocus( combo() );
    }
}

void PropertyBoolItem::hideEditor()
{
    PropertyItem::hideEditor();
    combo()->hide();
}

void PropertyBoolItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( comb ) {
	combo()->blockSignals( true );
	if ( v.toBool() )
	    combo()->setCurrentItem( 1 );
	else
	    combo()->setCurrentItem( 0 );
	combo()->blockSignals( false );
    }
    QString tmp = i18n("True" );
    if ( !v.toBool() )
	tmp = i18n("False" );
    setText( 1, tmp );
    PropertyItem::setValue( v );
}

void PropertyBoolItem::setValue()
{
    if ( !comb )
	return;
    setText( 1, combo()->currentText() );
    bool b = combo()->currentItem() == 0 ? (bool)false : (bool)true;
    PropertyItem::setValue( QVariant( b, 0 ) );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyIntItem::PropertyIntItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				  const QString &propName, bool s )
    : PropertyItem( l, after, prop, propName ), signedValue( s )
{
    spinBx = 0;
}

QSpinBox *PropertyIntItem::spinBox()
{
    if ( spinBx )
	return spinBx;
    if ( signedValue )
	spinBx = new QSpinBox( -INT_MAX, INT_MAX, 1, listview->viewport() );
    else
	spinBx = new QSpinBox( 0, INT_MAX, 1, listview->viewport() );
    spinBx->hide();
    spinBx->installEventFilter( listview );
    QObjectListol = spinBx->queryList( "QLineEdit" );
    if ( ol && ol->first() )
	ol->first()->installEventFilter( listview );
    delete ol;
    connect( spinBx, SIGNAL( valueChanged( int ) ),
	     this, SLOT( setValue() ) );
    return spinBx;
}

PropertyIntItem::~PropertyIntItem()
{
    delete (QSpinBox*)spinBx;
    spinBx = 0;
}

void PropertyIntItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !spinBx ) {
	spinBox()->blockSignals( true );
	if ( signedValue )
	    spinBox()->setValue( value().toInt() );
	else
	    spinBox()->setValue( value().toUInt() );
	spinBox()->blockSignals( false );
    }
    placeEditor( spinBox() );
    if ( !spinBox()->isVisible()  || !spinBox()->hasFocus()  ) {
	spinBox()->show();
	setFocus( spinBox() );
    }
}

void PropertyIntItem::hideEditor()
{
    PropertyItem::hideEditor();
    spinBox()->hide();
}

void PropertyIntItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( spinBx ) {
	spinBox()->blockSignals( true );
	if ( signedValue )
	    spinBox()->setValue( v.toInt() );
	else
	    spinBox()->setValue( v.toUInt() );
	spinBox()->blockSignals( false );
    }

    if ( signedValue )
	    setText( 1, QString::number( v.toInt() ) );
    else
	    setText( 1, QString::number( v.toUInt() ) );
    PropertyItem::setValue( v );
}

void PropertyIntItem::setValue()
{
    if ( !spinBx )
	return;
    setText( 1, QString::number( spinBox()->value() ) );
    if ( signedValue )
	PropertyItem::setValue( spinBox()->value() );
    else
	PropertyItem::setValue( (uint)spinBox()->value() );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyListItem::PropertyListItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const QString &propName, bool e )
    : PropertyItem( l, after, prop, propName ), editable( e )
{
    comb = 0;
    oldInt = -1;
}

QComboBox *PropertyListItem::combo()
{
    if ( comb )
	return comb;
    comb = new QComboBox( editable, listview->viewport() );
    comb->hide();
    connect( comb, SIGNAL( activated( int ) ),
	     this, SLOT( setValue() ) );
    comb->installEventFilter( listview );
    if ( editable ) {
	QObjectListol = comb->queryList( "QLineEdit" );
	if ( ol && ol->first() )
	    ol->first()->installEventFilter( listview );
	delete ol;
    }
    return comb;
}

PropertyListItem::~PropertyListItem()
{
    delete (QComboBox*)comb;
    comb = 0;
}

void PropertyListItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !comb ) {
	combo()->blockSignals( true );
	combo()->clear();
	combo()->insertStringList( value().toStringList() );
	combo()->blockSignals( false );
    }
    placeEditor( combo() );
    if ( !combo()->isVisible()  || !combo()->hasFocus() ) {
	combo()->show();
	setFocus( combo() );
    }
}

void PropertyListItem::hideEditor()
{
    PropertyItem::hideEditor();
    combo()->hide();
}

void PropertyListItem::setValue( const QVariant &v )
{
    if ( comb ) {
	combo()->blockSignals( true );
	combo()->clear();
	combo()->insertStringList( v.toStringList() );
	combo()->blockSignals( false );
    }
    setText( 1, v.toStringList().first() );
    PropertyItem::setValue( v );
}

void PropertyListItem::setValue()
{
    if ( !comb )
	return;
    setText( 1, combo()->currentText() );
    QStringList lst;
    for ( uint i = 0; i < combo()->listBox()->count(); ++i )
	lst << combo()->listBox()->item( i )->text();
    PropertyItem::setValue( lst );
    notifyValueChange();
    oldInt = currentIntItem();
    oldString = currentItem();
}

QString PropertyListItem::currentItem() const
{
    return ( (PropertyListItem*)this )->combo()->currentText();
}

void PropertyListItem::setCurrentItem( const QString &s )
{
    if ( comb && currentItem().toLower() == s.toLower() )
	return;

    if ( !comb ) {
	combo()->blockSignals( true );
	combo()->clear();
	combo()->insertStringList( value().toStringList() );
	combo()->blockSignals( false );
    }
    for ( uint i = 0; i < combo()->listBox()->count(); ++i ) {
	if ( combo()->listBox()->item( i )->text().toLower() == s.toLower() ) {
	    combo()->setCurrentItem( i );
	    setText( 1, combo()->currentText() );
	    break;
	}
    }
    oldInt = currentIntItem();
    oldString = currentItem();
}

void PropertyListItem::addItem( const QString &s )
{
    combo()->insertItem( s );
}

void PropertyListItem::setCurrentItem( int i )
{
    if ( comb && i == combo()->currentItem() )
	return;

    if ( !comb ) {
	combo()->blockSignals( true );
	combo()->clear();
	combo()->insertStringList( value().toStringList() );
	combo()->blockSignals( false );
    }
    combo()->setCurrentItem( i );
    setText( 1, combo()->currentText() );
    oldInt = currentIntItem();
    oldString = currentItem();
}

int PropertyListItem::currentIntItem() const
{
    return ( (PropertyListItem*)this )->combo()->currentItem();
}

int PropertyListItem::currentIntItemFromObject() const
{
    return oldInt;
}

QString PropertyListItem::currentItemFromObject() const
{
    return oldString;
}

// --------------------------------------------------------------

PropertyCoordItem::PropertyCoordItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const QString &propName, Type t )
    : PropertyItem( l, after, prop, propName ), typ( t )
{
    lin = 0;

}

QLineEdit *PropertyCoordItem::lined()
{
    if ( lin )
	return lin;
    lin = new QLineEdit( listview->viewport() );
    lin->setReadOnly( true );
    lin->installEventFilter( listview );
    lin->hide();
    return lin;
}

void PropertyCoordItem::createChildren()
{
    PropertyItem *i = this;
    if ( typ == Rect || typ == Point ) {
	i = new PropertyIntItem( listview, i, this, i18n("x" ), true );
	addChild( i );
	i = new PropertyIntItem( listview, i, this, i18n("y" ), true );
	addChild( i );
    }
    if ( typ == Rect || typ == Size ) {
	i = new PropertyIntItem( listview, i, this, i18n("width" ), true );
	addChild( i );
	i = new PropertyIntItem( listview, i, this, i18n("height" ), true );
	addChild( i );
    }
}

void PropertyCoordItem::initChildren()
{
    PropertyItem *item = 0;
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == i18n("x" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().x() );
	    else if ( typ == Point )
		item->setValue( val.toPoint().x() );
	} else if ( item->name() == i18n("y" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().y() );
	    else if ( typ == Point )
		item->setValue( val.toPoint().y() );
	} else if ( item->name() == i18n("width" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().width() );
	    else if ( typ == Size )
		item->setValue( val.toSize().width() );
	} else if ( item->name() == i18n("height" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().height() );
	    else if ( typ == Size )
		item->setValue( val.toSize().height() );
	}
    }
}

PropertyCoordItem::~PropertyCoordItem()
{
    delete (QLineEdit*)lin;
    lin = 0;
}

void PropertyCoordItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin )
	lined()->setText( text( 1 ) );
    placeEditor( lined() );
    if ( !lined()->isVisible() || !lined()->hasFocus()  ) {
	lined()->show();
	setFocus( lined() );
    }
}

void PropertyCoordItem::hideEditor()
{
    PropertyItem::hideEditor();
    lined()->hide();
}

void PropertyCoordItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    QString s;
    if ( typ == Rect )
	s = "[ " + QString::number( v.toRect().x() ) + ", " + QString::number( v.toRect().y() ) + ", " +
	    QString::number( v.toRect().width() ) + ", " + QString::number( v.toRect().height() ) + " ]";
    else if ( typ == Point )
	s = "[ " + QString::number( v.toPoint().x() ) + ", " +
	    QString::number( v.toPoint().y() ) + " ]";
    else if ( typ == Size )
	s = "[ " + QString::number( v.toSize().width() ) + ", " +
	    QString::number( v.toSize().height() ) + " ]";
    setText( 1, s );
    if ( lin )
	lined()->setText( s );
    PropertyItem::setValue( v );
}

bool PropertyCoordItem::hasSubItems() const
{
    return true;
}

void PropertyCoordItem::childValueChanged( PropertyItem *child )
{
    if ( typ == Rect ) {
	QRect r = value().toRect();
	if ( child->name() == i18n("x" ) )
	    r.setX( child->value().toInt() );
	else if ( child->name() == i18n("y" ) )
	    r.setY( child->value().toInt() );
	else if ( child->name() == i18n("width" ) )
	    r.setWidth( child->value().toInt() );
	else if ( child->name() == i18n("height" ) )
	    r.setHeight( child->value().toInt() );
	setValue( r );
    } else if ( typ == Point ) {
	QPoint r = value().toPoint();
	if ( child->name() == i18n("x" ) )
	    r.setX( child->value().toInt() );
	else if ( child->name() == i18n("y" ) )
	    r.setY( child->value().toInt() );
	setValue( r );
    } else if ( typ == Size ) {
	QSize r = value().toSize();
	if ( child->name() == i18n("width" ) )
	    r.setWidth( child->value().toInt() );
	else if ( child->name() == i18n("height" ) )
	    r.setHeight( child->value().toInt() );
	setValue( r );
    }
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyPixmapItem::PropertyPixmapItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				      const QString &propName, bool isIconSet )
    : PropertyItem( l, after, prop, propName ), iconSet( isIconSet )
{
    box = new Q3HBox( listview->viewport() );
    box->hide();
    pixPrev = new QLabel( box );
    pixPrev->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum ) );
    pixPrev->setBackgroundColor( pixPrev->colorGroup().color( QPalette::Base ) );
    button = new QPushButton( "...", box );
    button->setFixedWidth( 20 );
    box->setFrameStyle( Q3Frame::StyledPanel | Q3Frame::Sunken );
    box->setLineWidth( 2 );
    pixPrev->setFrameStyle( Q3Frame::NoFrame );
    box->installEventFilter( listview );
    connect( button, SIGNAL( clicked() ),
	     this, SLOT( getPixmap() ) );
}
PropertyPixmapItem::~PropertyPixmapItem()
{
    delete (Q3HBox*)box;
}

void PropertyPixmapItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() ) {
	box->show();
	listView()->viewport()->setFocus();
    }
}

void PropertyPixmapItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyPixmapItem::setValue( const QVariant &v )
{
    QString s;
    if ( !iconSet )
	pixPrev->setPixmap( v.toPixmap() );
    else
	pixPrev->setPixmap( v.toIconSet().pixmap() );
    PropertyItem::setValue( v );
    repaint();
}

void PropertyPixmapItem::getPixmap()
{
    QPixmap pix = qChoosePixmap( listview, listview->propertyEditor()->formWindow(), value().toPixmap() );
    if ( !pix.isNull() ) {
	if ( !iconSet )
	    setValue( pix );
	else
	    setValue( QIcon( pix ) );
	notifyValueChange();
    }
}

bool PropertyPixmapItem::hasCustomContents() const
{
    return true;
}

void PropertyPixmapItem::drawCustomContents( QPainter *p, const QRect &r )
{
    QPixmap pix( (!iconSet ? value().toPixmap() : value().toIconSet().pixmap()) );
    if ( !pix.isNull() ) {
	p->save();
	p->setClipRect( QRect( QPoint( (int)(p->worldMatrix().dx() + r.x()),
				       (int)(p->worldMatrix().dy() + r.y()) ),
			       r.size() ) );
	p->drawPixmap( r.x(), r.y() + ( r.height() - pix.height() ) / 2, pix );
	p->restore();
    }
}


// --------------------------------------------------------------

PropertyColorItem::PropertyColorItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				      const QString &propName, bool children )
    : PropertyItem( l, after, prop, propName ), withChildren( children )
{
    box = new Q3HBox( listview->viewport() );
    box->hide();
    colorPrev = new Q3Frame( box );
    button = new QPushButton( "...", box );
    button->setFixedWidth( 20 );
    box->setFrameStyle( Q3Frame::StyledPanel | Q3Frame::Sunken );
    box->setLineWidth( 2 );
    colorPrev->setFrameStyle( Q3Frame::Plain | Q3Frame::Box );
    colorPrev->setLineWidth( 2 );
    QPalette pal = colorPrev->palette();
    QColorGroup cg = pal.active();
    cg.setColor( QPalette::Foreground, cg.color( QPalette::Base ) );
    pal.setActive( cg );
    pal.setInactive( cg );
    pal.setDisabled( cg );
    colorPrev->setPalette( pal );
    box->installEventFilter( listview );
    connect( button, SIGNAL( clicked() ),
	     this, SLOT( getColor() ) );
}

void PropertyColorItem::createChildren()
{
    PropertyItem *i = this;
    i = new PropertyIntItem( listview, i, this, i18n("Red" ), true );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, i18n("Green" ), true );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, i18n("Blue" ), true );
    addChild( i );
}

void PropertyColorItem::initChildren()
{
    PropertyItem *item = 0;
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == i18n("Red" ) )
	    item->setValue( val.toColor().red() );
	else if ( item->name() == i18n("Green" ) )
	    item->setValue( val.toColor().green() );
	else if ( item->name() == i18n("Blue" ) )
	    item->setValue( val.toColor().blue() );
    }
}

PropertyColorItem::~PropertyColorItem()
{
    delete (Q3HBox*)box;
}

void PropertyColorItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() ) {
	box->show();
	listView()->viewport()->setFocus();
    }
}

void PropertyColorItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyColorItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    QString s;
    setText( 1, v.toColor().name() );
    colorPrev->setBackgroundColor( v.toColor() );
    PropertyItem::setValue( v );
}

bool PropertyColorItem::hasSubItems() const
{
    return withChildren;
}

void PropertyColorItem::childValueChanged( PropertyItem *child )
{
    QColor c( val.toColor() );
    if ( child->name() == i18n("Red" ) )
	c.setRgb( child->value().toInt(), c.green(), c.blue() );
    else if ( child->name() == i18n("Green" ) )
	c.setRgb( c.red(), child->value().toInt(), c.blue() );
    else if ( child->name() == i18n("Blue" ) )
	c.setRgb( c.red(), c.green(), child->value().toInt() );
    setValue( c );
    notifyValueChange();
}

void PropertyColorItem::getColor()
{
    QColor c = val.asColor();
    if ( KColorDialog::getColor( c, listview ) == QDialog::Accepted ) {
	setValue( c );
	notifyValueChange();
    }
}

bool PropertyColorItem::hasCustomContents() const
{
    return true;
}

void PropertyColorItem::drawCustomContents( QPainter *p, const QRect &r )
{
    p->save();
    p->setPen( QPen( black, 1 ) );
    p->setBrush( val.toColor() );
    p->drawRect( r.x() + 2, r.y() + 2, r.width() - 5, r.height() - 5 );
    p->restore();
}

// --------------------------------------------------------------

PropertyFontItem::PropertyFontItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    box = new Q3HBox( listview->viewport() );
    box->hide();
    lined = new QLineEdit( box );
    button = new QPushButton( "...", box );
    button->setFixedWidth( 20 );
    box->setFrameStyle( Q3Frame::StyledPanel | Q3Frame::Sunken );
    box->setLineWidth( 2 );
    lined->setFrame( false );
    lined->setReadOnly( true );
    box->setFocusProxy( lined );
    box->installEventFilter( listview );
    lined->installEventFilter( listview );
    button->installEventFilter( listview );
    connect( button, SIGNAL( clicked() ),
	     this, SLOT( getFont() ) );
}

void PropertyFontItem::createChildren()
{
    PropertyItem *i = this;
    i = new PropertyListItem( listview, i, this, i18n("Family" ), false );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, i18n("Point Size" ), true );
    addChild( i );
    i = new PropertyBoolItem( listview, i, this, i18n("Bold" ) );
    addChild( i );
    i = new PropertyBoolItem( listview, i, this, i18n("Italic" ) );
    addChild( i );
    i = new PropertyBoolItem( listview, i, this, i18n("Underline" ) );
    addChild( i );
    i = new PropertyBoolItem( listview, i, this, i18n("Strikeout" ) );
    addChild( i );
}

void PropertyFontItem::initChildren()
{
    PropertyItem *item = 0;
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == i18n("Family" ) ) {
	    ( (PropertyListItem*)item )->setValue( getFontList() );
	    ( (PropertyListItem*)item )->setCurrentItem( val.toFont().family() );
	} else if ( item->name() == i18n("Point Size" ) )
	    item->setValue( val.toFont().pointSize() );
	else if ( item->name() == i18n("Bold" ) )
	    item->setValue( QVariant( val.toFont().bold(), 0 ) );
	else if ( item->name() == i18n("Italic" ) )
	    item->setValue( QVariant( val.toFont().italic(), 0 ) );
	else if ( item->name() == i18n("Underline" ) )
	    item->setValue( QVariant( val.toFont().underline(), 0 ) );
	else if ( item->name() == i18n("Strikeout" ) )
	    item->setValue( QVariant( val.toFont().strikeOut(), 0 ) );
    }
}

PropertyFontItem::~PropertyFontItem()
{
    delete (Q3HBox*)box;
}

void PropertyFontItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() || !lined->hasFocus() ) {
	box->show();
	setFocus( lined );
    }
}

void PropertyFontItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyFontItem::setValue( const QVariant &v )
{
    if ( value() == v )
	return;

    setText( 1, v.toFont().family() + "-" + QString::number( v.toFont().pointSize() ) );
    lined->setText( v.toFont().family() + "-" + QString::number( v.toFont().pointSize() ) );
    PropertyItem::setValue( v );
}

void PropertyFontItem::getFont()
{
    bool ok = false;
    QFont f = QFontDialog::getFont( &ok, val.toFont(), listview );
    if ( ok && f != val.toFont() ) {
	setValue( f );
	notifyValueChange();
    }
}

bool PropertyFontItem::hasSubItems() const
{
    return true;
}

void PropertyFontItem::childValueChanged( PropertyItem *child )
{
    QFont f = val.toFont();
    if ( child->name() == i18n("Family" ) )
	f.setFamily( ( (PropertyListItem*)child )->currentItem() );
    else if ( child->name() == i18n("Point Size" ) )
	f.setPointSize( child->value().toInt() );
    else if ( child->name() == i18n("Bold" ) )
	f.setBold( child->value().toBool() );
    else if ( child->name() == i18n("Italic" ) )
	f.setItalic( child->value().toBool() );
    else if ( child->name() == i18n("Underline" ) )
	f.setUnderline( child->value().toBool() );
    else if ( child->name() == i18n("Strikeout" ) )
	f.setStrikeOut( child->value().toBool() );
    setValue( f );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyDatabaseItem::PropertyDatabaseItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
					    const QString &propName, bool wField )
    : PropertyItem( l, after, prop, propName ), withField( wField )
{
    box = new Q3HBox( listview->viewport() );
    box->hide();
    lined = new QLineEdit( box );
    button = new QPushButton( "...", box );
    button->setFixedWidth( 20 );
    box->setFrameStyle( Q3Frame::StyledPanel | Q3Frame::Sunken );
    box->setLineWidth( 2 );
    lined->setFrame( false );
    lined->setReadOnly( true );
    box->setFocusProxy( lined );
    box->installEventFilter( listview );
    lined->installEventFilter( listview );
    button->installEventFilter( listview );
}

void PropertyDatabaseItem::createChildren()
{
    PropertyItem *i = this;
    i = new PropertyListItem( listview, i, this, i18n("Connection" ), true );
    addChild( i );
    i = new PropertyListItem( listview, i, this, i18n("Table" ), true );
    addChild( i );
    if ( withField ) {
	i = new PropertyListItem( listview, i, this, i18n("Field" ), true );
	addChild( i );
    }
}

void PropertyDatabaseItem::initChildren()
{
#ifndef QT_NO_SQL
    PropertyItem *item = 0;
    QStringList lst = value().toStringList();
    QString conn, table;
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == i18n("Connection" ) ) {
		QStringList cl = listview->propertyEditor()->formWindow()->project()->databaseConnectionList();
	    if ( !cl.isEmpty() )
		item->setValue( cl );
	    else if ( lst.count() > 0 )
		item->setValue( QStringList( lst[ 0 ] ) );
	    else if ( withField )
		item->setValue( QStringList( MetaDataBase::fakeProperty( listview->propertyEditor()->formWindow()->mainContainer(),
									 "database" ).toStringList()[ 0 ] ) );

	    if ( lst.count() > 0 && !lst[ 0 ].isEmpty() )
		item->setCurrentItem( lst[ 0 ] );
	    else if ( !isChanged() && withField )
		item->setCurrentItem( MetaDataBase::fakeProperty( listview->propertyEditor()->formWindow()->mainContainer(),
								  "database" ).toStringList()[ 0 ] );
	    else
		item->setCurrentItem( 0 );
	    conn = item->currentItem();
	} else if ( item->name() == i18n("Table" ) ) {
	    QStringList cl = listview->propertyEditor()->formWindow()->project()->databaseTableList( conn );
	    if ( !cl.isEmpty() )
		item->setValue( cl );
	    else if ( lst.count() > 1 )
		item->setValue( QStringList( lst[ 1 ] ) );
	    else if ( withField ) {
		QStringList fakeLst = MetaDataBase::fakeProperty( listview->propertyEditor()->formWindow()->mainContainer(), "database" ).toStringList();
		if ( fakeLst.count() > 1 )
		    item->setValue( fakeLst[ 1 ] );
	    }

	    if ( lst.count() > 1 && !lst[ 1 ].isEmpty() )
		item->setCurrentItem( lst[ 1 ] );
	    else if ( !isChanged() && withField ) {
		QStringList fakeLst = MetaDataBase::fakeProperty( listview->propertyEditor()->formWindow()->mainContainer(), "database" ).toStringList();
		if ( fakeLst.count() > 1 )
		    item->setCurrentItem( fakeLst[ 1 ] );
	        else
	            item->setCurrentItem( 0 );
	    } else
		item->setCurrentItem( 0 );
	    table = item->currentItem();
	} else if ( item->name() == i18n("Field" ) ) {
	    QStringList cl = listview->propertyEditor()->formWindow()->project()->databaseFieldList( conn, table );
	    if ( !cl.isEmpty() )
		item->setValue( cl );
	    else if ( lst.count() > 2 )
		item->setValue( QStringList( lst[ 2 ] ) );
	    if ( lst.count() > 2 && !lst[ 2 ].isEmpty() )
		item->setCurrentItem( lst[ 2 ] );
	    else
		item->setCurrentItem( 0 );
	}
    }
#endif
}

PropertyDatabaseItem::~PropertyDatabaseItem()
{
    delete (Q3HBox*)box;
}

void PropertyDatabaseItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() || !lined->hasFocus() ) {
	box->show();
	setFocus( lined );
    }
}

void PropertyDatabaseItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyDatabaseItem::setValue( const QVariant &v )
{
    if ( value() == v )
	return;

    QStringList lst = v.toStringList();
    QString s = lst.join( "." );
    setText( 1, s );
    lined->setText( s );
    PropertyItem::setValue( v );
}

bool PropertyDatabaseItem::hasSubItems() const
{
    return true;
}

void PropertyDatabaseItem::childValueChanged( PropertyItem *c )
{
#ifndef QT_NO_SQL
    QStringList lst;
    lst << ( (PropertyListItem*)PropertyItem::child( 0 ) )->currentItem()
	<< ( (PropertyListItem*)PropertyItem::child( 1 ) )->currentItem();
    if ( withField )
	lst << ( (PropertyListItem*)PropertyItem::child( 2 ) )->currentItem();
    if ( c == PropertyItem::child( 0 ) ) { // if the connection changed
	lst[ 0 ] = ( (PropertyListItem*)c )->currentItem();
	PropertyItem::child( 1 )->setValue( listview->propertyEditor()->formWindow()->project()->databaseTableList( lst[ 0 ] ) );
	if ( withField )
	    PropertyItem::child( 2 )->setValue( listview->propertyEditor()->formWindow()->project()->databaseFieldList( lst[ 0 ], lst[ 1 ] ) );
    } else if ( withField && c == PropertyItem::child( 1 ) ) { // if the table changed
	lst[ 1 ] = ( (PropertyListItem*)c )->currentItem();
	if ( withField )
	    PropertyItem::child( 2 )->setValue( listview->propertyEditor()->formWindow()->project()->databaseFieldList( lst[ 0 ], lst[ 1 ] ) );
    }
    lst.clear();
    lst << ( (PropertyListItem*)PropertyItem::child( 0 ) )->currentItem()
	<< ( (PropertyListItem*)PropertyItem::child( 1 ) )->currentItem();
    if ( withField )
	lst << ( (PropertyListItem*)PropertyItem::child( 2 ) )->currentItem();
    setValue( lst );
    notifyValueChange();
#else
    Q_UNUSED( c );
#endif
}

// --------------------------------------------------------------

PropertySizePolicyItem::PropertySizePolicyItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
						const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    lin = 0;
}

QLineEdit *PropertySizePolicyItem::lined()
{
    if ( lin )
	return lin;
    lin = new QLineEdit( listview->viewport() );
    lin->hide();
    lin->setReadOnly( true );
    return lin;
}

void PropertySizePolicyItem::createChildren()
{
    QStringList lst;
    lst << "Fixed" << "Minimum" << "Maximum" << "Preferred" << "MinimumExpanding" << "Expanding" << "Ignored";

    PropertyItem *i = this;
    i = new PropertyListItem( listview, i, this, i18n("hSizeType" ), false );
    i->setValue( lst );
    addChild( i );
    i = new PropertyListItem( listview, i, this, i18n("vSizeType" ), false );
    i->setValue( lst );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, i18n("horizontalStretch" ), true );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, i18n("verticalStretch" ), true );
    addChild( i );
}

void PropertySizePolicyItem::initChildren()
{
    PropertyItem *item = 0;
    QSizePolicy sp = val.toSizePolicy();
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == i18n("hSizeType" ) )
	    ( (PropertyListItem*)item )->setCurrentItem( size_type_to_int( sp.horData() ) );
	else if ( item->name() == i18n("vSizeType" ) )
	    ( (PropertyListItem*)item )->setCurrentItem( size_type_to_int( sp.verData() ) );
	else if ( item->name() == i18n("horizontalStretch" ) )
	    ( (PropertyIntItem*)item )->setValue( sp.horStretch() );
	else if ( item->name() == i18n("verticalStretch" ) )
	    ( (PropertyIntItem*)item )->setValue( sp.verStretch() );
    }
}

PropertySizePolicyItem::~PropertySizePolicyItem()
{
    delete (QLineEdit*)lin;
}

void PropertySizePolicyItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( lined() );
    if ( !lined()->isVisible() || !lined()->hasFocus() ) {
	lined()->show();
	listView()->viewport()->setFocus();
    }
}

void PropertySizePolicyItem::hideEditor()
{
    PropertyItem::hideEditor();
    lined()->hide();
}

void PropertySizePolicyItem::setValue( const QVariant &v )
{
    if ( value() == v )
	return;

    QString s = QString( "%1/%2/%2/%2" );
    s = s.arg( size_type_to_string( v.toSizePolicy().horData() ) ).
	arg( size_type_to_string( v.toSizePolicy().verData() ) ).
	arg( v.toSizePolicy().horStretch() ).
	arg( v.toSizePolicy().verStretch() );
    setText( 1, s );
    lined()->setText( s );
    PropertyItem::setValue( v );
}

void PropertySizePolicyItem::childValueChanged( PropertyItem *child )
{
    QSizePolicy sp = val.toSizePolicy();
    if ( child->name() == i18n("hSizeType" ) )
	sp.setHorData( int_to_size_type( ( ( PropertyListItem*)child )->currentIntItem() ) );
    else if ( child->name() == i18n("vSizeType" ) )
	sp.setVerData( int_to_size_type( ( ( PropertyListItem*)child )->currentIntItem() ) );
    else if ( child->name() == i18n("horizontalStretch" ) )
	sp.setHorStretch( ( ( PropertyIntItem*)child )->value().toInt() );
    else if ( child->name() == i18n("verticalStretch" ) )
	sp.setVerStretch( ( ( PropertyIntItem*)child )->value().toInt() );
    setValue( sp );
    notifyValueChange();
}

bool PropertySizePolicyItem::hasSubItems() const
{
    return true;
}

// --------------------------------------------------------------

PropertyPaletteItem::PropertyPaletteItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				      const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    box = new Q3HBox( listview->viewport() );
    box->hide();
    palettePrev = new QLabel( box );
    button = new QPushButton( "...", box );
    button->setFixedWidth( 20 );
    box->setFrameStyle( Q3Frame::StyledPanel | Q3Frame::Sunken );
    box->setLineWidth( 2 );
    palettePrev->setFrameStyle( Q3Frame::NoFrame );
    box->installEventFilter( listview );
    connect( button, SIGNAL( clicked() ),
	     this, SLOT( getPalette() ) );
}
PropertyPaletteItem::~PropertyPaletteItem()
{
    delete (Q3HBox*)box;
}

void PropertyPaletteItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() ) {
	box->show();
	listView()->viewport()->setFocus();
    }
}

void PropertyPaletteItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyPaletteItem::setValue( const QVariant &v )
{
    QString s;
    palettePrev->setPalette( v.toPalette() );
    PropertyItem::setValue( v );
    repaint();
}

void PropertyPaletteItem::getPalette()
{
    if ( !listview->propertyEditor()->widget()->isWidgetType() )
	return;
    bool ok = false;
    QWidget *w = (QWidget*)listview->propertyEditor()->widget();
    if ( w->inherits( "QScrollView" ) )
	w = ( (Q3ScrollView*)w )->viewport();
    QPalette pal = PaletteEditor::getPalette( &ok, val.toPalette(),
#if defined(QT_NON_COMMERCIAL)
					      w->backgroundMode(), listview->topLevelWidget(),
#else
					      w->backgroundMode(), listview,
#endif
 					      "choose_palette", listview->propertyEditor()->formWindow() );
    if ( !ok )
	return;
    setValue( pal );
    notifyValueChange();
}

bool PropertyPaletteItem::hasCustomContents() const
{
    return true;
}

void PropertyPaletteItem::drawCustomContents( QPainter *p, const QRect &r )
{
    QPalette pal( value().toPalette() );
    p->save();
    p->setClipRect( QRect( QPoint( (int)(p->worldMatrix().dx() + r.x()),
				   (int)(p->worldMatrix().dy() + r.y()) ),
			   r.size() ) );
    QRect r2( r );
    r2.setX( r2.x() + 2 );
    r2.setY( r2.y() + 2 );
    r2.setWidth( r2.width() - 3 );
    r2.setHeight( r2.height() - 3 );
    p->setPen( QPen( black, 1 ) );
    p->setBrush( pal.active().background() );
    p->drawRect( r2 );
    p->restore();
}

// --------------------------------------------------------------

PropertyCursorItem::PropertyCursorItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
					const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    comb = 0;
}

QComboBox *PropertyCursorItem::combo()
{
    if ( comb )
	return comb;
    comb = new QComboBox( false, listview->viewport() );
    comb->hide();
    QBitmap cur;

    cur = QBitmap(arrow_width, arrow_height, arrow_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Arrow"), QObject::ArrowCursor);

    cur = QBitmap(uparrow_width, uparrow_height, uparrow_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Up-Arrow"), QObject::UpArrowCursor );

    cur = QBitmap(cross_width, cross_height, cross_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Cross"), QObject::CrossCursor );

    cur = QBitmap(wait_width, wait_height, wait_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Waiting"), QObject::WaitCursor );

    cur = QBitmap(ibeam_width, ibeam_height, ibeam_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("iBeam"), QObject::IbeamCursor );

    cur = QBitmap(sizev_width, sizev_height, sizev_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Size Vertical"), QObject::SizeVerCursor );

    cur = QBitmap(sizeh_width, sizeh_height, sizeh_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Size Horizontal"), QObject::SizeHorCursor );

    cur = QBitmap(sizef_width, sizef_height, sizef_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Size Slash"), QObject::SizeBDiagCursor );

    cur = QBitmap(sizeb_width, sizeb_height, sizeb_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Size Backslash"), QObject::SizeFDiagCursor );

    cur = QBitmap(sizeall_width, sizeall_height, sizeall_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Size All"), QObject::SizeAllCursor );

    cur = QBitmap( 25, 25, 1 );
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Blank"), QObject::BlankCursor );

    cur = QBitmap(vsplit_width, vsplit_height, vsplit_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Split Vertical"), QObject::SplitVCursor );

    cur = QBitmap(hsplit_width, hsplit_height, hsplit_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Split Horizontal"), QObject::SplitHCursor );

    cur = QBitmap(hand_width, hand_height, hand_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Pointing Hand"), QObject::PointingHandCursor );

    cur = QBitmap(no_width, no_height, no_bits, true);
    cur.setMask( cur );
    comb->insertItem( cur, i18n("Forbidden"), QObject::ForbiddenCursor );

    connect( comb, SIGNAL( activated( int ) ),
	     this, SLOT( setValue() ) );
    comb->installEventFilter( listview );
    return comb;
}

PropertyCursorItem::~PropertyCursorItem()
{
    delete (QComboBox*)comb;
}

void PropertyCursorItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !comb ) {
	combo()->blockSignals( true );
	combo()->setCurrentItem( (int)value().toCursor().shape() );
	combo()->blockSignals( false );
    }
    placeEditor( combo() );
    if ( !combo()->isVisible() || !combo()->hasFocus() ) {
	combo()->show();
	setFocus( combo() );
    }
}

void PropertyCursorItem::hideEditor()
{
    PropertyItem::hideEditor();
    combo()->hide();
}

void PropertyCursorItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    combo()->blockSignals( true );
    combo()->setCurrentItem( (int)v.toCursor().shape() );
    combo()->blockSignals( false );
    setText( 1, combo()->currentText() );
    PropertyItem::setValue( v );
}

void PropertyCursorItem::setValue()
{
    if ( !comb )
	return;
    if ( QVariant( QCursor( combo()->currentItem() ) ) == val )
	return;
    setText( 1, combo()->currentText() );
    PropertyItem::setValue( QCursor( combo()->currentItem() ) );
    notifyValueChange();
}

// --------------------------------------------------------------

/*!
  \class PropertyList propertyeditor.h
  \brief PropertyList is a QListView derived class which is used for editing widget properties

  This class is used for widget properties. It has to be child of a
  PropertyEditor.

  To initialize it for editing a widget call setupProperties() which
  iterates through the properties of the current widget (see
  PropertyEditor::widget()) and builds the list.

  To update the item values, refetchData() can be called.

  If the value of an item has been changed by the user, and this
  change should be applied to the widget's property, valueChanged()
  has to be called.

  To set the value of an item, setPropertyValue() has to be called.
*/

PropertyList::PropertyList( PropertyEditor *e )
    : Q3ListView( e ), editor( e )
{
    init_colors();

    whatsThis = new PropertyWhatsThis( this );
    showSorted = false;
    header()->setMovingEnabled( false );
    header()->setStretchEnabled( true );
    setResizePolicy( Q3ScrollView::Manual );
    viewport()->setAcceptDrops( true );
    viewport()->installEventFilter( this );
    addColumn( i18n("Property" ) );
    addColumn( i18n("Value" ) );
    connect( header(), SIGNAL( sizeChange( int, int, int ) ),
	     this, SLOT( updateEditorSize() ) );
    disconnect( header(), SIGNAL( sectionClicked( int ) ),
		this, SLOT( changeSortColumn( int ) ) );
    connect( header(), SIGNAL( sectionClicked( int ) ),
	     this, SLOT( toggleSort() ) );
    connect( this, SIGNAL( pressed( Q3ListViewItem *, const QPoint &, int ) ),
	     this, SLOT( itemPressed( Q3ListViewItem *, const QPoint &, int ) ) );
    connect( this, SIGNAL( doubleClicked( Q3ListViewItem * ) ),
	     this, SLOT( toggleOpen( Q3ListViewItem * ) ) );
    setSorting( -1 );
    setHScrollBarMode( AlwaysOff );
    setVScrollBarMode( AlwaysOn );
    setColumnWidthMode( 1, Manual );
    mousePressed = false;
    pressItem = 0;
    header()->installEventFilter( this );
}

void PropertyList::toggleSort()
{
    showSorted = !showSorted;
    editor->clear();
    editor->setup();
}

void PropertyList::resizeEvent( QResizeEvent *e )
{
    Q3ListView::resizeEvent( e );
    if ( currentItem() )
	( ( PropertyItem* )currentItem() )->showEditor();
}

static QVariant::Type type_to_variant( const QString &s )
{
    if ( s == "Invalid " )
	return QVariant::Invalid;
    if ( s == "Map" )
	return QVariant::Map;
    if ( s == "List" )
	return QVariant::List;
    if ( s == "String" )
	return QVariant::String;
    if ( s == "StringList" )
	return QVariant::StringList;
    if ( s == "Font" )
	return QVariant::Font;
    if ( s == "Pixmap" )
	return QVariant::Pixmap;
    if ( s == "Brush" )
	return QVariant::Brush;
    if ( s == "Rect" )
	return QVariant::Rect;
    if ( s == "Size" )
	return QVariant::Size;
    if ( s == "Color" )
	return QVariant::Color;
    if ( s == "Palette" )
	return QVariant::Palette;
    if ( s == "ColorGroup" )
	return QVariant::ColorGroup;
    if ( s == "IconSet" )
	return QVariant::IconSet;
    if ( s == "Point" )
	return QVariant::Point;
    if ( s == "Image" )
	return QVariant::Image;
    if ( s == "Int" )
	return QVariant::Int;
    if ( s == "UInt" )
	return QVariant::UInt;
    if ( s == "Bool" )
	return QVariant::Bool;
    if ( s == "Double" )
	return QVariant::Double;
    if ( s == "CString" )
	return QVariant::CString;
    if ( s == "PointArray" )
	return QVariant::PointArray;
    if ( s == "Region" )
	return QVariant::Region;
    if ( s == "Bitmap" )
	return QVariant::Bitmap;
    if ( s == "Cursor" )
	return QVariant::Cursor;
    if ( s == "SizePolicy" )
	return QVariant::SizePolicy;
    if ( s == "Date" )
	return QVariant::Date;
    if ( s == "Time" )
	return QVariant::Time;
    if ( s == "DateTime" )
	return QVariant::DateTime;
    return QVariant::Invalid;
}

#ifndef QT_NO_SQL
static bool parent_is_data_aware( QObject *o )
{
    if ( !o->inherits( "QWidget" ) )
	return false;
    QWidget *w = (QWidget*)o;
    QWidget *p = w->parentWidget();
    while ( p && !p->isTopLevel() ) {
	if ( p->inherits( "QDesignerDataBrowser" ) || p->inherits( "QDesignerDataView" ) )
	    return true;
	p = p->parentWidget();
    }
    return false;
}
#endif

/*!  Sets up the property list by adding an item for each designable
property of the widget which is just edited.
*/

void PropertyList::setupProperties()
{
    if ( !editor->widget() )
	return;
    bool allProperties = !editor->widget()->inherits( "Spacer" );
    Q3StrList lst = editor->widget()->metaObject()->propertyNames( allProperties );
    PropertyItem *item = 0;
    QMap<QString, bool> unique;
    QObject *w = editor->widget();
    QStringList valueSet;
    bool parentHasLayout =
	w->isWidgetType() &&
	!editor->formWindow()->isMainContainer( (QWidget*)w ) && ( (QWidget*)w )->parentWidget() &&
	WidgetFactory::layoutType( ( (QWidget*)w )->parentWidget() ) != WidgetFactory::NoLayout;
    for ( Q3PtrListIterator<char> it( lst ); it.current(); ++it ) {
	const QMetaProperty* p =
	    editor->widget()->metaObject()->
	    property( editor->widget()->metaObject()->findProperty( it.current(), allProperties), allProperties );
	if ( !p )
	    continue;
	if ( unique.contains( QString::fromLatin1( it.current() ) ) )
	    continue;
	if ( editor->widget()->inherits( "QDesignerToolBar" ) || editor->widget()->inherits( "QDesignerMenuBar" ) ) {
	    if ( qstrcmp( p->name(), "minimumHeight" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "minimumWidth" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "maximumHeight" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "maximumWidth" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "geometry" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "sizePolicy" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "minimumSize" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "maximumSize" ) == 0 )
		continue;
	}
	unique.insert( QString::fromLatin1( it.current() ), true );
	if ( editor->widget()->isWidgetType() &&
	     editor->formWindow()->isMainContainer( (QWidget*)editor->widget() ) ) {
	    if ( qstrcmp( p->name(), "geometry" ) == 0 )
		continue;
	} else { // hide some toplevel-only stuff
	    if ( qstrcmp( p->name(), "icon" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "iconText" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "caption" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "sizeIncrement" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "baseSize" ) == 0 )
		continue;
	    if ( parentHasLayout && qstrcmp( p->name(), "geometry" ) == 0 )
		continue;
	    if ( w->inherits( "QLayoutWidget" ) || w->inherits( "Spacer" ) ) {
		if ( qstrcmp( p->name(), "sizePolicy" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "minimumHeight" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "minimumWidth" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "maximumHeight" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "maximumWidth" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "geometry" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "minimumSize" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "maximumSize" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "enabled" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "paletteForegroundColor" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "paletteBackgroundColor" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "paletteBackgroundPixmap" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "palette" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "font" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "cursor" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "mouseTracking" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "focusPolicy" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "acceptDrops" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "autoMask" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "backgroundOrigin" ) == 0 )
		    continue;
	    }
	}
	if ( w->inherits( "QActionGroup" ) ) {
	    if ( qstrcmp( p->name(), "usesDropDown" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "toggleAction" ) == 0 )
		continue;
	}
	if ( qstrcmp( p->name(), "minimumHeight" ) == 0 )
	    continue;
	if ( qstrcmp( p->name(), "minimumWidth" ) == 0 )
	    continue;
	if ( qstrcmp( p->name(), "maximumHeight" ) == 0 )
	    continue;
	if ( qstrcmp( p->name(), "maximumWidth" ) == 0 )
	    continue;
	if ( qstrcmp( p->name(), "buttonGroupId" ) == 0 ) { // #### remove this when designable in Q_PROPERTY can take a function (isInButtonGroup() in this case)
	    if ( !editor->widget()->isWidgetType() ||
		 !editor->widget()->parent() ||
		 !editor->widget()->parent()->inherits( "QButtonGroup" ) )
		continue;
	}


	if ( p->designable(w) ) {
	    if ( p->isSetType() ) {
		if ( QString( p->name() ) == "alignment" ) {
		    QStringList lst;
		    lst << p->valueToKey( AlignAuto )
			<< p->valueToKey( Qt::AlignLeft )
			<< p->valueToKey( Qt::AlignHCenter )
			<< p->valueToKey( Qt::AlignRight )
			<< p->valueToKey( AlignJustify );
		    item = new PropertyListItem( this, item, 0, "hAlign", false );
		    item->setValue( lst );
		    setPropertyValue( item );
		    if ( MetaDataBase::isPropertyChanged( editor->widget(), "hAlign" ) )
			item->setChanged( true, false );
		    if ( !editor->widget()->inherits( "QMultiLineEdit" ) ) {
			lst.clear();
			lst << p->valueToKey( Qt::AlignTop )
			    << p->valueToKey( Qt::AlignVCenter )
			    << p->valueToKey( Qt::AlignBottom );
			item = new PropertyListItem( this, item, 0, "vAlign", false );
			item->setValue( lst );
			setPropertyValue( item );
			if ( MetaDataBase::isPropertyChanged( editor->widget(), "vAlign" ) )
			    item->setChanged( true, false );
			item = new PropertyBoolItem( this, item, 0, "wordwrap" );
			if ( w->inherits( "QGroupBox" ) )
			    item->setVisible( false );
			setPropertyValue( item );
			if ( MetaDataBase::isPropertyChanged( editor->widget(), "wordwrap" ) )
			    item->setChanged( true, false );
		    }
		} else {
		    qWarning( "Sets except 'alignment' not supported yet.... %s.", p->name() );
		}
	    } else if ( p->isEnumType() ) {
		Q3StrList l = p->enumKeys();
		QStringList lst;
		for ( uint i = 0; i < l.count(); ++i ) {
		    QString k = l.at( i );
		    // filter out enum-masks
		    if ( k[0] == 'M' && k[1].category() == QChar::Letter_Uppercase )
			continue;
		    lst << l.at( i );
		}
		item = new PropertyListItem( this, item, 0, p->name(), false );
		item->setValue( lst );
	    } else {
		QVariant::Type t = QVariant::nameToType( p->type() );
		if ( !addPropertyItem( item, p->name(), t ) )
		    continue;
	    }
	}
	if ( item && !p->isSetType() ) {
	    if ( valueSet.findIndex( item->name() ) == -1 ) {
		setPropertyValue( item );
		valueSet << item->name();
	    }
	    if ( MetaDataBase::isPropertyChanged( editor->widget(), p->name() ) )
		item->setChanged( true, false );
	}
    }

    if ( !w->inherits( "QSplitter" ) && !w->inherits( "QDesignerMenuBar" ) && !w->inherits( "QDesignerToolBar" ) &&
	 w->isWidgetType() && WidgetFactory::layoutType( (QWidget*)w ) != WidgetFactory::NoLayout ) {
	item = new PropertyIntItem( this, item, 0, "layoutSpacing", true );
	setPropertyValue( item );
	item->setChanged( true );
	item = new PropertyIntItem( this, item, 0, "layoutMargin", true );
	setPropertyValue( item );
	item->setChanged( true );
    }


    if ( !w->inherits( "Spacer" ) && !w->inherits( "QLayoutWidget" ) && !w->inherits( "QAction" ) &&
	 !w->inherits( "QDesignerMenuBar" ) && !w->inherits( "QDesignerToolBar" ) ) {
	item = new PropertyTextItem( this, item, 0, "toolTip", true, false );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "toolTip" ) )
	    item->setChanged( true, false );
	item = new PropertyTextItem( this, item, 0, "whatsThis", true, true );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "whatsThis" ) )
	    item->setChanged( true, false );
    }

#ifndef QT_NO_SQL
    if ( !editor->widget()->inherits( "QDataTable" ) && !editor->widget()->inherits( "QDataBrowser" ) &&
	 !editor->widget()->inherits( "QDataView" ) && parent_is_data_aware( editor->widget() ) ) {
	item = new PropertyDatabaseItem( this, item, 0, "database", editor->formWindow()->mainContainer() != w );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "database" ) )
	    item->setChanged( true, false );
    }

    if ( editor->widget()->inherits( "QDataTable" ) || editor->widget()->inherits( "QDataBrowser" ) || editor->widget()->inherits( "QDataView" ) ) {
	item = new PropertyDatabaseItem( this, item, 0, "database", false );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "database" ) )
	    item->setChanged( true, false );
	item = new PropertyBoolItem( this, item, 0, "frameworkCode" );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "frameworkCode" ) )
	    item->setChanged( true, false );
    }
#endif

    if ( w->inherits( "CustomWidget" ) ) {
	MetaDataBase::CustomWidget *cw = ( (CustomWidget*)w )->customWidget();
	if ( cw ) {
	    for ( Q3ValueList<MetaDataBase::Property>::Iterator it = cw->lstProperties.begin(); it != cw->lstProperties.end(); ++it ) {
		if ( unique.contains( QString( (*it).property ) ) )
		    continue;
		unique.insert( QString( (*it).property ), true );
		addPropertyItem( item, (*it).property, type_to_variant( (*it).type ) );
		setPropertyValue( item );
		if ( MetaDataBase::isPropertyChanged( editor->widget(), (*it).property ) )
		    item->setChanged( true, false );
	    }
	}
    }

    setCurrentItem( firstChild() );

    if ( showSorted ) {
	setSorting( 0 );
	sort();
	setSorting( -1 );
	setCurrentItem( firstChild() );
	qApp->processEvents();
    }

    updateEditorSize();
    updateEditorSize();
}

bool PropertyList::addPropertyItem( PropertyItem *&item, const Q3CString &name, QVariant::Type t )
{
    switch ( t ) {
    case QVariant::String:
	item = new PropertyTextItem( this, item, 0, name, true,
				     editor->widget()->inherits( "QLabel" ) || editor->widget()->inherits( "QTextView" ) );
	break;
    case QVariant::CString:
	item = new PropertyTextItem( this, item, 0,
				     name, name == "name" &&
				     editor->widget() == editor->formWindow()->mainContainer(),
				     false, true );
	break;
    case QVariant::Bool:
	item = new PropertyBoolItem( this, item, 0, name );
	break;
    case QVariant::Font:
	item = new PropertyFontItem( this, item, 0, name );
	break;
    case QVariant::Int:
	if ( name == "accel" )
	    item = new PropertyTextItem( this, item, 0, name, false, false, false, true );
	else
	    item = new PropertyIntItem( this, item, 0, name, true );
	break;
    case QVariant::Double:
	item = new PropertyDoubleItem( this, item, 0, name );
	break;
    case QVariant::KeySequence:
	item = new PropertyTextItem( this, item, 0, name, false, false, false, true );
	break;
    case QVariant::UInt:
	item = new PropertyIntItem( this, item, 0, name, false );
	break;
    case QVariant::StringList:
	item = new PropertyListItem( this, item, 0, name, true );
	break;
    case QVariant::Rect:
	item = new PropertyCoordItem( this, item, 0, name, PropertyCoordItem::Rect );
	break;
    case QVariant::Point:
	item = new PropertyCoordItem( this, item, 0, name, PropertyCoordItem::Point );
	break;
    case QVariant::Size:
	item = new PropertyCoordItem( this, item, 0, name, PropertyCoordItem::Size );
	break;
    case QVariant::Color:
	item = new PropertyColorItem( this, item, 0, name, true );
	break;
    case QVariant::Pixmap:
    case QCoreVariant::Icon:
	item = new PropertyPixmapItem( this, item, 0, name, t == QCoreVariant::Icon );
	break;
    case QVariant::SizePolicy:
	item = new PropertySizePolicyItem( this, item, 0, name );
	break;
    case QVariant::Palette:
	item = new PropertyPaletteItem( this, item, 0, name );
	break;
    case QVariant::Cursor:
	item = new PropertyCursorItem( this, item, 0, name );
	break;
    case QVariant::Date:
	item = new PropertyDateItem( this, item, 0, name );
	break;
    case QVariant::Time:
	item = new PropertyTimeItem( this, item, 0, name );
	break;
    case QVariant::DateTime:
	item = new PropertyDateTimeItem( this, item, 0, name );
	break;
    default:
	return false;
    }
    return true;
}

void PropertyList::paintEmptyArea( QPainter *p, const QRect &r )
{
    p->fillRect( r, *backColor2 );
}

void PropertyList::setCurrentItem( Q3ListViewItem *i )
{
    if ( !i )
	return;

    if ( currentItem() )
	( (PropertyItem*)currentItem() )->hideEditor();
    Q3ListView::setCurrentItem( i );
    ( (PropertyItem*)currentItem() )->showEditor();
}

void PropertyList::updateEditorSize()
{
    if ( currentItem() )
	( ( PropertyItem* )currentItem() )->showEditor();
}

/*!  This has to be called if the value if \a i should be set as
  property to the currently edited widget.
*/

void PropertyList::valueChanged( PropertyItem *i )
{
    if ( !editor->widget() )
	return;
    QString pn( i18n("Set '%1' of '%2'", i->name(), editor->widget()->name() ) );
    SetPropertyCommand *cmd = new SetPropertyCommand( pn, editor->formWindow(),
						      editor->widget(), editor,
						      i->name(), WidgetFactory::property( editor->widget(), i->name() ),
						      i->value(), i->currentItem(), i->currentItemFromObject() );
    cmd->execute();
    editor->formWindow()->commandHistory()->addCommand( cmd, true );
}

void PropertyList::itemPressed( Q3ListViewItem *i, const QPoint &p, int c )
{
    if ( !i )
	return;
    PropertyItem *pi = (PropertyItem*)i;
    if ( !pi->hasSubItems() )
	return;

    if ( c == 0 && viewport()->mapFromGlobal( p ).x() < 20 )
	toggleOpen( i );
}

void PropertyList::toggleOpen( Q3ListViewItem *i )
{
    if ( !i )
	return;
    PropertyItem *pi = (PropertyItem*)i;
    if ( pi->hasSubItems() ) {
	pi->setOpen( !pi->isOpen() );
    } else {
	pi->toggle();
    }
}

bool PropertyList::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e )
	return true;

    PropertyItem *i = (PropertyItem*)currentItem();
    if ( o != this &&e->type() == QEvent::KeyPress ) {
	QKeyEvent *ke = (QKeyEvent*)e;
	if ( ( ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down ) &&
	     ( o != this || o != viewport() ) &&
	     !( ke->state() & Qt::ControlModifier ) ) {
	    QApplication::sendEvent( this, (QKeyEvent*)e );
	    return true;
	} else if ( ( !o->inherits( "QLineEdit" ) ||
		      ( o->inherits( "QLineEdit" ) && ( (QLineEdit*)o )->isReadOnly() ) ) &&
		    i && i->hasSubItems() ) {
	    if ( !i->isOpen() &&
		 ( ke->key() == Qt::Key_Plus ||
		   ke->key() == Qt::Key_Right ))
		i->setOpen( true );
	    else if ( i->isOpen() &&
		      ( ke->key() == Qt::Key_Minus ||
			ke->key() == Qt::Key_Left ) )
		i->setOpen( false );
	} else if ( ( ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter ) && o->inherits( "QComboBox" ) ) {
	    QKeyEvent ke2( QEvent::KeyPress, Qt::Key_Space, 0, 0 );
	    QApplication::sendEvent( o, &ke2 );
	    return true;
	}
    } else if ( e->type() == QEvent::FocusOut && o->inherits( "QLineEdit" ) && editor->formWindow() ) {
	QTimer::singleShot( 100, editor->formWindow()->commandHistory(), SLOT( checkCompressedCommand() ) );
    } else if ( o == viewport() ) {
	QMouseEvent *me;
	PropertyListItem* i;
	switch ( e->type() ) {
	case QEvent::MouseButtonPress:
	    me = (QMouseEvent*)e;
	    i = (PropertyListItem*) itemAt( me->pos() );
	    if( i  && ( i->inherits("PropertyColorItem") || i->inherits("PropertyPixmapItem") ) ) {
		pressItem = i;
		pressPos = me->pos();
		mousePressed = true;
	    }
	    break;
	case QEvent::MouseMove:
	    me = (QMouseEvent*)e;
	    if ( me && me->state() & LeftButton && mousePressed) {

		i = (PropertyListItem*) itemAt( me->pos() );
		if( i  && i == pressItem ) {

		    if(( pressPos - me->pos() ).manhattanLength() > QApplication::startDragDistance() ){
			if ( i->inherits("PropertyColorItem") ) {
			    QColor col = i->value().asColor();
			    Q3ColorDrag *drg = new Q3ColorDrag( col, this );
			    QPixmap pix( 25, 25 );
			    pix.fill( col );
			    QPainter p( &pix );
			    p.drawRect( 0, 0, pix.width(), pix.height() );
			    p.end();
			    drg->setPixmap( pix );
			    mousePressed = false;
			    drg->dragCopy();
			}
			else if ( i->inherits("PropertyPixmapItem") ) {
			    QPixmap pix = i->value().asPixmap();
			    if( !pix.isNull() ) {
				QImage img = pix.convertToImage();
				Q3ImageDrag *drg = new Q3ImageDrag( img, this );
				drg->setPixmap( pix );
				mousePressed = false;
				drg->dragCopy();
			    }
			}
		    }
		}
	    }
	    break;
	default:
	    break;
	}
    } else if ( o == header() ) {
	if ( e->type() == QEvent::ContextMenu ) {
	    ((QContextMenuEvent *)e)->accept();
	    Q3PopupMenu menu( 0 );
	    menu.setCheckable( true );
	    const int cat_id = 1;
	    const int alpha_id = 2;
	    menu.insertItem( i18n("Sort &Categorized" ), cat_id );
	    int alpha = menu.insertItem( i18n("Sort &Alphabetically" ), alpha_id );
	    if ( showSorted )
		menu.setItemChecked( alpha_id, true );
	    else
		menu.setItemChecked( cat_id, true );
	    int res = menu.exec( ( (QContextMenuEvent*)e )->globalPos() );
	    if ( res != -1 ) {
		bool newShowSorted = ( res == alpha );
		if ( showSorted != newShowSorted ) {
		    showSorted = newShowSorted;
		    editor->clear();
		    editor->setup();
		}
	    }
	    return true;
	}
    }

    return Q3ListView::eventFilter( o, e );
}

/*!  This method re-initializes each item of the property list.
*/

void PropertyList::refetchData()
{
    Q3ListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	PropertyItem *i = (PropertyItem*)it.current();
	if ( !i->propertyParent() )
	    setPropertyValue( i );
	if ( i->hasSubItems() )
	    i->initChildren();
	bool changed = MetaDataBase::isPropertyChanged( editor->widget(), i->name() );
	if ( changed != i->isChanged() )
	    i->setChanged( changed, false );
    }
    updateEditorSize();
}

static void clearAlignList( Q3StrList &l )
{
    if ( l.count() == 1 )
	return;
    if ( l.find( "AlignAuto" ) != -1 )
	l.remove( "AlignAuto" );
    if ( l.find( "WordBreak" ) != -1 )
	l.remove( "WordBreak" );
}

/*!  This method initializes the value of the item \a i to the value
  of the corresponding property.
*/

void PropertyList::setPropertyValue( PropertyItem *i )
{
    const QMetaProperty *p =
	editor->widget()->metaObject()->
	property( editor->widget()->metaObject()->findProperty( i->name(), true), true );
    if ( !p ) {
	if ( i->name() == "hAlign" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    p = editor->widget()->metaObject()->
		property( editor->widget()->metaObject()->findProperty( "alignment", true ), true );
	    align &= ~AlignVertical_Mask;
	    Q3StrList l = p->valueToKeys( align );
	    clearAlignList( l );
	    ( (PropertyListItem*)i )->setCurrentItem( l.last() );
	} else if ( i->name() == "vAlign" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    p = editor->widget()->metaObject()->
		property( editor->widget()->metaObject()->findProperty( "alignment", true ), true );
	    align &= ~AlignHorizontal_Mask;
	    ( (PropertyListItem*)i )->setCurrentItem( p->valueToKeys( align ).last() );
	} else if ( i->name() == "wordwrap" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    if ( align & WordBreak )
		i->setValue( QVariant( true, 0 ) );
	    else
		i->setValue( QVariant( false, 0 ) );
	} else if ( i->name() == "layoutSpacing" ) {
	    ( (PropertyIntItem*)i )->setValue( MetaDataBase::spacing( WidgetFactory::containerOfWidget( (QWidget*)editor->widget() ) ) );
	} else if ( i->name() == "layoutMargin" ) {
	    ( (PropertyIntItem*)i )->setValue( MetaDataBase::margin( WidgetFactory::containerOfWidget( (QWidget*)editor->widget() ) ) );
	} else if ( i->name() == "toolTip" || i->name() == "whatsThis" || i->name() == "database" || i->name() == "frameworkCode" ) {
	    i->setValue( MetaDataBase::fakeProperty( editor->widget(), i->name() ) );
	} else if ( editor->widget()->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)editor->widget() )->customWidget();
	    if ( !cw )
		return;
	    i->setValue( MetaDataBase::fakeProperty( editor->widget(), i->name() ) );
	}
	return;
    }
    if ( p->isSetType() )
	;
    else if ( p->isEnumType() )
	( (PropertyListItem*)i )->setCurrentItem( p->valueToKey( editor->widget()->property( i->name() ).toInt() ) );
    else
	i->setValue( editor->widget()->property( i->name() ) );
}

void PropertyList::setCurrentProperty( const QString &n )
{
    if ( currentItem() && currentItem()->text( 0 ) == n ||
	 currentItem() && ( (PropertyItem*)currentItem() )->propertyParent() &&
	 ( (PropertyItem*)currentItem() )->propertyParent()->text( 0 ) == n )
	return;

    Q3ListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	if ( it.current()->text( 0 ) == n ) {
	    setCurrentItem( it.current() );
	    break;
	}
    }
}

PropertyEditor *PropertyList::propertyEditor() const
{
    return editor;
}

void PropertyList::resetProperty()
{
    if ( !currentItem() )
	return;
    PropertyItem *i = (PropertyItem*)currentItem();
    if ( !MetaDataBase::isPropertyChanged( editor->widget(), i->PropertyItem::name() ) )
	return;
    QString pn( i18n("Reset '%1' of '%2'", i->name(), editor->widget()->name() ) );
    SetPropertyCommand *cmd = new SetPropertyCommand( pn, editor->formWindow(),
						      editor->widget(), editor,
						      i->name(), i->value(),
						      WidgetFactory::defaultValue( editor->widget(), i->name() ),
						      WidgetFactory::defaultCurrentItem( editor->widget(), i->name() ),
						      i->currentItem(), true );
    cmd->execute();
    editor->formWindow()->commandHistory()->addCommand( cmd, false );
    if ( i->hasSubItems() )
	i->initChildren();
}

void PropertyList::viewportDragEnterEvent( QDragEnterEvent *e )
{
    PropertyListItem *i = (PropertyListItem*) itemAt( e->pos() );
    if( !i ) {
	e->ignore();
	return;
    }

    if ( i->inherits("PropertyColorItem") && Q3ColorDrag::canDecode( e ) )
	e->accept();
    else if ( i->inherits("PropertyPixmapItem") && Q3ImageDrag::canDecode( e ) )
	e->accept();
    else
	e->ignore();
}

void PropertyList::viewportDragMoveEvent ( QDragMoveEvent *e )
{
    PropertyListItem *i = (PropertyListItem*) itemAt( e->pos() );
    if( !i ) {
	e->ignore();
	return;
    }

    if ( i->inherits("PropertyColorItem") && Q3ColorDrag::canDecode( e ) )
	e->accept();
    else if ( i->inherits("PropertyPixmapItem") && Q3ImageDrag::canDecode( e ) )
	e->accept();
    else
	e->ignore();
}

void PropertyList::viewportDropEvent ( QDropEvent *e )
{
    PropertyListItem *i = (PropertyListItem*) itemAt( e->pos() );
    if( !i ) {
	e->ignore();
	return;
    }

    if ( i->inherits("PropertyColorItem") && Q3ColorDrag::canDecode( e ) ) {
	QColor color;
	Q3ColorDrag::decode( e, color );
	i->setValue( QVariant( color ) );
	valueChanged( i );
	e->accept();
    }
    else if ( i->inherits("PropertyPixmapItem")  && Q3ImageDrag::canDecode( e ) ) {
	QImage img;
	Q3ImageDrag::decode( e, img );
	QPixmap pm;
	pm.convertFromImage( img );
	i->setValue( QVariant( pm ) );
	valueChanged( i );
	e->accept();
    }
    else
	e->ignore();
}

QString PropertyList::whatsThisAt( const QPoint &p )
{
    return whatsThisText( itemAt( p ) );
}

void PropertyList::showCurrentWhatsThis()
{
    if ( !currentItem() )
	return;
    QPoint p( 0, currentItem()->itemPos() );
    p = viewport()->mapToGlobal( contentsToViewport( p ) );
    Q3WhatsThis::display( whatsThisText( currentItem() ), p, viewport() );
}

QString PropertyList::whatsThisText( Q3ListViewItem *i )
{
    if ( !i || !editor->widget() )
	return QString();
    readPropertyDocs();
    if ( ( (PropertyItem*)i )->propertyParent() )
	i = ( (PropertyItem*)i )->propertyParent();

    const QMetaObject *mo = editor->widget()->metaObject();
    QString prop = ( (PropertyItem*)i )->name();
    while ( mo ) {
	QString s;
	s = QString( mo->className() ) + "::" + prop;
	QMap<QString, QString>::Iterator it;
	if ( ( it = propertyDocs.find( s ) ) != propertyDocs.end() ) {
	    return *it;
	}
	mo = mo->superClass();
    }

    return i18n("<p><b>QWidget::%1</b></p><p>There is no documentation available for this property.</p>", prop );
}

void PropertyList::readPropertyDocs()
{
    if ( !propertyDocs.isEmpty() )
	return;

    QString docFile = MainWindow::self->documentationPath() + "/propertydocs";
    QFile f( docFile );
    if ( !f.open( QIODevice::ReadOnly ) )
	return;
    QDomDocument doc;
    QString errMsg;
    int errLine;
    if ( !doc.setContent( &f, &errMsg, &errLine ) )
	return;
    QDomElement e = doc.firstChild().toElement().firstChild().toElement();

    for ( ; !e.isNull(); e = e.nextSibling().toElement() ) {
	QDomElement n = e.firstChild().toElement();
	QString name;
	QString doc;
	for ( ; !n.isNull(); n = n.nextSibling().toElement() ) {
	    if ( n.tagName() == "name" )
		name = n.firstChild().toText().data();
	    else if ( n.tagName() == "doc" )
		doc = n.firstChild().toText().data();
	}
	doc.insert( 0, "<p><b>" + name + "</b></p>" );
	propertyDocs.insert( name, doc );
    }
}

// ------------------------------------------------------------

#ifndef KOMMANDER
EventList::EventList( QWidget *parent, FormWindow *fw, PropertyEditor *e )
    : HierarchyList( parent, fw, true ), editor( e )
{
    header()->hide();
    removeColumn( 1 );
    setRootIsDecorated( true );
    connect( this, SIGNAL( itemRenamed( Q3ListViewItem *, int, const QString & ) ),
	     this, SLOT( renamed( Q3ListViewItem * ) ) );
}

void EventList::setup()
{
    clear();

#ifndef KOMMANDER
    if ( MetaDataBase::hasEvents( formWindow->project()->language() ) ) {
#else
    if ( MetaDataBase::hasEvents( "C++" ) ) {
#endif
	Q3ValueList<MetaDataBase::EventDescription> events =
#ifndef KOMMANDER
	    MetaDataBase::events( editor->widget(), formWindow->project()->language() );
#else
	    MetaDataBase::events( editor->widget(), "C++" );
#endif
	if ( events.isEmpty() )
	    return;
	for ( Q3ValueList<MetaDataBase::EventDescription>::Iterator it = events.begin(); it != events.end(); ++it ) {
	    HierarchyItem *eventItem = new HierarchyItem( HierarchyItem::Event, this, (*it).name,
							  QString(), QString() );
	    eventItem->setOpen( true );
	    QStringList funcs = MetaDataBase::eventFunctions( editor->widget(),
							      (*it).name,
#ifndef KOMMANDER
							      formWindow->project()->language() );
#else
							      "C++" );
#endif
	    for ( QStringList::Iterator fit = funcs.begin(); fit != funcs.end(); ++fit ) {
		HierarchyItem *item = new HierarchyItem( HierarchyItem::EventFunction, eventItem,
							 *fit, QString(), QString() );
		item->setPixmap( 0, PixmapChooser::loadPixmap( "editslots.xpm" ) );
	    }
#if 0 // ### for conversation from old to new
	    if ( !funcs.isEmpty() )
		save( eventItem );
#endif
	}
    } else {
	Q3StrList sigs = editor->widget()->metaObject()->signalNames( true );
	sigs.remove( "destroyed()" );
	QStrListIterator it( sigs );
	while ( it.current() ) {
	    HierarchyItem *eventItem = new HierarchyItem( HierarchyItem::Event, this,
							  it.current(), QString(), QString() );
	    eventItem->setOpen( true );
	    Q3ValueList<MetaDataBase::Connection> conns =
		MetaDataBase::connections( formWindow, editor->widget(), formWindow->mainContainer() );
	    for ( Q3ValueList<MetaDataBase::Connection>::Iterator cit = conns.begin(); cit != conns.end(); ++cit ) {
		if ( MetaDataBase::normalizeSlot( QString( (*cit).signal ) ) !=
		     MetaDataBase::normalizeSlot( QString( it.current() ) ) )
		    continue;
		HierarchyItem *item = new HierarchyItem( HierarchyItem::EventFunction, eventItem,
							 (*cit).slot, QString(), QString() );
		item->setPixmap( 0, PixmapChooser::loadPixmap( "editslots.xpm" ) );
	    }
	    ++it;
	}
    }
}

extern Q3ListViewItem *newItem;

void EventList::contentsMouseDoubleClickEvent( QMouseEvent *e )
{
    Q3ListViewItem *i = itemAt( contentsToViewport( e->pos() ) );
    if ( !i || i->parent() )
	return;
    QString s;
#ifndef KOMMANDER
    if ( MetaDataBase::hasEvents( formWindow->project()->language() ) ) {
#else
    if ( MetaDataBase::hasEvents( "C++" ) ) {
#endif
	QString s1 = i->text( 0 );
	int pt = s1.find( "(" );
	if ( pt != -1 )
	    s1 = s1.left( pt );
	s = QString( editor->widget()->name() ) + "_" + s1;
    } else {
	s = QString( editor->widget()->name() ) + "_" + i->text( 0 );
    }

    insertEntry( i, PixmapChooser::loadPixmap( "editslots.xpm" ), s );
}

void EventList::setCurrent( QWidget * )
{
}

void EventList::objectClicked( Q3ListViewItem *i )
{
    if ( !i || !i->parent() )
	return;
    formWindow->mainWindow()->editFunction( i->text( 0 ) );
}

void EventList::showRMBMenu( Q3ListViewItem *i, const QPoint &pos )
{
    if ( !i )
	return;
    Q3PopupMenu menu;
    const int NEW_ITEM = 1;
    const int DEL_ITEM = 2;
    menu.insertItem( PixmapChooser::loadPixmap( "document-new" ), i18n("New Signal Handler" ), NEW_ITEM );
    menu.insertItem( PixmapChooser::loadPixmap( "edit-cut" ), i18n("Delete Signal Handler" ), DEL_ITEM );
    int res = menu.exec( pos );
    if ( res == NEW_ITEM ) {
	QString s;
#ifndef KOMMANDER
	if ( MetaDataBase::hasEvents( formWindow->project()->language() ) ) {
#else
	if ( MetaDataBase::hasEvents( "C++" ) ) {
#endif
	    QString s1 = ( i->parent() ? i->parent() : i )->text( 0 );
	    int pt = s1.find( "(" );
	    if ( pt != -1 )
		s1 = s1.left( pt );
	    s = QString( editor->widget()->name() ) + "_" + s1;
	} else {
	    s = QString( editor->widget()->name() ) + "_" + ( i->parent() ? i->parent() : i )->text( 0 );
	}
	insertEntry( i->parent() ? i->parent() : i, PixmapChooser::loadPixmap( "editslots.xpm" ), s );
    } else if ( res == DEL_ITEM && i->parent() ) {
#ifndef KOMMANDER
	if ( MetaDataBase::hasEvents( formWindow->project()->language() ) ) {
#else
	if ( MetaDataBase::hasEvents( "C++" ) ) {
#endif
	    Q3ListViewItem *p = i->parent();
	    delete i;
	    save( p );
	} else {
	    MetaDataBase::Connection conn;
	    conn.sender = editor->widget();
	    conn.receiver = formWindow->mainContainer();
	    conn.signal = i->parent()->text( 0 );
	    conn.slot = i->text( 0 );
	    delete i;
	    RemoveConnectionCommand *cmd = new RemoveConnectionCommand( i18n("Remove Connection" ),
									formWindow,
									conn );
	    formWindow->commandHistory()->addCommand( cmd );
	    cmd->execute();
#ifndef KOMMANDER
		editor->formWindow()->mainWindow()->objectHierarchy()->updateFormDefinitionView();
#endif
	}
    }
}

void EventList::renamed( Q3ListViewItem *i )
{
    if ( newItem == i )
	newItem = 0;
    if ( !i->parent() )
	return;
    Q3ListViewItem *itm = i->parent()->firstChild();
    bool del = false;
    while ( itm ) {
	if ( itm != i && itm->text( 0 ) == i->text( 0 ) ) {
	    del = true;
	    break;
	}
	itm = itm->nextSibling();
    }
    i->setRenameEnabled( 0, false );
    if ( del ) {
	delete i;
    } else {
#ifndef KOMMANDER
	if ( MetaDataBase::hasEvents( formWindow->project()->language() ) ) {
#else
	if ( MetaDataBase::hasEvents( "C++" ) ) {
#endif

	    save( i->parent() );
	    editor->formWindow()->mainWindow()->
#ifndef KOMMANDER
		editFunction( i->text( 0 ), editor->formWindow()->project()->language(), true );
#else
		editFunction( i->text( 0 ), "C++", true );
#endif
	} else {
	    MetaDataBase::Connection conn;
	    conn.sender = editor->widget();
	    conn.receiver = formWindow->mainContainer();
	    conn.signal = i->parent()->text( 0 );
	    conn.slot = i->text( 0 );
	    AddConnectionCommand *cmd = new AddConnectionCommand( i18n("Add Connection" ),
								  formWindow,
								  conn );
	    formWindow->commandHistory()->addCommand( cmd );
	    // #### we should look if the specified slot already
	    // exists and if we can connect to this one
#ifndef KOMMANDER
	    MetaDataBase::addSlot( formWindow, i->text( 0 ).toLatin1(), "virtual", "public",
				   formWindow->project()->language(), "void" );
#else
	    MetaDataBase::addSlot( formWindow, i->text( 0 ).toLatin1(), "virtual", "public",
				   "C++", "void" );
#endif
	    editor->formWindow()->mainWindow()->
		editFunction( i->text( 0 ).left( i->text( 0 ).find( "(" ) ),
#ifndef KOMMANDER
			      editor->formWindow()->project()->language(), true );
#else
			      "C++", true );
#endif

	    cmd->execute();
#ifndef KOMMANDER
		editor->formWindow()->mainWindow()->objectHierarchy()->updateFormDefinitionView();
#endif
	}
    }
}

void EventList::save( Q3ListViewItem *p )
{
    QStringList lst;
    Q3ListViewItem *i = p->firstChild();
    while ( i ) {
	lst << i->text( 0 );
	i = i->nextSibling();
    }

#ifndef KOMMANDER
    if ( MetaDataBase::hasEvents( formWindow->project()->language() ) ) {
	if ( MetaDataBase::setEventFunctions( editor->widget(), formWindow,
					      formWindow->project()->language(), p->text( 0 ), lst ) )
			editor->formWindow()->mainWindow()->objectHierarchy()->updateFormDefinitionView();
    }
#endif
}
#endif

// --------------------------------------------------------------

/*!
  \class PropertyEditor propertyeditor.h
  \brief PropertyEdior toplevel window

  This is the toplevel window of the property editor which contains a
  listview for editing properties.
*/

PropertyEditor::PropertyEditor( QWidget *parent )
    : QTabWidget( parent, 0, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title |
		  Qt::WStyle_StaysOnTop | Qt::WStyle_Tool |Qt::WStyle_MinMax | Qt::WStyle_SysMenu )
{
    setCaption( i18n("Property Editor" ) );
    setIcon( PixmapChooser::loadPixmap( "logo" ) );
    wid = 0;
    formwindow = 0;
    listview = new PropertyList( this );
    addTab( listview, i18n("P&roperties" ) );
#ifndef KOMMANDER
    eList = new EventList( this, formWindow(), this );
    addTab( eList, i18n("S&ignal Handlers" ) );
#endif
}

QObject *PropertyEditor::widget() const
{
    return wid;
}

void PropertyEditor::setWidget( QObject *w, FormWindow *fw )
{
#ifndef KOMMANDER
    eList->setFormWindow( fw );
#endif
    if ( w && w == wid ) {
	bool ret = true;
	if ( wid->isWidgetType() && WidgetFactory::layoutType( (QWidget*)wid ) != WidgetFactory::NoLayout ) {
	    Q3ListViewItemIterator it( listview );
	    ret = false;
	    while ( it.current() ) {
		if ( it.current()->text( 0 ) == "layoutSpacing" || it.current()->text( 0 ) == "layoutMargin" ) {
		    ret = true;
		    break;
		}
		++it;
	    }
	}
	if ( ret )
	    return;
    }

    if ( !w || !fw ) {
	setCaption( i18n("Property Editor" ) );
	clear();
	wid = 0;
	formwindow = 0;
	return;
    }

    wid = w;
    formwindow = fw;
    setCaption( i18n("Property Editor (%1)", formwindow->name() ) );
    listview->viewport()->setUpdatesEnabled( false );
    listview->setUpdatesEnabled( false );
    clear();
    listview->viewport()->setUpdatesEnabled( true );
    listview->setUpdatesEnabled( true );
    setup();
}

void PropertyEditor::clear()
{
    listview->setContentsPos( 0, 0 );
    listview->clear();
#ifndef KOMMANDER
    eList->setContentsPos( 0, 0 );
    eList->clear();
#endif
}

void PropertyEditor::setup()
{
    if ( !formwindow || !wid )
	return;
    listview->viewport()->setUpdatesEnabled( false );
    listview->setupProperties();
    listview->viewport()->setUpdatesEnabled( true );
    listview->updateEditorSize();

#ifndef KOMMANDER
    eList->viewport()->setUpdatesEnabled( false );
    eList->setup();
    eList->viewport()->setUpdatesEnabled( true );
#endif
}

void PropertyEditor::refetchData()
{
    listview->refetchData();
}

void PropertyEditor::emitWidgetChanged()
{
    if ( formwindow && wid )
	formwindow->widgetChanged( wid );
}

void PropertyEditor::closed( FormWindow *w )
{
    if ( w == formwindow ) {
	formwindow = 0;
	wid = 0;
	clear();
    }
}

void PropertyEditor::closeEvent( QCloseEvent *e )
{
    emit hidden();
    e->accept();
}

PropertyList *PropertyEditor::propertyList() const
{
    return listview;
}

FormWindow *PropertyEditor::formWindow() const
{
    return formwindow;
}

QString PropertyEditor::currentProperty() const
{
    if ( !wid )
	return QString();
    if ( ( (PropertyItem*)listview->currentItem() )->propertyParent() )
	return ( (PropertyItem*)listview->currentItem() )->propertyParent()->name();
    return ( (PropertyItem*)listview->currentItem() )->name();
}

QString PropertyEditor::classOfCurrentProperty() const
{
    if ( !wid )
	return QString();
    QObject *o = wid;
    QString curr = currentProperty();
    QMetaObject *mo = o->metaObject();
    while ( mo ) {
	Q3StrList props = mo->propertyNames( false );
	if ( props.find( curr.toLatin1() ) != -1 )
	    return mo->className();
	mo = mo->superClass();
    }
    return QString();
}

QMetaObject* PropertyEditor::metaObjectOfCurrentProperty() const
{
    if ( !wid )
	return 0;
    return wid->metaObject();
}

void PropertyEditor::resetFocus()
{
    if ( listview->currentItem() )
	( (PropertyItem*)listview->currentItem() )->showEditor();
}

#ifndef KOMMANDER
EventList *PropertyEditor::eventList() const
{
    return eList;
}
#endif
#include "propertyeditor.moc"
