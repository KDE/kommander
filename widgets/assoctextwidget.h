#ifndef _HAVE_ASSOCTEXTWIDGET_H_
#define _HAVE_ASSOCTEXTWIDGET_H_

/* KDE INCLUDES */
#include <kprocess.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qstringlist.h>
#include <qobject.h>

class AssocTextWidget
{
	friend class MyProcess;
public:
	AssocTextWidget(QObject *);
	virtual ~AssocTextWidget();

	virtual QStringList states() const;
	virtual QString currentState() const = 0;

	virtual bool isAssociatedTextWidget() const = 0;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString evalAssociatedText() const;
	virtual QString evalAssociatedText(const QString &) const;

	virtual QString widgetText() const = 0;
//	virtual void setWidgetText(QString) = 0;
protected:
	virtual void setStates(QStringList);
	QString dcopQuery(QString) const;
	QString execCommand(QString) const;
	void printError(QString, QString) const;

	QObject *m_thisObject;
	QStringList m_states;
	QStringList m_associatedText;
};



#define SPECIAL_NAME "widgetText"
#define ESCCHAR '@'

#endif
