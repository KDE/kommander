/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qbuttongroup.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"
#include "buttongroup.h"

#include "radiobutton.h" // include a button header for the compiler with dynamic cast below

ButtonGroup::ButtonGroup(QWidget *a_parent, const char *a_name)
	: QButtonGroup(a_parent, a_name), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
}

ButtonGroup::~ButtonGroup()
{
}

QString ButtonGroup::currentState() const
{
	return QString("default");
}

bool ButtonGroup::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList ButtonGroup::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void ButtonGroup::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void ButtonGroup::setWidgetText(const QString &)
{
}

QString ButtonGroup::widgetText() const
{
	int i = 0;
	QString text;
	for(;i < count();++i)
	{
		QButton *button = find(i);
		AssocTextWidget *textWidget = dynamic_cast<AssocTextWidget *>(button);
		if(textWidget)
			text += textWidget->evalAssociatedText();
	}
	return text;
}
#include "buttongroup.moc"
