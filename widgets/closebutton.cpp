/***************************************************************************
                          closebutton.cpp - Button that when clicked, closes the dialog it's in 
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
/* KDE INCLUDES */
#include <kmessagebox.h>
#include <klocale.h>

/* QT INCLUDES */
#include <qdialog.h>
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qpushbutton.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"
#include "closebutton.h"

CloseButton::CloseButton(QWidget *a_parent, const char *a_name)
	: QPushButton(a_parent, a_name), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);
	setWriteStdout(false);

	connect(this, SIGNAL(clicked()), this, SLOT(startProcess()));

	QObject *parent = this;
	while(parent->parent() != 0)
	{
		parent = parent->parent();
		if(parent->inherits("QDialog"))
			break;
	}
	connect(this, SIGNAL(clicked()), parent, SLOT(reject()));

	emit widgetOpened();
}

CloseButton::~CloseButton()
{
}

QString CloseButton::currentState() const
{
	return QString("default");
}

bool CloseButton::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList CloseButton::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void CloseButton::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void CloseButton::setWidgetText(const QString &a_text)
{
	emit widgetTextChanged(a_text);
}

QString CloseButton::widgetText() const
{
	return m_output;
}

void CloseButton::startProcess()
{
	QString at = evalAssociatedText();

	if(!at.isEmpty())
	{
		KShellProcess *process = new KShellProcess;

		*process << at;

		connect(process, SIGNAL(processExited(KProcess *)), SLOT(endProcess(KProcess *)));
		connect(process, SIGNAL(receivedStdout(KProcess *, char *, int)), SLOT(appendOutput(KProcess *, char *, int)));
		connect(process, SIGNAL(receivedStderr(KProcess *, char *, int)), SLOT(appendOutput(KProcess *, char *, int)));

		if(!process->start(KProcess::Block, KProcess::Stdout))
		{
			KMessageBox::error(this, i18n("Failed to start shell process."));
			endProcess(process);
			return;
		}
	}
	else
		endProcess(0);
}

void CloseButton::appendOutput(KProcess *, char *a_buffer, int a_len)
{
	char *buffer = new char[a_len+1];
	buffer[a_len] = 0;
	for(int i = 0;i < a_len;++i)
		buffer[i] = a_buffer[i];

	QString bufferString(buffer);
	m_output += bufferString;
	if(writeStdout())
	{
		fputs(buffer, stdout);
		fflush(stdout);
	}
	delete buffer;
}

void CloseButton::endProcess(KProcess *a_process)
{
	emit widgetTextChanged(m_output);
	m_output = "";

	if(a_process)
		delete a_process;

}

bool CloseButton::writeStdout() const
{
	return m_writeStdout;
}

void CloseButton::setWriteStdout(bool a_enable)
{
	m_writeStdout = a_enable;
}
#include "closebutton.moc"
