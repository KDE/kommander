/***************************************************************************
                          richtexteditor.h - Widget providing simple rich texxt editing 
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
#ifndef _HAVE_RICHTEXTEDITOR_H_
#define _HAVE_RICHTEXTEDITOR_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qstringlist.h>
#include <qstring.h>
#include <qwidget.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

/* Forward Decls */
class QTextEdit;
class QFrame;
class QToolButton;
class QHButtonGroup;

class RichTextEditor : public QWidget, public AssocTextWidget
{
	Q_OBJECT
	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false);
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget)
public:
	RichTextEditor(QWidget *, const char *);
	virtual ~RichTextEditor();
	QString widgetText() const;

	bool isAssociatedTextWidget() const;
	void setAssociatedText(QStringList);
	QStringList associatedText() const;
	QString currentState() const;

public slots:
	void setTextChanged();
	void setWidgetText(const QString &);
	void textAlign(int);
	void textBold(bool);
	void textUnder(bool);
	void textItalic(bool);
	void fontChanged(const QFont &);
	void alignmentChanged(int);
signals:
	void widgetOpened();
	void widgetTextChanged(const QString &);
private:
	QFrame *m_toolbar;
	QTextEdit *m_textedit;

	QToolButton *m_buttonTextBold, *m_buttonTextUnder, *m_buttonTextItalic, *m_buttonTextLeft, *m_buttonTextCenter, *m_buttonTextRight;

	QHButtonGroup *m_formatGroup, *m_alignGroup;

};

#endif
