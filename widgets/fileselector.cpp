/* KDE INCLUDES */
#include <kfiledialog.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qsizepolicy.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"
#include "fileselector.h"

FileSelector::FileSelector(QWidget *a_parent, const char *a_name)
	: QWidget(a_parent, a_name), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);

	m_lineEdit = new QLineEdit(this);
	m_selectButton = new QPushButton("...", this);

	m_boxLayout = new QHBoxLayout(this, 0, 11);
	m_boxLayout->addWidget(m_lineEdit);
	m_boxLayout->addWidget(m_selectButton);

	setSizePolicy(m_lineEdit->sizePolicy());
	m_lineEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	m_selectButton->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));

	connect(m_selectButton, SIGNAL(clicked()), this, SLOT(makeSelection()));

	setSelectionType(Open);
	setSelectionOpenMultiple(FALSE);
}

FileSelector::~FileSelector()
{
}

QString FileSelector::currentState() const
{
	return QString("default");
}

bool FileSelector::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList FileSelector::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void FileSelector::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void FileSelector::setWidgetText(const QString &a_text)
{
	m_lineEdit->setText(a_text);
	emit widgetTextChanged(a_text);
}

QString FileSelector::widgetText() const
{
	return m_lineEdit->text();
}

FileSelector::SelectionType FileSelector::selectionType() const
{
	return m_selectionType;
}

void FileSelector::setSelectionType(SelectionType a_selectionType)
{
	m_selectionType = a_selectionType;
}

QString FileSelector::selectionFilter() const
{
	return m_filter;
}

void FileSelector::setSelectionFilter(QString a_filter)
{
	m_filter = a_filter;
}

QString FileSelector::selectionCaption() const
{
	return m_caption;
}

void FileSelector::setSelectionCaption(QString a_caption)
{
	m_caption = a_caption;
}

bool FileSelector::selectionOpenMultiple() const
{
	return m_openMultiple;
}

void FileSelector::setSelectionOpenMultiple(bool a_openMultiple)
{
	m_openMultiple = a_openMultiple;
}

void FileSelector::makeSelection()
{
	// Depending on the SelectionType property we need to show either a save, open or directory dialog.
	QString text;
	if(m_selectionType == Open)
	{
		if(m_openMultiple)
			text = KFileDialog::getOpenFileNames(QString::null, m_filter, this, m_caption).join(" ");
		else
			text = KFileDialog::getOpenFileName(QString::null, m_filter, this, m_caption);
	}
	else if(m_selectionType == Save)
	{
		text = KFileDialog::getSaveFileName(QString::null, m_filter, this, m_caption);
	}
	else if(m_selectionType == Directory)
	{
		text = KFileDialog::getExistingDirectory(QString::null, this, m_caption);
	}

	if(!text.isEmpty())
		setWidgetText(text);
}
#include "fileselector.moc"
