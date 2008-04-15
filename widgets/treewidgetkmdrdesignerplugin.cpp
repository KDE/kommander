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
#include "treewidget.h"
#include "treewidgetkmdrdesignerplugin.h"

#include <QtPlugin>

TreeWidgetKmdrDesignerPlugin::TreeWidgetKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void TreeWidgetKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool TreeWidgetKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *TreeWidgetKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new TreeWidget(parent);
}

QString TreeWidgetKmdrDesignerPlugin::name() const
{
  return "TreeWidget";
}

QString TreeWidgetKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon TreeWidgetKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString TreeWidgetKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString TreeWidgetKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool TreeWidgetKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString TreeWidgetKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"TreeWidget\" name=\"TreeWidget\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>100</width>\n"
        "   <height>150</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a TreeWidget</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The TreeWidget offers both tree and detail data views</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString TreeWidgetKmdrDesignerPlugin::includeFile() const
{
  return "treewidget.h";
}
