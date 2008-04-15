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
#include "groupbox.h"
#include "groupboxkmdrdesignerplugin.h"

#include <QtPlugin>

GroupBoxKmdrDesignerPlugin::GroupBoxKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void GroupBoxKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool GroupBoxKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *GroupBoxKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new GroupBox(parent);
}

QString GroupBoxKmdrDesignerPlugin::name() const
{
  return "GroupBox";
}

QString GroupBoxKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon GroupBoxKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString GroupBoxKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString GroupBoxKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool GroupBoxKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString GroupBoxKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"GroupBox\" name=\"GroupBox\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>100</width>\n"
        "   <height>50</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a GroupBox</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The GroupBox offers visual grouping of widgets</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString GroupBoxKmdrDesignerPlugin::includeFile() const
{
  return "groupbox.h";
}
