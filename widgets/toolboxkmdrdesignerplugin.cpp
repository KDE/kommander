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
#include "toolbox.h"
#include "toolboxkmdrdesignerplugin.h"

#include <QtPlugin>

ToolBoxKmdrDesignerPlugin::ToolBoxKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void ToolBoxKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool ToolBoxKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *ToolBoxKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new ToolBox(parent);
}

QString ToolBoxKmdrDesignerPlugin::name() const
{
  return "ToolBox";
}

QString ToolBoxKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon ToolBoxKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString ToolBoxKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString ToolBoxKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool ToolBoxKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString ToolBoxKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"ToolBox\" name=\"ToolBox\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>200</width>\n"
        "   <height>200</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a ToolBox</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The ToolBox offers widget areas similar to the TabWidget</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString ToolBoxKmdrDesignerPlugin::includeFile() const
{
  return "toolbox.h";
}
