/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qlistbox.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"
#include "listbox.h"

ListBox::ListBox(QWidget *a_parent, const char *a_name)
	: QListBox(a_parent, a_name), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);

	connect(this, SIGNAL(clicked(QListBoxItem *)), this, SLOT(setActivatedText(QListBoxItem *)));
}

ListBox::~ListBox()
{
}

QString ListBox::currentState() const
{
	return QString("default");
}

bool ListBox::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList ListBox::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void ListBox::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void ListBox::setWidgetText(const QString &a_text)
{
		//FIXME
	emit widgetTextChanged(a_text);
}

QString ListBox::widgetText() const
{
	return currentText();
}

void ListBox::setActivated(QListBoxItem *a_item)
{
	if(a_item)
		emit widgetTextChanged(a_item->text());
}
