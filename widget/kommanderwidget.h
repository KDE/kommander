/***************************************************************************
                          kommanderwidget.cpp - Text widget core functionality 
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
#ifndef _HAVE_KOMMANDERWIDGET_H_
#define _HAVE_KOMMANDERWIDGET_H_

/* KDE INCLUDES */
#include <kprocess.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qstringlist.h>
#include <qobject.h>

class KommanderWidget
{
	friend class MyProcess;
public:
	KommanderWidget(QObject *);
	virtual ~KommanderWidget();

	virtual QStringList states() const;
	virtual QStringList displayStates() const;
	virtual QString currentState() const = 0;

	virtual bool isKommanderWidget() const = 0;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString evalAssociatedText() const;
	virtual QString evalAssociatedText(const QString &) const;

	virtual QString populationText() const;
	virtual void setPopulationText( QString );

	virtual QString widgetText() const = 0;
	virtual void setWidgetText(const QString &) = 0;

	virtual QString selectedWidgetText() const = 0;
	virtual void setSelectedWidgetText(const QString &) = 0;

	virtual void populate() = 0;
protected:
	virtual void setStates(QStringList);
	virtual void setDisplayStates(QStringList);
	QStringList parseArgs( const QString &args, bool &ok ) const;
	QString dcopQuery(QString) const;
	QString execCommand(QString) const;
	void printError(QString, QString) const;

	QObject *m_thisObject;
	QStringList m_states;
	QStringList m_displayStates;
	QStringList m_associatedText;
	QString m_populationText;
};


#define ESCCHAR '@'

#endif
