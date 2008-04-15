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
#include "spinboxint.h"
#include "spinboxintkmdrdesignerplugin.h"

#include <QtPlugin>

SpinBoxIntKmdrDesignerPlugin::SpinBoxIntKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void SpinBoxIntKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool SpinBoxIntKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *SpinBoxIntKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new SpinBoxInt(parent);
}

QString SpinBoxIntKmdrDesignerPlugin::name() const
{
  return "SpinBoxInt";
}

QString SpinBoxIntKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon SpinBoxIntKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString SpinBoxIntKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString SpinBoxIntKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool SpinBoxIntKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString SpinBoxIntKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"SpinBoxInt\" name=\"SpinBoxInt\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>40</width>\n"
        "   <height>24</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a SpinBoxInt</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The SpinBoxInt offers integer based data management</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString SpinBoxIntKmdrDesignerPlugin::includeFile() const
{
  return "spinboxint.h";
}
