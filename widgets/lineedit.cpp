/***************************************************************************
                          lineedit.cpp - Lineedit widget 
                             -------------------
    copyright            : (C) 2002 by Marc Britton
    email                : consume@optusnet.com.au
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* QT INCLUDES */
#include <qlayout.h>
#include <qevent.h>
#include <qlineedit.h>

/* OTHER INCLUDES */
#include "lineedit.h"

LineEdit::LineEdit(QWidget *a_parent, const char *a_name)
	: QLineEdit(a_parent, a_name), KommanderWidget((QObject *)this)
{

	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

	connect(this, SIGNAL(textChanged(const QString &)), this, SIGNAL(widgetTextChanged(const QString &)));
}

void LineEdit::showEvent( QShowEvent *e )
{
    QLineEdit::showEvent( e );
    emit widgetOpened();
}

QString LineEdit::currentState() const
{
	return QString("default");
}

LineEdit::~LineEdit()
{
}

bool LineEdit::isKommanderWidget() const
{
	return TRUE;
}

QStringList LineEdit::associatedText() const
{
	return KommanderWidget::associatedText();
}

void LineEdit::setAssociatedText(const QStringList& a_at)
{
	KommanderWidget::setAssociatedText(a_at);
}

void LineEdit::setPopulationText(const QString& a_text)
{
    KommanderWidget::setPopulationText( a_text );
}

QString LineEdit::populationText() const
{
    return KommanderWidget::populationText();
}

void LineEdit::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
    setWidgetText( txt );
}

QString LineEdit::widgetText() const
{
	return text();
}

void LineEdit::setSelectedWidgetText(const QString& a_text)
{
    int f = text().find( a_text );
    if( f != -1 )
	setSelection( f, a_text.length() );
}

QString LineEdit::selectedWidgetText() const
{
    return selectedText();
}

void LineEdit::setWidgetText(const QString& a_text)
{
	setText(a_text);
	emit widgetTextChanged(a_text);
}
#include "lineedit.moc"
