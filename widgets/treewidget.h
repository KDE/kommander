/***************************************************************************
				treewidget.h - A tree widget
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

#ifndef _HAVE_TREEWIDGET_H_
#define _HAVE_TREEWIDGET_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qlistview.h>
#include <qstring.h>
#include <qstringlist.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QWidget;

class TreeWidget : public QListView, public KommanderWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
	
public:
	TreeWidget(QWidget *a_parent, const char *a_name);
	~TreeWidget();

	virtual QString widgetText() const;

	virtual bool isKommanderWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

public slots:
	virtual void setWidgetText(const QString &);
signals:
	void widgetOpened();
	void widgetTextChanged(const QString &);
protected:
private:
};

#endif
