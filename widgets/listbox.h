/***************************************************************************
                          listbox.h - Listbox widget 
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
#ifndef _HAVE_LISTBOX_H_
#define _HAVE_LISTBOX_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qstringlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qlistbox.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QWidget;

class ListBox : public QListBox, public KommanderWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
public:
	ListBox(QWidget *a_parent, const char *a_name);
	~ListBox();

	virtual QString widgetText() const;

	virtual bool isKommanderWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

public slots:
	virtual void setWidgetText(const QString &);
	virtual void setActivated(int);
signals:
	void widgetOpened();
	void widgetTextChanged(const QString &);
protected:
private:
};

#endif
