#ifndef HAVE_SCRIPTOBJECTEDITORIMPL_H
#define HAVE_SCRIPTOBJECTEDITORIMPL_H

/* Qt Includes */
#include <qobject.h>
#include <qwidget.h>

/* Other Includes */
#include "scriptobjecteditor.h"

class ScriptObjectEditor : public ScriptObjectEditorBase
{
	Q_OBJECT
public:
	ScriptObjectEditor(QWidget * = 0, const char * = 0);
	~ScriptObjectEditor();
private:
};

#endif
