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
#include "konsole.h"
#include "konsolekmdrdesignerplugin.h"

#include <QtPlugin>

KonsoleKmdrDesignerPlugin::KonsoleKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void KonsoleKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool KonsoleKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *KonsoleKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new Konsole(parent);
}

QString KonsoleKmdrDesignerPlugin::name() const
{
  return "Konsole";
}

QString KonsoleKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon KonsoleKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString KonsoleKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString KonsoleKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool KonsoleKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString KonsoleKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"Konsole\" name=\"Konsole\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>250</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a Konsole</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The Konsole runs commands like a console</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString KonsoleKmdrDesignerPlugin::includeFile() const
{
  return "konsole.h";
}
