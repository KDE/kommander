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
#include "execbutton.h"
#include "execbuttonkmdrdesignerplugin.h"

#include <QtPlugin>

ExecButtonKmdrDesignerPlugin::ExecButtonKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void ExecButtonKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool ExecButtonKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *ExecButtonKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new ExecButton(parent);
}

QString ExecButtonKmdrDesignerPlugin::name() const
{
  return "ExecButton";
}

QString ExecButtonKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon ExecButtonKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString ExecButtonKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString ExecButtonKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool ExecButtonKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString ExecButtonKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"ExecButton\" name=\"ExecButton\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>40</width>\n"
        "   <height>26</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a ExecButton</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The ExecButton runs the script it contains</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString ExecButtonKmdrDesignerPlugin::includeFile() const
{
  return "execbutton.h";
}
