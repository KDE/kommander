/***************************************************************************
 *   Copyright (C) 2008 by Eric Laffoon   *
 *   sequitur@kde.org   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "aboutdialog.h"
#include "aboutdialogkmdrdesignerplugin.h"

#include <QtPlugin>

AboutDialogKmdrDesignerPlugin::AboutDialogKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void AboutDialogKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool AboutDialogKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *AboutDialogKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new AboutDialog(parent);
}

QString AboutDialogKmdrDesignerPlugin::name() const
{
  return "AboutDialog";
}

QString AboutDialogKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon AboutDialogKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString AboutDialogKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString AboutDialogKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool AboutDialogKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString AboutDialogKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"AboutDialog\" name=\"AboutDialog\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>32</width>\n"
        "   <height>32</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>About Dialog</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>Standard KDE about dialog</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString AboutDialogKmdrDesignerPlugin::includeFile() const
{
  return "aboutdialog.h";
}
