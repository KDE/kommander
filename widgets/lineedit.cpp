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
	: QLineEdit(a_parent, a_name), AssocTextWidget((QObject *)this)
{

	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

	connect(this, SIGNAL(textChanged(const QString &)), this, SIGNAL(widgetTextChanged(const QString &)));

	emit widgetOpened();
}

QString LineEdit::currentState() const
{
	return QString("default");
}

LineEdit::~LineEdit()
{
}

bool LineEdit::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList LineEdit::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void LineEdit::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

QString LineEdit::widgetText() const
{
	return text();
}

void LineEdit::setWidgetText(const QString &a_text)
{
	setText(a_text);
}
#include "lineedit.moc"
