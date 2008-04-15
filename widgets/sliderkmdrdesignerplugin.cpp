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
#include "slider.h"
#include "sliderkmdrdesignerplugin.h"

#include <QtPlugin>

SliderKmdrDesignerPlugin::SliderKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void SliderKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool SliderKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *SliderKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new Slider(parent);
}

QString SliderKmdrDesignerPlugin::name() const
{
  return "Slider";
}

QString SliderKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon SliderKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString SliderKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString SliderKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool SliderKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString SliderKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"Slider\" name=\"Slider\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>50</width>\n"
        "   <height>32</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a Slider</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The Slider offers visual control over a range of values</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString SliderKmdrDesignerPlugin::includeFile() const
{
  return "slider.h";
}
