#ifndef _HAVE_WIZARD_H_
#define _HAVE_WIZARD_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qwizard.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class Wizard : public QWizard, public AssocTextWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false);
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget)
public:
	Wizard(QWidget *, const char *, bool=TRUE, int=0);
	~Wizard();

	virtual QString widgetText() const;

	virtual bool isAssociatedTextWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;
public slots:
	virtual void setWidgetText(const QString &);
signals:
	void widgetTextChanged(const QString &);
protected:
private:
};

#endif
