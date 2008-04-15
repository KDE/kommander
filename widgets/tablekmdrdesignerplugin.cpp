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
#include "table.h"
#include "tablekmdrdesignerplugin.h"

#include <QtPlugin>

TableKmdrDesignerPlugin::TableKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void TableKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool TableKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *TableKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new Table(parent);
}

QString TableKmdrDesignerPlugin::name() const
{
  return "Table";
}

QString TableKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon TableKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString TableKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString TableKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool TableKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString TableKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"Table\" name=\"Table\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>100</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a Table</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The Table offers a spreadsheet like grid</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString TableKmdrDesignerPlugin::includeFile() const
{
  return "table.h";
}
