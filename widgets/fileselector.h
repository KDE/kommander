#ifndef _HAVE_FILESELECTOR_H_
#define _HAVE_FILESELECTOR_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qwidget.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class QBoxLayout;
class QPushButton;
class QLineEdit;

class FileSelector : public QWidget, public AssocTextWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget)

	Q_PROPERTY(SelectionType selectionType READ selectionType WRITE setSelectionType)
	Q_ENUMS(SelectionType)

	Q_PROPERTY(bool selectionOpenMultiple READ selectionOpenMultiple WRITE setSelectionOpenMultiple)
	Q_PROPERTY(QString selectionFilter READ selectionFilter WRITE setSelectionFilter)
	Q_PROPERTY(QString selectionCaption READ selectionCaption WRITE setSelectionCaption)
public:
	FileSelector(QWidget *a_parent, const char *a_name);
	~FileSelector();

	virtual QString widgetText() const;

	virtual bool isAssociatedTextWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

	enum SelectionType { Open, Save, Directory };
	SelectionType selectionType() const;
	void setSelectionType(SelectionType);

	bool selectionOpenMultiple() const;
	void setSelectionOpenMultiple(bool);
	QString selectionFilter() const;
	void setSelectionFilter(QString);
	QString selectionCaption() const;
	void setSelectionCaption(QString);
public slots:
	virtual void setWidgetText(const QString &);
	virtual void makeSelection();
signals:
	void widgetTextChanged(const QString &);
protected:
	bool m_openMultiple;
	QString m_caption;
	QString m_filter;
	SelectionType m_selectionType;
	QLineEdit *m_lineEdit;
	QPushButton *m_selectButton;
	QBoxLayout *m_boxLayout;
private:
};

#endif
