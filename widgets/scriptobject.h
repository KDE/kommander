#ifndef _HAVE_SCRIPTOBJECT_H_
#define _HAVE_SCRIPTOBJECT_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qwidget.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class ScriptObject : public QWidget, public AssocTextWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false);
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget);
public:
	ScriptObject(QWidget *a_parent, const char *a_name);
	~ScriptObject();

	virtual QString widgetText() const;

	virtual bool isAssociatedTextWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

public slots:
	virtual void setWidgetText(const QString &);
signals:
	void widgetOpened();
	void widgetTextChanged(const QString &);
protected:
	QString m_script;
private:
};

#endif
