#ifndef _HAVE_MYPROCESS_H_
#define _HAVE_MYPROCESS_H_

/* KDE INCLUDES */
#include <kprocess.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qobject.h>

class AssocTextWidget;

class MyProcess : QObject
{
	Q_OBJECT
public:
	MyProcess(const AssocTextWidget *);
public slots:
	QString run(QString);
	void receivedStdout(KProcess *, char *, int);
protected:
	const AssocTextWidget *m_atw;
	QString m_output;
};

#endif
