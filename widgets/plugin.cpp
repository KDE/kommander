#include <kommanderplugin.h>

/* WIDGET INCLUDES */
#include "buttongroup.h"
#include "checkbox.h"
#include "closebutton.h"
#include "combobox.h"
#include "dialog.h"
#include "execbutton.h"
#include "fileselector.h"
#include "groupbox.h"
#include "konsole.h"
#include "label.h"
#include "lineedit.h"
#include "listbox.h"
#include "pixmaplabel.h"
#include "progressbar.h"
#include "radiobutton.h"
#include "richtexteditor.h"
#include "scriptobject.h"
#include "slider.h"
#include "spinboxint.h"
#include "statusbar.h"
#include "subdialog.h"
#include "tabwidget.h"
#include "textbrowser.h"
#include "textedit.h"
#include "textedit.h"
#include "treewidget.h"
#include "wizard.h"


class KomStdPlugin : public KommanderPlugin
{
public:
    KomStdPlugin();
    virtual QWidget *create( const QString &key, QWidget *parent = 0, const char *name = 0 );
};

KomStdPlugin::KomStdPlugin()
{
  const char *group = "Kommander";
  addWidget("Label", group, "");
  addWidget("PixmapLabel", group, "");
  addWidget("LineEdit", group, "");
  addWidget("Dialog", group, "");
  addWidget("ExecButton", group, "");
  addWidget("CloseButton", group, "");
  addWidget("Konsole", group, "");
  addWidget("TextEdit", group, "");
  addWidget("RadioButton", group, "");
  // addWidget("", group, "");
  addWidget("GroupBox", group, "");
  addWidget("ButtonGroup", group, "");
  addWidget("CheckBox", group, "");
  addWidget("ComboBox", group, "");
  addWidget("SpinBoxInt", group, "");
  // addWidget("Wizard", group, "");
  addWidget("TabWidget", group, "");
  // addWidget("SubDialog", group, "");
  addWidget("ListBox", group, "");
  addWidget("ScriptObject", group, "");
  addWidget("RichTextEditor", group, "");
  addWidget("TreeWidget", group, "");
  addWidget("StatusBar", group, "");
  addWidget("TextBrowser", group, "");
  addWidget("Slider", group, "");
}

QWidget *KomStdPlugin::create( const QString &key, QWidget *parent, const char *name )
{
  if (key == "Label")
    return new Label(parent, name);
  else if (key == "PixmapLabel")
    return new PixmapLabel(parent, name);
  else if (key == "LineEdit")
    return new LineEdit(parent, name);
  else if (key == "Dialog")
    return new Dialog(parent, name);
  else if (key == "Wizard")
    return new Wizard(parent, name);
  else if (key == "TabWidget")
    return new TabWidget(parent, name);
  else if (key == "ExecButton")
    return new ExecButton(parent, name);
  else if (key == "CloseButton")
    return new CloseButton(parent, name);
  else if (key == "SubDialog")
    return new SubDialog(parent, name);
  else if (key == "FileSelector")
    return new FileSelector(parent, name);
  else if (key == "TextEdit")
    return new TextEdit(parent, name);
  else if (key == "TextBrowser")
    return new TextBrowser(parent, name);
  else if (key == "RadioButton")
    return new RadioButton(parent, name);
  else if (key == "ButtonGroup")
    return new ButtonGroup(parent, name);
  else if (key == "GroupBox")
    return new GroupBox(parent, name);
  else if (key == "CheckBox")
    return new CheckBox(parent, name);
  else if (key == "ComboBox")
    return new ComboBox(parent, name);
  else if (key == "SpinBoxInt")
    return new SpinBoxInt(parent, name);
  else if (key == "ListBox")
    return new ListBox(parent, name);
  else if (key == "ScriptObject")
    return new ScriptObject(parent, name);
  else if (key == "RichTextEditor")
    return new RichTextEditor(parent, name);
  else if (key == "TreeWidget")
    return new TreeWidget(parent, name);
  else if (key == "StatusBar")
    return new StatusBar(parent, name);
  else if (key == "ProgressBar")
    return new ProgressBar(parent, name);
  else if (key == "Slider")
    return new Slider(parent, name);
  else if (key == "Konsole")
    return new Konsole(parent, name);
  return 0;
}

KOMMANDER_EXPORT_PLUGIN(KomStdPlugin)
