/* KDE INCLUDES */
#include <kprocess.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qwizard.h>
#include <qdialog.h>

/* OTHER INCLUDES */
#include "ewidgetfactory.h"
#include "assoctextwidget.h"
#include "wizard.h"

Wizard::Wizard(QWidget *a_parent, const char *a_name, bool a_modal, int a_flags)
	: QWizard(a_parent, a_name, a_modal, a_flags), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);

	connect(this, SIGNAL(helpClicked()), SLOT(runHelp()));
}

Wizard::~Wizard()
{
}

QString Wizard::currentState() const
{
	return QString("default");
}

bool Wizard::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList Wizard::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void Wizard::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void Wizard::setWidgetText(const QString &a_text)
{
	setCaption(a_text);
	emit widgetTextChanged(a_text);
}

QString Wizard::widgetText() const
{
	return caption();
}

void Wizard::exec()
{
	QWizard::exec();

	emit finished();
}

void Wizard::runHelp()
{
	if(helpAction() == Command)
	{
		KProcess proc;

		proc << helpActionText();

		proc.start(KProcess::DontCare, KProcess::NoCommunication);
	}
	else if(helpAction() == Dialog)
	{
		QDialog *dialog = (QDialog *)EWidgetFactory::create(helpActionText());

		dialog->exec();

		delete dialog;
	}
}

Wizard::HelpAction Wizard::helpAction() const
{
	return m_helpAction;
}

void Wizard::setHelpAction(HelpAction a_helpAction)
{
	m_helpAction = a_helpAction;
}

QString Wizard::helpActionText() const
{
	return m_helpActionText;
}

void Wizard::setHelpActionText(QString a_helpActionText)
{
	m_helpActionText = a_helpActionText;
}

