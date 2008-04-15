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
#include "listbox.h"
#include "listboxkmdrdesignerplugin.h"

#include <QtPlugin>

ListBoxKmdrDesignerPlugin::ListBoxKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void ListBoxKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool ListBoxKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *ListBoxKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new ListBox(parent);
}

QString ListBoxKmdrDesignerPlugin::name() const
{
  return "ListBox";
}

QString ListBoxKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon ListBoxKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString ListBoxKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString ListBoxKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool ListBoxKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString ListBoxKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"ListBox\" name=\"ListBox\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>50</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a ListBox</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The ListBox offers a visual list to select items from</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString ListBoxKmdrDesignerPlugin::includeFile() const
{
  return "listbox.h";
}
