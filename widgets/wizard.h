/***************************************************************************
                          wizard.h - Widget providing a wizard 
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
#ifndef _HAVE_WIZARD_H_
#define _HAVE_WIZARD_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qwizard.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class Wizard : public QWizard, public AssocTextWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget)

	Q_PROPERTY(HelpAction helpAction READ helpAction WRITE setHelpAction)
	Q_ENUMS(HelpAction)

	Q_PROPERTY(QString helpActionText READ helpActionText WRITE setHelpActionText)
public:
	Wizard(QWidget *, const char *, bool=TRUE, int=0);
	~Wizard();

	virtual QString widgetText() const;

	virtual bool isAssociatedTextWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

	enum HelpAction { None, Command, Dialog };
	HelpAction helpAction() const;
	void setHelpAction(HelpAction);

	QString helpActionText() const;
	void setHelpActionText(QString);

public slots:
	virtual void setWidgetText(const QString &);
	virtual void exec();
	virtual void runHelp();
signals:
	void widgetOpened();
	void widgetTextChanged(const QString &);
	void finished();
protected:
	HelpAction m_helpAction;
	QString m_helpActionText;
private:
};

#endif
