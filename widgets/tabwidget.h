#ifndef _HAVE_TABWIDGET_H_
#define _HAVE_TABWIDGET_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qtabwidget.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class TabWidget : public QTabWidget, public AssocTextWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget)
public:
	TabWidget(QWidget *, const char *, int=0);
	~TabWidget();

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
