/***************************************************************************
                          textedit.cpp - Rich text editing widget 
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
#include <qlineedit.h>
#include <qstringlist.h>
#include <qevent.h>

/* OTHER INCLUDES */
#include "textedit.h"

TextEdit::TextEdit(QWidget *a_parent, const char *a_name)
	: QTextEdit(a_parent, a_name), KommanderWidget((QObject *)this)
{

	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

	connect(this, SIGNAL(textChanged()), this, SLOT(setTextChanged()));

}

QString TextEdit::currentState() const
{
	return QString("default");
}

TextEdit::~TextEdit()
{
}

bool TextEdit::isKommanderWidget() const
{
	return TRUE;
}

QStringList TextEdit::associatedText() const
{
	return KommanderWidget::associatedText();
}

void TextEdit::setAssociatedText(QStringList a_at)
{
	KommanderWidget::setAssociatedText(a_at);
}

void TextEdit::setPopulationText( QString a_text )
{
    KommanderWidget::setPopulationText( a_text );
}

QString TextEdit::populationText() const
{
    return KommanderWidget::populationText();
}

void TextEdit::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
    setWidgetText( txt );
}

QString TextEdit::widgetText() const
{
	return text();
}

void TextEdit::setWidgetText(const QString &a_text)
{
	setText(a_text);
}

void TextEdit::setTextChanged()
{
	emit widgetTextChanged(text());
}

void TextEdit::showEvent( QShowEvent *e )
{
    QTextEdit::showEvent( e );
    emit widgetOpened();
}

#include "textedit.moc"
