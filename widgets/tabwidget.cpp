/* KDE INCLUDES */

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qtabwidget.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *a_parent, const char *a_name, int a_flags)
	: QTabWidget(a_parent, a_name, a_flags), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);
}

TabWidget::~TabWidget()
{
}

QString TabWidget::currentState() const
{
	return QString("default");
}

bool TabWidget::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList TabWidget::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void TabWidget::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void TabWidget::setWidgetText(const QString &a_text)
{
	setCaption(a_text);
	emit widgetTextChanged(a_text);
}

QString TabWidget::widgetText() const
{
	return caption();
}
#include "tabwidget.moc"
