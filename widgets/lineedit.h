#ifndef _HAVE_LINEEDIT_H_
#define _HAVE_LINEEDIT_H_

/* KDE INCLUDES */
//#include <klineedit.h>

/* QT INCLUDES */
#include <qtextedit.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qlineedit.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class LineEdit : public QLineEdit, public AssocTextWidget
{
	Q_OBJECT
	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget)
public:
	LineEdit(QWidget *, const char *);
	virtual ~LineEdit();
	virtual QString widgetText() const;

	virtual bool isAssociatedTextWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;

public slots:
	virtual void setWidgetText(const QString &);
signals:
	void widgetTextChanged(const QString &);
private:
};

#endif
