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
#include "label.h"
#include "labelkmdrdesignerplugin.h"

#include <QtPlugin>

LabelKmdrDesignerPlugin::LabelKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void LabelKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool LabelKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *LabelKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new Label(parent);
}

QString LabelKmdrDesignerPlugin::name() const
{
  return "Label";
}

QString LabelKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon LabelKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString LabelKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString LabelKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool LabelKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString LabelKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"Label\" name=\"Label\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>40</width>\n"
        "   <height>24</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a Label</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>use labels to help end users identify widgets</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString LabelKmdrDesignerPlugin::includeFile() const
{
  return "label.h";
}
