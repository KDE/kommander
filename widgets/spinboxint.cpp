/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qspinbox.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"
#include "spinboxint.h"

SpinBoxInt::SpinBoxInt(QWidget *a_parent, const char *a_name)
	: QSpinBox(a_parent, a_name), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
}

SpinBoxInt::~SpinBoxInt()
{
}

QString SpinBoxInt::currentState() const
{
	return "default";
}

bool SpinBoxInt::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList SpinBoxInt::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void SpinBoxInt::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void SpinBoxInt::setWidgetText(const QString &a_text)
{
	emit widgetTextChanged(a_text);
}

QString SpinBoxInt::widgetText() const
{
	return text();
}
