/***************************************************************************
                          closebutton.h - Button that when clicked, closes the dialog it's in
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
#ifndef _HAVE_CLOSEBUTTON_H_
#define _HAVE_CLOSEBUTTON_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qpushbutton.h>
#include <qobject.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QWidget;

class CloseButton : public QPushButton, public KommanderWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
	Q_PROPERTY(bool writeStdout READ writeStdout WRITE setWriteStdout)
public:
	CloseButton(QWidget *a_parent, const char *a_name);
	~CloseButton();

	virtual QString widgetText() const;

	virtual bool isKommanderWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;
	bool writeStdout() const;
public slots:
	virtual void setWriteStdout(bool);
	virtual void setWidgetText(const QString &);
	virtual void startProcess();
	virtual void appendOutput(KProcess *, char *, int);
	virtual void endProcess(KProcess *);
signals:
	void widgetOpened();
	void widgetTextChanged(const QString &);
protected:
	bool m_writeStdout;
	QString m_output;
private:
};

#endif
