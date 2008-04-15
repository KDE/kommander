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
#include "combobox.h"
#include "comboboxkmdrdesignerplugin.h"

#include <QtPlugin>

ComboBoxKmdrDesignerPlugin::ComboBoxKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void ComboBoxKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool ComboBoxKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *ComboBoxKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new ComboBox(parent);
}

QString ComboBoxKmdrDesignerPlugin::name() const
{
  return "ComboBox";
}

QString ComboBoxKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon ComboBoxKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString ComboBoxKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString ComboBoxKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool ComboBoxKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString ComboBoxKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"ComboBox\" name=\"ComboBox\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>50</width>\n"
        "   <height>26</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a ComboBox</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The ComboBox drops down a list to choose values from</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString ComboBoxKmdrDesignerPlugin::includeFile() const
{
  return "combobox.h";
}
