#ifndef _HAVE_RADIOBUTTON_H_
#define _HAVE_RADIOBUTTON_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qradiobutton.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qstring.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class QWidget;

class RadioButton : public QRadioButton, public AssocTextWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false);
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget)
public:
	RadioButton(QWidget *a_parent, const char *a_name);
	~RadioButton();

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
