/* KDE INCLUDES */
#include <kprocess.h>

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>

/* OTHER INCLUDES */
#include "myprocess.h"
#include "assoctextwidget.h"
#include <cstdio>
#include <cstdlib>

MyProcess::MyProcess(const AssocTextWidget *a_atw)
	: m_atw(a_atw)
{
}

QString MyProcess::run(QString a_command)
{
	KShellProcess proc;

	proc << a_command;

	connect(&proc, SIGNAL(receivedStdout(KProcess *, char *, int)), this, SLOT(receivedStdout(KProcess *, char *, int)));
	if(!proc.start(KProcess::Block, KProcess::Stdout))
	{
		m_atw->printError(QString(m_atw->m_thisObject->className()), "Unable to start shell process");
		return QString::null;
	}
	return m_output;
}

void MyProcess::receivedStdout(KProcess *, char *a_buffer, int a_len)
{
	char *buffer = new char[a_len+1];
	memcpy(buffer, a_buffer, a_len);
	buffer[a_len] = 0;

	QString output(buffer);
	m_output += output;
	delete buffer;
}
