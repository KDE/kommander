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
#include "textedit.h"
#include "texteditkmdrdesignerplugin.h"

#include <QtPlugin>

TextEditKmdrDesignerPlugin::TextEditKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void TextEditKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool TextEditKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *TextEditKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new TextEdit(parent);
}

QString TextEditKmdrDesignerPlugin::name() const
{
  return "TextEdit";
}

QString TextEditKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon TextEditKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString TextEditKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString TextEditKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool TextEditKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString TextEditKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"TextEdit\" name=\"TextEdit\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>100</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a TextEdit</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The TextEdit offers multi-line data management that can function as a limited WYSYWYG type HTML editor</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString TextEditKmdrDesignerPlugin::includeFile() const
{
  return "textedit.h";
}
