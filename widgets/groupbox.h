#ifndef _HAVE_GROUPBOX_H_
#define _HAVE_GROUPBOX_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qgroupbox.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qobjectlist.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class QWidget;

class GroupBox : public QGroupBox, public AssocTextWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false);
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget)
public:
	GroupBox(QWidget *a_parent, const char *a_name);
	~GroupBox();

	virtual QString widgetText() const;

	virtual bool isAssociatedTextWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

	virtual void insertChild(QObject *);
	virtual void removeChild(QObject *);
public slots:
	virtual void setWidgetText(const QString &);
signals:
	void widgetTextChanged(const QString &);
protected:
	QObjectList m_childList;
private:
};

#endif
