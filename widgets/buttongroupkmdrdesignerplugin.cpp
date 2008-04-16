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
#include "buttongroup.h"
#include "buttongroupkmdrdesignerplugin.h"

#include <QtPlugin>

ButtonGroupKmdrDesignerPlugin::ButtonGroupKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void ButtonGroupKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool ButtonGroupKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *ButtonGroupKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new ButtonGroup(parent, ""); //TODO: name is not passed, might not be critical though
}

QString ButtonGroupKmdrDesignerPlugin::name() const
{
  return "ButtonGroup";
}

QString ButtonGroupKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon ButtonGroupKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString ButtonGroupKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString ButtonGroupKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool ButtonGroupKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString ButtonGroupKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"ButtonGroup\" name=\"ButtonGroup\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>100</width>\n"
        "   <height>26</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a ButtonGroup</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The ButtonGroup is used to group RadioButtons together</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString ButtonGroupKmdrDesignerPlugin::includeFile() const
{
  return "buttongroup.h";
}
