#ifndef _HAVE_DIALOG_H_
#define _HAVE_DIALOG_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qdialog.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class Dialog : public QDialog, public AssocTextWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false);
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget)
public:
	Dialog(QWidget *, const char *, bool=TRUE, int=0);
	~Dialog();

	virtual QString widgetText() const;

	virtual bool isAssociatedTextWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;
public slots:
	virtual void setWidgetText(const QString &);
	virtual void exec();
signals:
	void widgetOpened();
	void widgetTextChanged(const QString &);
	void finished();
protected:
private:
};

#endif
