/***************************************************************************
                          textedit.h - Rich text editing widget 
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
#ifndef _HAVE_TEXTEDIT_H_
#define _HAVE_TEXTEDIT_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qtextedit.h>
#include <qstringlist.h>
#include <qstring.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QShowEvent;
class TextEdit : public QTextEdit, public KommanderWidget
{
	Q_OBJECT

	Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
public:
	TextEdit(QWidget *, const char *);
	virtual ~TextEdit();
	virtual QString widgetText() const;

	virtual bool isKommanderWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

	virtual QString populationText() const;
	virtual void setPopulationText(QString);

public slots:
	virtual void setTextChanged();
	virtual void setWidgetText(const QString &);
	virtual void populate();
signals:
	void widgetOpened();
	void widgetTextChanged(const QString &);
protected:
    void showEvent( QShowEvent *e );
private:
};

#endif
