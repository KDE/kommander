/* KDE INCLUDES */
#include <kprocess.h>
#include <kmessagebox.h>
#include <klocale.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qpushbutton.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"
#include "execbutton.h"
#include <cstdio>

ExecButton::ExecButton(QWidget *a_parent, const char *a_name)
	: QPushButton(a_parent, a_name), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
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

bool ExecButton::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList ExecButton::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void ExecButton::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void ExecButton::setWidgetText(const QString &a_text)
{
	emit widgetTextChanged(a_text);
}

QString ExecButton::widgetText() const
{
	return m_output;
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
		// FIXME : This is seriously screwed. I think the app needs to be executing for start to work. This means changing back AGAIN to modeless dialogs
#if 1
		if(!process->start(KProcess::NotifyOnExit, KProcess::Stdout))
		{
			KMessageBox::error(this, i18n("Failed to start shell process"));
			delete process;
			return;
		}
#else
		// Screw it, don't check for errors ;)
		process->start(KProcess::NotifyOnExit, KProcess::Stdout);
#endif

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
		printf(buffer);
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
#include "execbutton.moc"
