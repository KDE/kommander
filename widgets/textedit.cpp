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
#include <qevent.h>
#include <qlineedit.h>

/* OTHER INCLUDES */
#include "textedit.h"

TextEdit::TextEdit(QWidget *a_parent, const char *a_name)
	: QTextEdit(a_parent, a_name), AssocTextWidget((QObject *)this)
{

	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

	connect(this, SIGNAL(textChanged()), this, SLOT(setTextChanged()));

	emit widgetOpened();
}

QString TextEdit::currentState() const
{
	return QString("default");
}

TextEdit::~TextEdit()
{
}

bool TextEdit::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList TextEdit::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void TextEdit::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
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
#include "textedit.moc"
