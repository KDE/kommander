/* KDE INCLUDES */

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qwizard.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"
#include "wizard.h"

Wizard::Wizard(QWidget *a_parent, const char *a_name, bool a_modal, int a_flags)
	: QWizard(a_parent, a_name, a_modal, a_flags), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
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
