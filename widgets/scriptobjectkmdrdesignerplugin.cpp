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
#include "scriptobject.h"
#include "scriptobjectkmdrdesignerplugin.h"

#include <QtPlugin>

ScriptObjectKmdrDesignerPlugin::ScriptObjectKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void ScriptObjectKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool ScriptObjectKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *ScriptObjectKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new ScriptObject(parent);
}

QString ScriptObjectKmdrDesignerPlugin::name() const
{
  return "ScriptObject";
}

QString ScriptObjectKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon ScriptObjectKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString ScriptObjectKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString ScriptObjectKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool ScriptObjectKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString ScriptObjectKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"ScriptObject\" name=\"ScriptObject\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>32</width>\n"
        "   <height>32</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a ScriptObject</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The ScriptObject executes scripts and can receive and return values</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString ScriptObjectKmdrDesignerPlugin::includeFile() const
{
  return "scriptobject.h";
}
