/***************************************************************************
                          dialog.h - Main dialog widget 
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
#ifndef _HAVE_DIALOG_H_
#define _HAVE_DIALOG_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qdialog.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QShowEvent;
class Dialog : public QDialog, public KommanderWidget
{
	Q_OBJECT

	Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
public:
	Dialog(QWidget *, const char *, bool=TRUE, int=0);
	~Dialog();

	virtual QString widgetText() const;
	virtual QString selectedWidgetText() const;

	virtual bool isKommanderWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

	virtual QString populationText() const;
	virtual void setPopulationText(QString);
public slots:
	virtual void setWidgetText(const QString &);
	virtual void setSelectedWidgetText(const QString &a_text);
	virtual void exec();
	virtual void populate();
signals:
	void widgetOpened();
	void widgetTextChanged(const QString &);
	void finished();
protected:
    void showEvent( QShowEvent *e );
private:
};

#endif
