/***************************************************************************
                          lineedit.h - Lineedit widget 
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
#ifndef _HAVE_LINEEDIT_H_
#define _HAVE_LINEEDIT_H_

/* KDE INCLUDES */
//#include <klineedit.h>

/* QT INCLUDES */
#include <qtextedit.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qlineedit.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class LineEdit : public QLineEdit, public KommanderWidget
{
	Q_OBJECT
	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
public:
	LineEdit(QWidget *, const char *);
	virtual ~LineEdit();
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
private:
};

#endif
