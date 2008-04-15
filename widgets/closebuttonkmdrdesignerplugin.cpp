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
#include "closebutton.h"
#include "closebuttonkmdrdesignerplugin.h"

#include <QtPlugin>

CloseButtonKmdrDesignerPlugin::CloseButtonKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void CloseButtonKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool CloseButtonKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *CloseButtonKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new CloseButton(parent);
}

QString CloseButtonKmdrDesignerPlugin::name() const
{
  return "CloseButton";
}

QString CloseButtonKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon CloseButtonKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString CloseButtonKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString CloseButtonKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool CloseButtonKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString CloseButtonKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"CloseButton\" name=\"CloseButton\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>50</width>\n"
        "   <height>26</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a CloseButton</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The CloseButton performs the operations you write into it and then closes the dialog</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString CloseButtonKmdrDesignerPlugin::includeFile() const
{
  return "closebutton.h";
}
