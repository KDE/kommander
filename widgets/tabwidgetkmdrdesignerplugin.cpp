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
#include "tabwidget.h"
#include "tabwidgetkmdrdesignerplugin.h"

#include <QtPlugin>

TabWidgetKmdrDesignerPlugin::TabWidgetKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void TabWidgetKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool TabWidgetKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *TabWidgetKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new TabWidget(parent);
}

QString TabWidgetKmdrDesignerPlugin::name() const
{
  return "TabWidget";
}

QString TabWidgetKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon TabWidgetKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString TabWidgetKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString TabWidgetKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool TabWidgetKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString TabWidgetKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"TabWidget\" name=\"TabWidget\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>100</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a TabWidget</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The TabWidget provides tabbed areas for widgets</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString TabWidgetKmdrDesignerPlugin::includeFile() const
{
  return "tabwidget.h";
}
