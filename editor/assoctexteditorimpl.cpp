/* KDE INCLUDES */
#include <kfiledialog.h>

/* QT INCLUDES */
#include <qstringlist.h>
#include <qdict.h>
#include <qcombobox.h>
#include <qtextedit.h>
#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qobject.h>
#include <qobjectlist.h>
#include <qpushbutton.h>
#include <qcheckbox.h>

/* OTHER INCLUDES */
#include <cstdio>
#include "assoctexteditor.h"
#include "assoctexteditorimpl.h"
#include <assoctextwidget.h>

AssocTextEditor::AssocTextEditor(QWidget *a_widget, AssocTextWidget *a_atw, QWidget *a_parent, const char *a_name, bool a_modal)
    : AssocTextEditorBase(a_parent, a_name, a_modal)
{
    // signals and slots connections
	m_widget = a_widget;
	build(a_atw);
	connect(stateComboBox, SIGNAL(activated(int)), SLOT(stateChanged(int)));
	connect(associatedTextEdit, SIGNAL(textChanged()), SLOT(textEditChanged()));
	connect(widgetComboBox, SIGNAL(activated(int)), SLOT(insertWidgetName(int)));
	connect(filePushButton, SIGNAL(clicked()), SLOT(insertFile()));
	connect(allWidgetsCheckBox, SIGNAL(clicked()), SLOT(updateTextWidgets()));
	connect(insertIdentifierButton, SIGNAL(clicked()), SLOT(insertIdentifier()));
}

AssocTextEditor::~AssocTextEditor()
{
}

void AssocTextEditor::build(AssocTextWidget *a_atw)
{
	a_atw->associatedText();
	QStringList at = a_atw->associatedText();
	m_states = a_atw->states();

	stateComboBox->insertStringList(m_states);
	QStringList::iterator s_it = m_states.begin();
	QStringList::iterator at_it = at.begin();
	for(;s_it != m_states.end();++s_it)// fill dict with text for each state
	{
		if(at_it != at.end())
		{
			m_atdict[(*s_it)] = (*at_it);
			++at_it;
		}
		else
		{
			m_atdict[(*s_it)] = QString::null;
		}
	}
	// initial text for initial state
	m_currentState = stateComboBox->currentText();
	associatedTextEdit->setText(m_atdict[m_currentState]);

	// populate widget combo
	updateTextWidgets();
}

void AssocTextEditor::updateTextWidgets()
{
	QObject *thisObject = m_widget;

	if(!(allWidgetsCheckBox->isChecked())) // list all of the widgets
	{
		QObject *tmp;
		while((tmp = thisObject->parent()) != 0)
		{
			thisObject = tmp;
			if(thisObject->inherits("QDialog")) // only track back to parent dialog
				break;
		}
	}

	QObjectList *objectList = thisObject->queryList();

	widgetComboBox->clear();
	if(objectList)
	{
		if(objectList->find(m_widget) != -1)
			objectList->remove(objectList->current());
			
		QObjectListIt it(*objectList);

		while(it.current() != 0)
		{
			QVariant flag = (it.current())->property("AssocTextWidget");
			if(flag.isValid() && !(QString(it.current()->name()).startsWith("qt_")))
				widgetComboBox->insertItem((it.current())->name());
			++it;
		}
		delete objectList;
	}
}

void AssocTextEditor::stateChanged(int a_index)
{
	m_currentState = stateComboBox->text(a_index);
	associatedTextEdit->setText(m_atdict[m_currentState]);
}

void AssocTextEditor::insertAssociatedText(QString a_text)
{
	associatedTextEdit->insert(a_text);
	m_atdict[m_currentState] = associatedTextEdit->text();
}

void AssocTextEditor::textEditChanged()
{
	m_atdict[m_currentState] = associatedTextEdit->text();
}

QStringList AssocTextEditor::associatedText() const
{
	QStringList at;
	QStringList states = m_states;
	QStringList::iterator it = states.begin();
	for(;it != states.end();++it)
	{
		at.append(m_atdict[(*it)]);
	}
	return at;
}

void AssocTextEditor::insertFile()
{
	QString fileName = KFileDialog::getOpenFileName();

	if(!fileName.isEmpty())
	{
		QFile insertFile(fileName);
		if(!insertFile.open(IO_ReadOnly))
		{
			qWarning(QString("Failed to open '") + fileName + "' for insertion");
			return;
		}

		QTextStream insertStream(&insertFile);

		QString insertText = insertStream.read();

		insertAssociatedText(insertText);

		insertFile.close();
	}
}

void AssocTextEditor::insertWidgetName(int index)
{
	QString name = widgetComboBox->text(index);
	
	if(!name.isEmpty())
		insertAssociatedText(QString(QString(QChar(ESCCHAR)))+name);
}

void AssocTextEditor::insertIdentifier()
{
	insertAssociatedText(QString("@widgetText"));
}

#if 0
QString AssocTextEditor::escapeText(QString &a_text)
{
	char *text = a_text.latin1();
	QString escapedText;

	int textLength = a_text.length();
	int i = 0;
	while(i < textLength)
	{
		while(i < textLength && text[i] != '$')
				escapedText += text[i++];

		if(i < textLength)
		{
			escapedText += "\\$";
		}
		++i;
	}
	return escapedText;
}

void AssocTextEditor::escapeDollarSigns()
{
	QString text = associatedTextEdit->text();
	associatedTextEdit->setText(escapeText(text));
}

AssocTextEdit::AssocTextEdit(QWidget *a_parent, const char *a_name)
    : QTextEdit(a_parent, a_name)
{
}

void AssocTextEdit::insert(const QString &a_text, bool a_indent, bool a_checkNewLine, bool a_removeSelected)
{
    QString text = a_text;
    QString escapedText;
    
    int textLength = a_text.length();
    int i = 0;
    while(i < textLength)
    {
        while(i < textLength && text[i] != '$')
   	    escapedText += text[i++];

        if(i < textLength)
        {
				escapedText += "\\$";
        }
        ++i;
    }
    QTextEdit::insert(escapedText, a_indent, a_checkNewLine, a_removeSelected);
}
#endif
#include "assoctexteditorimpl.moc"
