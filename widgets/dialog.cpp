/* KDE INCLUDES */

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qdialog.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"
#include "dialog.h"

Dialog::Dialog(QWidget *a_parent, const char *a_name, bool a_modal, int a_flags)
	: QDialog(a_parent, a_name, a_modal, a_flags), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
}

Dialog::~Dialog()
{
}

QString Dialog::currentState() const
{
	return QString("default");
}

bool Dialog::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList Dialog::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void Dialog::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void Dialog::setWidgetText(const QString &a_text)
{
	setCaption(a_text);
	emit widgetTextChanged(a_text);
}

QString Dialog::widgetText() const
{
	return caption();
}

void Dialog::exec()
{
	QDialog::exec();

	emit finished();
}
