/***************************************************************************
			treewidget.cpp - A tree widget
                             -------------------
		begin			: 03/08/2003
		copyright		: (C) Marc Britton
		email			: consume@optusnet.com.au
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
#include <klocale.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qlistview.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "treewidget.h"

TreeWidget::TreeWidget(QWidget *a_parent, const char *a_name)
	: QListView(a_parent, a_name), KommanderWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

}

TreeWidget::~TreeWidget()
{
}

QString TreeWidget::currentState() const
{
	return QString("default");
}

bool TreeWidget::isKommanderWidget() const
{
	return TRUE;
}

QStringList TreeWidget::associatedText() const
{
	return KommanderWidget::associatedText();
}

void TreeWidget::setAssociatedText(QStringList a_at)
{
	KommanderWidget::setAssociatedText(a_at);
}

void TreeWidget::setPopulationText( QString a_text )
{
    KommanderWidget::setPopulationText( a_text );
}

QString TreeWidget::populationText() const
{
    return KommanderWidget::populationText();
}

void TreeWidget::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
    //implement me
}

void TreeWidget::setWidgetText(const QString &a_text)
{
	//set the widget text of your widget here
	emit widgetTextChanged(a_text);
}

QString TreeWidget::widgetText() const
{
	// implement your widget text here
	return QString::null;
}

void TreeWidget::setSelectedWidgetText( const QString & )
{
}

QString TreeWidget::selectedWidgetText() const
{
    return QString::null;
}

void TreeWidget::showEvent( QShowEvent *e )
{
    QListView::showEvent( e );
    emit widgetOpened();
}


#include "treewidget.moc"
