/* Qt Includes */
#include <qwidget.h>
#include <qstringlist.h>

/* Other Includes */
#include "assoctextwidget.h"
#include "scriptobject.h"

ScriptObject::ScriptObject(QWidget *a_parent, const char *a_name)
	: QWidget(a_parent, a_name), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

	setHidden(TRUE);

	QStringList at("@widgetText");
	setAssociatedText(at);

	emit widgetOpened();
}

ScriptObject::~ScriptObject()
{
}

QString ScriptObject::currentState() const
{
	return QString("default");
}

bool ScriptObject::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList ScriptObject::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void ScriptObject::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void ScriptObject::setWidgetText(const QString &a_text)
{
	m_script = a_text;
	emit widgetTextChanged(a_text);
}

QString ScriptObject::widgetText() const
{
	return m_script;
}

void ScriptObject::show()
{
	return; // widget is never shown
}


#include "scriptobject.moc"
