/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qcombobox.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"
#include "combobox.h"

ComboBox::ComboBox(QWidget *a_parent, const char *a_name)
	: QComboBox(a_parent, a_name), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

	connect(this, SIGNAL(activated(int)), this, SLOT(setActivatedText(int)));
}

ComboBox::~ComboBox()
{
}

QString ComboBox::currentState() const
{
	return QString("default");
}

bool ComboBox::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList ComboBox::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void ComboBox::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void ComboBox::setWidgetText(const QString &a_text)
{
	QStringList strings = QStringList::split("\n", a_text);

	clear();

	insertStringList(strings);
	
	emit widgetTextChanged(a_text);
}

QString ComboBox::widgetText() const
{
	return currentText();
}

QStringList ComboBox::items() const
{
	QStringList itemList;
	int i = 0;
	for(;i < count();++i)
		itemList += text(i);
	return itemList;
}

void ComboBox::setItems(QStringList a_items)
{
	int i = 0;
	for(;i < count();++i)
		removeItem(i);

	insertStringList(a_items);
}

void ComboBox::resetItems()
{
	int i = 0;
	for(;i < count();++i)
		removeItem(i);
}

void ComboBox::setActivated(int a_index)
{
	QString currentText = text(a_index);
	emit widgetTextChanged(currentText);
}
#include "combobox.moc"
