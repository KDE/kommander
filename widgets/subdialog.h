/***************************************************************************
                          subdialog.h - Subdialog widget 
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
#ifndef _HAVE_SUBDIALOG_H_
#define _HAVE_SUBDIALOG_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qpushbutton.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QWidget;
class QDialog;

class QShowEvent;
class SubDialog : public QPushButton, public KommanderWidget
{
	Q_OBJECT

	Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
	Q_PROPERTY(QString kmdrFile READ kmdrFile WRITE setKmdrFile)
	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
public:
	SubDialog(QWidget *a_parent, const char *a_name);
	~SubDialog();

	virtual QString widgetText() const;

	virtual bool isKommanderWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

	virtual QString populationText() const;
	virtual void setPopulationText(QString);
	QString kmdrFile() const;
public slots:
	void setKmdrFile(QString);
	virtual void setWidgetText(const QString &);
	virtual void showDialog();
	virtual void slotFinished();
	virtual void populate();
signals:
	void widgetOpened();
	void widgetTextChanged(const QString &);
protected:
	QDialog *m_dialog;
	QString m_kmdrFile;
    void showEvent( QShowEvent *e );
private:
};

#endif
