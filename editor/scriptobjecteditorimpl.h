#ifndef HAVE_SCRIPTOBJECTEDITORIMPL_H
#define HAVE_SCRIPTOBJECTEDITORIMPL_H

/* Qt Includes */
#include <qobject.h>
#include <qwidget.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qstring.h>


/* Other Includes */
#include "scriptobjecteditor.h"

class QString;
class FormWindow;
class ScriptObject;

class ScriptObjectEditor : public ScriptObjectEditorBase
{
	Q_OBJECT
public:
	ScriptObjectEditor(QMap<QString, QString>, QWidget * = 0, const char * = 0);
	~ScriptObjectEditor();

	QMap<QString, QString> scriptObjects() const;
private slots:
	/* Private Slots */
	void createScriptObject();
	void removeScriptObject();
	void selectionChanged();
	void setScript();
	void changeObjectName(const QString &);
private:
	/* Private Methods */

	/* Private Data */
	QMap<QString, QString> m_objects;
};

#endif
