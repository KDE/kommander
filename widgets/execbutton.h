#ifndef _HAVE_EXECBUTTON_H_
#define _HAVE_EXECBUTTON_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qpushbutton.h>
#include <qobject.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class QWidget;
class KProcess;

class ExecButton : public QPushButton, public AssocTextWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget)
	Q_PROPERTY(bool writeStdout READ writeStdout WRITE setWriteStdout)
	
public:
	ExecButton(QWidget *a_parent, const char *a_name);
	~ExecButton();

	virtual QString widgetText() const;

	virtual bool isAssociatedTextWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

	virtual void setWriteStdout(bool);
	bool writeStdout() const;
public slots:
	virtual void setWidgetText(const QString &);
	virtual void startProcess();
	virtual void appendOutput(KProcess *, char *, int);
	virtual void endProcess(KProcess *);
signals:
	void widgetTextChanged(const QString &);
protected:
	bool m_writeStdout;
	QString m_output;
private:
};

#endif
