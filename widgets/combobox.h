#ifndef _HAVE_COMBOBOX_H_
#define _HAVE_COMBOBOX_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qstringlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qcombobox.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class QWidget;

class ComboBox : public QComboBox, public AssocTextWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false);
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget);
	
	Q_PROPERTY(QStringList items READ items WRITE setItems RESET resetItems);
public:
	ComboBox(QWidget *a_parent, const char *a_name);
	~ComboBox();

	virtual QString widgetText() const;

	virtual bool isAssociatedTextWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

	QStringList items() const;
	void setItems(QStringList);
	void resetItems();
public slots:
	virtual void setWidgetText(const QString &);
	virtual void setActivated(int);
signals:
	void widgetTextChanged(const QString &);
protected:
private:
};

#endif
