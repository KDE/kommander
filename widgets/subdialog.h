#ifndef _HAVE_SUBDIALOG_H_
#define _HAVE_SUBDIALOG_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qpushbutton.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"

class QWidget;
class QDialog;

class SubDialog : public QPushButton, public AssocTextWidget
{
	Q_OBJECT

	Q_PROPERTY(QString kmdrFile READ kmdrFile WRITE setKmdrFile);
	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false);
	Q_PROPERTY(bool AssocTextWidget READ isAssociatedTextWidget);
public:
	SubDialog(QWidget *a_parent, const char *a_name);
	~SubDialog();

	virtual QString widgetText() const;

	virtual bool isAssociatedTextWidget() const;
	virtual void setAssociatedText(QStringList);
	virtual QStringList associatedText() const;
	virtual QString currentState() const;
	QString kmdrFile() const;
public slots:
	void setKmdrFile(QString);
	virtual void setWidgetText(const QString &);
	virtual void showDialog();
	virtual void slotFinished();
signals:
	void widgetTextChanged(const QString &);
protected:
	QDialog *m_dialog;
	QString m_kmdrFile;
private:
};

#endif
