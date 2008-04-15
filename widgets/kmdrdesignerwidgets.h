//
// C++ Interface: designerwidgets
//
// Description: 
//
//
// Author:  Eric Laffoon <sequitur@kde.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DESIGNERWIDGETS_H
#define DESIGNERWIDGETS_H

/**
	@author Eric Laffoon <sequitur@kde.org>
*/
#include "aboutdialogkmdrdesignerplugin.h"
#include "buttongroupkmdrdesignerplugin.h"
#include "checkboxkmdrdesignerplugin.h"
#include "closebuttonkmdrdesignerplugin.h"
#include "comboboxkmdrdesignerplugin.h"
#include "datepickerkmdrdesignerplugin.h"
#include "execbuttonkmdrdesignerplugin.h"
#include "fileselectorkmdrdesignerplugin.h"
#include "fontdialogkmdrdesignerplugin.h"
#include "groupboxkmdrdesignerplugin.h"
#include "konsolekmdrdesignerplugin.h"
#include "labelkmdrdesignerplugin.h"
#include "lineeditkmdrdesignerplugin.h"
#include "listboxkmdrdesignerplugin.h"
#include "pixmaplabelkmdrdesignerplugin.h"
#include "popupmenukmdrdesignerplugin.h"
#include "progressbarkmdrdesignerplugin.h"
#include "radiobuttonkmdrdesignerplugin.h"
#include "scriptobjectkmdrdesignerplugin.h"
#include "sliderkmdrdesignerplugin.h"
#include "spinboxintkmdrdesignerplugin.h"
#include "statusbarkmdrdesignerplugin.h"
#include "tablekmdrdesignerplugin.h"
#include "tabwidgetkmdrdesignerplugin.h"
#include "textbrowserkmdrdesignerplugin.h"
#include "texteditkmdrdesignerplugin.h"
#include "timerkmdrdesignerplugin.h"
#include "toolboxkmdrdesignerplugin.h"
#include "treewidgetkmdrdesignerplugin.h"
#include "wizardkmdrdesignerplugin.h"

#include <QtDesigner/QtDesigner>
#include <QtCore/qplugin.h>

class KmdrDesignerWidgets: public QObject, public QDesignerCustomWidgetCollectionInterface
{
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

  public:
    KmdrDesignerWidgets(QObject *parent = 0);

  virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

  private:
    QList<QDesignerCustomWidgetInterface*> widgets;
    

};

#endif
