/***************************************************************************
                          combobox.h - Combobox widget 
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
#ifndef _HAVE_COMBOBOX_H_
#define _HAVE_COMBOBOX_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qstringlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qcombobox.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class QWidget;

class ComboBox : public QComboBox, public AssocTextWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget)
	
//	Q_PROPERTY(QStringList items READ items WRITE setItems RESET resetItems);
public:
	ComboBox(QWidget *a_parent, const char *a_name);
	~ComboBox();

	virtual QString widgetText() const;

	virtual bool isAssociatedTextWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

//	QStringList items() const;
//	void setItems(QStringList);
//	void resetItems();
public slots:
	virtual void setWidgetText(const QString &);
	
	virtual void emitWidgetTextChanged(int);
signals:
	void widgetOpened();
	void widgetTextChanged(const QString &);
protected:
private:
};

#endif
