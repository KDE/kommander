/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qcheckbox.h>
#include <qbutton.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"
#include "checkbox.h"

CheckBox::CheckBox(QWidget *a_parent, const char *a_name)
	: QCheckBox(a_parent, a_name), AssocTextWidget(this)
{
	QStringList states;
	states << "unchecked";
	states << "semichecked";
	states << "checked";
	setStates(states);
}

CheckBox::~CheckBox()
{
}

QString CheckBox::currentState() const
{
	if(state() == QButton::Off)
		return "checked";
	else if(state() == QButton::NoChange)
		return "semichecked";
	else if(state() == QButton::On)
		return "unchecked";
	return QString::null;
}

bool CheckBox::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList CheckBox::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void CheckBox::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void CheckBox::setWidgetText(const QString &a_text)
{
	emit widgetTextChanged(a_text);
}

QString CheckBox::widgetText() const
{
	return QString::null;
}
#include "checkbox.moc"
