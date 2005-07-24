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

#include <qvariant.h>  // HP-UX compiler needs this here
#include "styledbutton.h"
#include "formwindow.h"
#include "pixmapchooser.h"
#include <kcolordialog.h>
#include <qpalette.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qdragobject.h>
#include <qstyle.h>

StyledButton::StyledButton(QWidget* parent, const char* name)
    : QButton( parent, name ), pix( 0 ), spix( 0 ), s( 0 ), formWindow( 0 ), mousePressed( false )
{
    setMinimumSize( minimumSizeHint() );
    setAcceptDrops( true );

    connect( this, SIGNAL(clicked()), SLOT(onEditor()));

    setEditor( ColorEditor );
}

StyledButton::StyledButton( const QBrush& b, QWidget* parent, const char* name, WFlags f )
    : QButton( parent, name, f ), spix( 0 ), s( 0 ), formWindow( 0 )
{
    col = b.color();
    pix = b.pixmap();
    setMinimumSize( minimumSizeHint() );
}

StyledButton::~StyledButton()
{
}

void StyledButton::setEditor( EditorType e )
{
    if ( edit == e )
	return;

    edit = e;
    update();
}

StyledButton::EditorType StyledButton::editor() const
{
    return edit;
}

void StyledButton::setColor( const QColor& c )
{
    col = c;
    update();
}

void StyledButton::setPixmap( const QPixmap & pm )
{
    if ( !pm.isNull() ) {
	delete pix;
	pix = new QPixmap( pm );
    } else {
	delete pix;
	pix = 0;
    }
    scalePixmap();
}

QColor StyledButton::color() const
{
    return col;
}

QPixmap* StyledButton::pixmap() const
{
    return pix;
}

bool StyledButton::scale() const
{
    return s;
}

void StyledButton::setScale( bool on )
{
    if ( s == on )
	return;

    s = on;
    scalePixmap();
}

QSize StyledButton::sizeHint() const
{
    return QSize( 50, 25 );
}

QSize StyledButton::minimumSizeHint() const
{
    return QSize( 50, 25 );
}

void StyledButton::scalePixmap()
{
    delete spix;

    if ( pix ) {
	spix = new QPixmap( 6*width()/8, 6*height()/8 );
	QImage img = pix->convertToImage();

	spix->convertFromImage( s? img.smoothScale( 6*width()/8, 6*height()/8 ) : img );
    } else {
	spix = 0;
    }

    update();
}

void StyledButton::resizeEvent( QResizeEvent* e )
{
    scalePixmap();
    QButton::resizeEvent( e );
}

void StyledButton::drawButton( QPainter *paint )
{
    style().drawPrimitive(QStyle::PE_ButtonBevel, paint, rect(), colorGroup(),
			  isDown() ? QStyle::Style_Sunken : QStyle::Style_Raised);
    drawButtonLabel(paint);

    if (hasFocus())
	style().drawPrimitive(QStyle::PE_FocusRect, paint,
			      style().subRect(QStyle::SR_PushButtonFocusRect, this),
			      colorGroup(), QStyle::Style_Default);
}

void StyledButton::drawButtonLabel( QPainter *paint )
{
    QColor pen = isEnabled() ?
		 hasFocus() ? palette().active().buttonText() : palette().inactive().buttonText()
		 : palette().disabled().buttonText();
    paint->setPen( pen );

    if(!isEnabled()) {
	paint->setBrush( QBrush( colorGroup().button() ) );
    }
    else if ( edit == PixmapEditor && spix ) {
	paint->setBrush( QBrush( col, *spix ) );
	paint->setBrushOrigin( width()/8, height()/8 );
    } else
	paint->setBrush( QBrush( col ) );

    paint->drawRect( width()/8, height()/8, 6*width()/8, 6*height()/8 );
}

void StyledButton::onEditor()
{
    switch (edit) {
    case ColorEditor: {
        QColor c = palette().active().background();
	if ( KColorDialog::getColor( c, this ) == QDialog::Accepted ) {
	    setColor( c );
	    emit changed();
	}
    } break;
    case PixmapEditor: {
	QPixmap p;
        if ( pixmap() )
		p = qChoosePixmap( this, formWindow, *pixmap() );
        else
		p = qChoosePixmap( this, formWindow, QPixmap() );
	if ( !p.isNull() ) {
	    setPixmap( p );
	    emit changed();
	}
    } break;
    default:
	break;
    }
}

void StyledButton::mousePressEvent(QMouseEvent* e)
{
    QButton::mousePressEvent(e);
    mousePressed = true;
    pressPos = e->pos();
}

void StyledButton::mouseMoveEvent(QMouseEvent* e)
{
    QButton::mouseMoveEvent( e );
#ifndef QT_NO_DRAGANDDROP
    if ( !mousePressed )
	return;
    if ( ( pressPos - e->pos() ).manhattanLength() > QApplication::startDragDistance() ) {
	if ( edit == ColorEditor ) {
	    QColorDrag *drg = new QColorDrag( col, this );
	    QPixmap pix( 25, 25 );
	    pix.fill( col );
	    QPainter p( &pix );
	    p.drawRect( 0, 0, pix.width(), pix.height() );
	    p.end();
	    drg->setPixmap( pix );
	    mousePressed = false;
	    drg->dragCopy();
	}
	else if ( edit == PixmapEditor && pix && !pix->isNull() ) {
	    QImage img = pix->convertToImage();
	    QImageDrag *drg = new QImageDrag( img, this );
	    if(spix)
		drg->setPixmap( *spix );
	    mousePressed = false;
	    drg->dragCopy();
	}
    }
#endif
}

#ifndef QT_NO_DRAGANDDROP
void StyledButton::dragEnterEvent( QDragEnterEvent *e )
{
    setFocus();
    if ( edit == ColorEditor && QColorDrag::canDecode( e ) )
	e->accept();
    else if ( edit == PixmapEditor && QImageDrag::canDecode( e ) )
	e->accept();
    else
	e->ignore();
}

void StyledButton::dragLeaveEvent( QDragLeaveEvent * )
{
    if ( hasFocus() )
	parentWidget()->setFocus();
}

void StyledButton::dragMoveEvent( QDragMoveEvent *e )
{
    if ( edit == ColorEditor && QColorDrag::canDecode( e ) )
	e->accept();
    else if ( edit == PixmapEditor && QImageDrag::canDecode( e ) )
	e->accept();
    else
	e->ignore();
}

void StyledButton::dropEvent( QDropEvent *e )
{
    if ( edit == ColorEditor && QColorDrag::canDecode( e ) ) {
	QColor color;
	QColorDrag::decode( e, color );
	setColor(color);
	emit changed();
	e->accept();
    }
    else if ( edit == PixmapEditor && QImageDrag::canDecode( e ) ) {
	QImage img;
	QImageDrag::decode( e, img );
	QPixmap pm;
	pm.convertFromImage(img);
	setPixmap(pm);
	emit changed();
	e->accept();
    } else {
	e->ignore();
    }
}
#endif // QT_NO_DRAGANDDROP
#include "styledbutton.moc"
