
/* QT INCLUDES */
#include <qlayout.h>
#include <qevent.h>
#include <qlineedit.h>

/* OTHER INCLUDES */
#include "textedit.h"

TextEdit::TextEdit(QWidget *a_parent, const char *a_name)
	: QTextEdit(a_parent, a_name), AssocTextWidget((QObject *)this)
{

	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

	connect(this, SIGNAL(textChanged()), this, SLOT(setTextChanged()));
}

QString TextEdit::currentState() const
{
	return QString("default");
}

TextEdit::~TextEdit()
{
}

bool TextEdit::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList TextEdit::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void TextEdit::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

QString TextEdit::widgetText() const
{
	return text();
}

void TextEdit::setWidgetText(const QString &a_text)
{
	setText(a_text);
}

void TextEdit::setTextChanged()
{
	emit widgetTextChanged(text());
}
#include "textedit.moc"
