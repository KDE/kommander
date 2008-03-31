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
#include "timer.h"
#include "toolbox.h"
#include "treewidget.h"
#include "wizard.h"
#include "popupmenu.h"
#include "fontdialog.h"
#include "aboutdialog.h"

#include <kiconloader.h>

#include <QIcon>

class KomStdPlugin : public KommanderPlugin
{
    public:
        KomStdPlugin();
        virtual QWidget *create ( const QString &key, QWidget *parent = 0, const char *name = 0 );
};

KomStdPlugin::KomStdPlugin()
{
//FIXME add the icons!
    const char *group = "Kommander";
    addWidget ( "Label", group, "", 0L );
    addWidget ( "PixmapLabel", group, "", 0L );
    addWidget ( "LineEdit", group, "", 0L );
    addWidget ( "Dialog", group, "", 0L );
    addWidget ( "ExecButton", group, "", 0L );
    addWidget ( "CloseButton", group, "", 0L );
    addWidget ( "Konsole", group, "", 0L );
    addWidget ( "TextEdit", group, "", 0L );
    addWidget ( "RadioButton", group, "", 0L );
    addWidget ( "GroupBox", group, "", 0L );
    addWidget ( "ButtonGroup", group, "", 0L );
    addWidget ( "CheckBox", group, "", 0L );
    addWidget ( "ComboBox", group, "", 0L );
    addWidget ( "SpinBoxInt", group, "", 0L );
    addWidget ( "TabWidget", group, "", 0L );
    addWidget ( "ToolBox", group, "", new QIcon ( KIconLoader::global()->loadIconSet ( "toolbox", KIconLoader::NoGroup, KIconLoader::SizeMedium ) ), "", true );
    addWidget ( "Timer", group, "", 0L );
    addWidget ( "ScriptObject", group, "", 0L );
    addWidget ( "StatusBar", group, "", 0L );
    addWidget ( "TextBrowser", group, "", 0L );
    addWidget ( "Slider", group, "", 0L );

    addWidget ( "Wizard", group, "", 0L );
    addWidget ( "Table", group, "", 0L );
    addWidget ( "RichTextEditor", group, "", 0L );
    addWidget ( "TreeWidget", group, "", 0L );
    addWidget ( "ListBox", group, "", 0L );
    addWidget ( "PopupMenu", group, "", new QIcon ( KIconLoader::global()->loadIconSet ( "contents", KIconLoader::NoGroup, KIconLoader::SizeMedium ) ) );
    addWidget ( "FontDialog", group, "", new QIcon ( KIconLoader::global()->loadIconSet ( "kfontcombo", KIconLoader::NoGroup, KIconLoader::SizeMedium ) ) );
    addWidget ( "AboutDialog", group, "", new QIcon ( KIconLoader::global()->loadIconSet ( "kommander", KIconLoader::NoGroup, KIconLoader::SizeMedium ) ) );

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
