/***************************************************************************
                          execbutton.cpp - Button that runs its text association
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
#include <kprocess.h>
#include <kmessagebox.h>
#include <klocale.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qpushbutton.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "execbutton.h"
#include <cstdio>

ExecButton::ExecButton(QWidget *a_parent, const char *a_name)
	: QPushButton(a_parent, a_name), KommanderWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);
	setWriteStdout(false);

	connect(this, SIGNAL(clicked()), this, SLOT(startProcess()));

}

ExecButton::~ExecButton()
{
}

QString ExecButton::currentState() const
{
	return QString("default");
}

bool ExecButton::isKommanderWidget() const
{
	return TRUE;
}

QStringList ExecButton::associatedText() const
{
	return KommanderWidget::associatedText();
}

void ExecButton::setAssociatedText(const QStringList& a_at)
{
	KommanderWidget::setAssociatedText(a_at);
}

void ExecButton::setPopulationText(const QString& a_text )
{
    KommanderWidget::setPopulationText( a_text );
}

QString ExecButton::populationText() const
{
    return KommanderWidget::populationText();
}

void ExecButton::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
    setWidgetText( txt );
}

void ExecButton::setWidgetText(const QString &a_text)
{
    setText( a_text );    
	emit widgetTextChanged(a_text);
}

QString ExecButton::widgetText() const
{
	return m_output;
}

void ExecButton::setSelectedWidgetText( const QString &)
{
}

QString ExecButton::selectedWidgetText() const
{
    return QString::null;
}

void ExecButton::startProcess()
{
	QString at = evalAssociatedText();

	if(!at.isEmpty())
	{
		KShellProcess *process = new KShellProcess;

		*process << at;

		connect(process, SIGNAL(processExited(KProcess *)), SLOT(endProcess(KProcess *)));
		connect(process, SIGNAL(receivedStdout(KProcess *, char *, int)), SLOT(appendOutput(KProcess *, char *, int)));
		connect(process, SIGNAL(receivedStderr(KProcess *, char *, int)), SLOT(appendOutput(KProcess *, char *, int)));
		if(!process->start(KProcess::NotifyOnExit, KProcess::Stdout))
		{
			KMessageBox::error(this, i18n("Failed to start shell process."));
			delete process;
			return;
		}

		setEnabled(false); // disabled until process ends
	}
}

void ExecButton::appendOutput(KProcess *, char *a_buffer, int a_len)
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

void ExecButton::endProcess(KProcess *a_process)
{
	emit widgetTextChanged(m_output);
	m_output = "";

	setEnabled(true);

	delete a_process;
}

bool ExecButton::writeStdout() const
{
	return m_writeStdout;
}

void ExecButton::setWriteStdout(bool a_enable)
{
	m_writeStdout = a_enable;
}

void ExecButton::showEvent( QShowEvent *e )
{
    QPushButton::showEvent( e );
    emit widgetOpened();
}

#include "execbutton.moc"
