/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qpushbutton.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"
#include "closebutton.h"

CloseButton::CloseButton(QWidget *a_parent, const char *a_name)
	: QPushButton(a_parent, a_name), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);

	QObject *parent = this;
	while(parent->parent() != 0)
	{
		parent = parent->parent();
		if(parent->inherits("QDialog"))
			break;
	}
	connect(this, SIGNAL(clicked()), parent, SLOT(reject()));
}

CloseButton::~CloseButton()
{
}

QString CloseButton::currentState() const
{
	return QString("default");
}

bool CloseButton::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList CloseButton::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void CloseButton::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void CloseButton::setWidgetText(const QString &a_text)
{
	emit widgetTextChanged(a_text);
}

QString CloseButton::widgetText() const
{
	return QString::null;
}
