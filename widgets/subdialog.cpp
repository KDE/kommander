/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qpushbutton.h>
#include <qdialog.h>

/* OTHER INCLUDES */
#include "ewidgetfactory.h"
#include "assoctextwidget.h"
#include "subdialog.h"

SubDialog::SubDialog(QWidget *a_parent, const char *a_name)
	: QPushButton(a_parent, a_name), AssocTextWidget(this), m_dialog(0)
{
	QStringList states;
	states << "default";
	setStates(states);

	connect(this, SIGNAL(clicked()), this, SLOT(showDialog()));
}

SubDialog::~SubDialog()
{
}

QString SubDialog::currentState() const
{
	return QString("default");
}

bool SubDialog::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList SubDialog::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void SubDialog::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void SubDialog::setWidgetText(const QString &a_text)
{
}

void SubDialog::setKmdrFile(QString a_kmdrFile)
{
	m_kmdrFile = a_kmdrFile;
}

QString SubDialog::kmdrFile() const
{
	return m_kmdrFile;
}

QString SubDialog::widgetText() const
{
	if(m_dialog)
	{
		AssocTextWidget *atw = dynamic_cast<AssocTextWidget *>(m_dialog);
		if(atw)
		{
			return atw->evalAssociatedText();
		}
	}
	return QString::null;
}

void SubDialog::showDialog()
{
	if(m_dialog)
		delete m_dialog;

	m_dialog = (QDialog *)EWidgetFactory::create(kmdrFile());
	if(!m_dialog)
		qWarning("Creation of sub dialog failed ..");
	connect(m_dialog, SIGNAL(finished()), this, SLOT(slotFinished()));

	m_dialog->exec();
}

void SubDialog::slotFinished()
{
	if(m_dialog)
	{
		AssocTextWidget *atw = dynamic_cast<AssocTextWidget *>(m_dialog);
		if(atw)
			emit widgetTextChanged(atw->evalAssociatedText());
	}
}
#include "subdialog.moc"
