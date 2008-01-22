/***************************************************************************
                          plugins.cpp - Kommander Plugins 
                          -------------------
    copyright            : (C) 2003        Marc Britton <consume@optusnet.com.au>
                           (C) 2004-2006   Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kommanderplugin.h>
#include <kiconloader.h>

/* WIDGET INCLUDES */
#include "buttongroup.h"
#include "checkbox.h"
#include "closebutton.h"
#include "combobox.h"
#include "datepicker.h"
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
#include "table.h"
#include "textbrowser.h"
#include "textedit.h"
#include "textedit.h"
#include "timer.h"
#include "treewidget.h"
#include "toolbox.h"
#include "wizard.h"
#include "popupmenu.h"
#include "fontdialog.h"
#include "aboutdialog.h"

class KomStdPlugin : public KommanderPlugin
{
public:
    KomStdPlugin();
    virtual QWidget *create( const QString &key, QWidget *parent = 0, const char *name = 0 );
};

KomStdPlugin::KomStdPlugin()
{
  const char *group = "Kommander";
  addWidget("Label", group, "", 0);
  addWidget("PixmapLabel", group, "", 0);
  addWidget("LineEdit", group, "", 0);
  addWidget("Dialog", group, "", 0);
  addWidget("ExecButton", group, "", 0);
  addWidget("CloseButton", group, "", 0);
  addWidget("Konsole", group, "", 0);
  addWidget("TextEdit", group, "", 0);
  addWidget("RadioButton", group, "", 0);
  // addWidget("", group, "");
  addWidget("GroupBox", group, "", 0);
  addWidget("ButtonGroup", group, "", 0);
  addWidget("CheckBox", group, "", 0);
  addWidget("ComboBox", group, "", 0);
  addWidget("SpinBoxInt", group, "", 0);
  // addWidget("Wizard", group, "");
  addWidget("TabWidget", group, "", 0);
  addWidget("ToolBox", group, "", new QIconSet(KGlobal::iconLoader()->loadIcon("toolbox", KIcon::NoGroup, KIcon::SizeMedium)), "", true);
  // addWidget("SubDialog", group, "");
  addWidget("ListBox", group, "", 0);
  addWidget("Timer", group, "", 0);
  addWidget("ScriptObject", group, "", 0);
  addWidget("RichTextEditor", group, "", 0);
  addWidget("TreeWidget", group, "", 0);
  addWidget("StatusBar", group, "", 0);
  addWidget("TextBrowser", group, "", 0);
  addWidget("Slider", group, "", 0);
  addWidget("Table", group, "", 0);
  addWidget("DatePicker", group, "", 0);
  addWidget("PopupMenu", group, "", new QIconSet(KGlobal::iconLoader()->loadIcon("contents", KIcon::NoGroup, KIcon::SizeMedium)));
  addWidget("FontDialog", group, "", new QIconSet(KGlobal::iconLoader()->loadIcon("kfontcombo", KIcon::NoGroup, KIcon::SizeMedium)));
  addWidget("AboutDialog", group, "", new QIconSet(KGlobal::iconLoader()->loadIcon("kommander", KIcon::NoGroup, KIcon::SizeMedium)));
}

QWidget *KomStdPlugin::create( const QString &key, QWidget *parent, const char *name )
{
  if (key == "ButtonGroup") return new ButtonGroup(parent, name);
  else if (key == "CheckBox") return new CheckBox(parent, name);
  else if (key == "CloseButton") return new CloseButton(parent, name);
  else if (key == "ComboBox") return new ComboBox(parent, name);
  else if (key == "Dialog") return new Dialog(parent, name);
  else if (key == "ExecButton") return new ExecButton(parent, name);
  else if (key == "FileSelector") return new FileSelector(parent, name);
  else if (key == "GroupBox") return new GroupBox(parent, name);
  else if (key == "Konsole") return new Konsole(parent, name);
  else if (key == "Label") return new Label(parent, name);
  else if (key == "LineEdit") return new LineEdit(parent, name);
  else if (key == "ListBox") return new ListBox(parent, name);
  else if (key == "PixmapLabel") return new PixmapLabel(parent, name);
  else if (key == "ProgressBar") return new ProgressBar(parent, name);
  else if (key == "RadioButton") return new RadioButton(parent, name);
  else if (key == "RichTextEditor") return new RichTextEditor(parent, name);
  else if (key == "ScriptObject") return new ScriptObject(parent, name);
  else if (key == "Slider") return new Slider(parent, name);
  else if (key == "SpinBoxInt") return new SpinBoxInt(parent, name);
  else if (key == "StatusBar") return new StatusBar(parent, name);
  else if (key == "SubDialog") return new SubDialog(parent, name);
  else if (key == "Table") return new Table(parent, name);
  else if (key == "TabWidget") return new TabWidget(parent, name);
  else if (key == "ToolBox") return new ToolBox(parent, name);
  else if (key == "TextBrowser") return new TextBrowser(parent, name);
  else if (key == "TextEdit") return new TextEdit(parent, name);
  else if (key == "Timer") return new Timer(parent, name);
  else if (key == "TreeWidget") return new TreeWidget(parent, name);
  else if (key == "Wizard") return new Wizard(parent, name);
  else if (key == "DatePicker") return new DatePicker(parent, name);
  else if (key == "PopupMenu") return new PopupMenu(parent, name);
  else if (key == "FontDialog") return new FontDialog(parent, name);
  else if (key == "AboutDialog") return new AboutDialog(parent, name);
  else return 0;
}

KOMMANDER_EXPORT_PLUGIN(KomStdPlugin)
