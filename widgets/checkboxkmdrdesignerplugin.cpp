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
#include "checkbox.h"
#include "checkboxkmdrdesignerplugin.h"

#include <QtPlugin>

CheckBoxKmdrDesignerPlugin::CheckBoxKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void CheckBoxKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool CheckBoxKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *CheckBoxKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new CheckBox(parent);
}

QString CheckBoxKmdrDesignerPlugin::name() const
{
  return "CheckBox";
}

QString CheckBoxKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon CheckBoxKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString CheckBoxKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString CheckBoxKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool CheckBoxKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString CheckBoxKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"CheckBox\" name=\"CheckBox\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>50</width>\n"
        "   <height>26</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a CheckBox</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The CheckBox is used for True/False conditions</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString CheckBoxKmdrDesignerPlugin::includeFile() const
{
  return "checkbox.h";
}
