/***************************************************************************
                          scriptobject.h - Widget for holding scripts 
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
#ifndef _HAVE_SCRIPTOBJECT_H_
#define _HAVE_SCRIPTOBJECT_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qwidget.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class ScriptObject : public QWidget, public AssocTextWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false);
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget);
	Q_PROPERTY(QString script READ widgetText WRITE setWidgetText DESIGNABLE false);
public:
	ScriptObject(QWidget *a_parent, const char *a_name);
	~ScriptObject();

	virtual QString widgetText() const;

	virtual bool isAssociatedTextWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

public slots:
	virtual void setWidgetText(const QString &);
	virtual void show();
signals:
	void widgetOpened();
	void widgetTextChanged(const QString &);
protected:
	QString m_script;
private:
};

#endif
