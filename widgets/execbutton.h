/***************************************************************************
                          execbutton.h - Button that runs its text association 
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
#ifndef _HAVE_EXECBUTTON_H_
#define _HAVE_EXECBUTTON_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qpushbutton.h>
#include <qobject.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QWidget;
class KProcess;

class QShowEvent;
class ExecButton : public QPushButton, public KommanderWidget
{
	Q_OBJECT

	Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
	Q_PROPERTY(bool writeStdout READ writeStdout WRITE setWriteStdout)
	
public:
	ExecButton(QWidget *a_parent, const char *a_name);
	~ExecButton();

	virtual QString widgetText() const;
	virtual QString selectedWidgetText() const;

	virtual bool isKommanderWidget() const;
	virtual void setAssociatedText(const QStringList&);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

	virtual QString populationText() const;
	virtual void setPopulationText(const QString&);

	virtual void setWriteStdout(bool);
	bool writeStdout() const;
public slots:
    virtual void populate();
	virtual void setWidgetText(const QString &);
		virtual void setSelectedWidgetText(const QString &a_text);
	virtual void startProcess();
	virtual void appendOutput(KProcess *, char *, int);
	virtual void endProcess(KProcess *);
signals:
	void widgetOpened();
	void widgetTextChanged(const QString&);
protected:
	bool m_writeStdout;
	QString m_output;
    void showEvent( QShowEvent *e );
private:
};

#endif
