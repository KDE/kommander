/***************************************************************************
                         Combobox - Combobox widget 
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
/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qcombobox.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "combobox.h"

ComboBox::ComboBox(QWidget *a_parent, const char *a_name)
	: QComboBox(a_parent, a_name), KommanderWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

	connect(this, SIGNAL(activated(int)), this, SLOT(emitWidgetTextChanged(int)));

}

ComboBox::~ComboBox()
{
}

QString ComboBox::currentState() const
{
	return QString("default");
}

bool ComboBox::isKommanderWidget() const
{
	return TRUE;
}

QStringList ComboBox::associatedText() const
{
	return KommanderWidget::associatedText();
}

void ComboBox::setAssociatedText(QStringList a_at)
{
	KommanderWidget::setAssociatedText(a_at);
}

void ComboBox::setPopulationText( QString a_text )
{
    KommanderWidget::setPopulationText( a_text );
}

QString ComboBox::populationText() const
{
    return KommanderWidget::populationText();
}

void ComboBox::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
    setWidgetText( txt );
}

void ComboBox::setWidgetText(const QString &a_text)
{
	QStringList strings = QStringList::split("\n", a_text);

	clear();

	insertStringList(strings);

	emit widgetTextChanged(a_text);
}

QString ComboBox::widgetText() const
{
	return currentText();
}

#if 0
QStringList ComboBox::items() const
{
	QStringList itemList;
	int i = 0;
	for(;i < count();++i)
		itemList += text(i);
	return itemList;
}

void ComboBox::setItems(QStringList a_items)
{
	int i = 0;
	for(;i < count();++i)
		removeItem(i);

	insertStringList(a_items);
}

void ComboBox::resetItems()
{
	int i = 0;
	for(;i < count();++i)
		removeItem(i);
}
#endif

void ComboBox::emitWidgetTextChanged(int a_index)
{
	QString currentText = text(a_index);
	emit widgetTextChanged(currentText);
}

void ComboBox::showEvent( QShowEvent *e )
{
    QComboBox::showEvent( e );
    emit widgetOpened();
}
#include "combobox.moc"
