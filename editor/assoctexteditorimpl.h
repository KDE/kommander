/***************************************************************************
                          assoctexteditorimpl.h - Associated text editor implementation 
                             -------------------
    copyright            : (C) 2003 by Marc Britton
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
#ifndef _HAVE_ASSOCTEXTEDITORIMPL_H_
#define _HAVE_ASSOCTEXTEDITORIMPL_H_

#include <qstringlist.h>
#include <qstring.h>
#include <qwidget.h>
#include <qobject.h>
#include <qmap.h>

#include "assoctexteditor.h"
#include <assoctextwidget.h>

class AssocTextEditor : public AssocTextEditorBase
{
	Q_OBJECT
public:
	AssocTextEditor(QWidget *, AssocTextWidget *, QWidget *, const char *, bool=TRUE);
	~AssocTextEditor();
	void build(AssocTextWidget *);
	QStringList associatedText() const;

public slots:
	void insertIdentifier();
	void insertAssociatedText(QString);
	void stateChanged(int);
	void textEditChanged();
	void updateTextWidgets();
	void insertFile();
	void insertWidgetName(int);
signals:
private:
	QWidget *m_widget;
	QString m_currentState;
	QStringList m_states;
	QMap<QString, QString> m_atdict;
};

#endif
