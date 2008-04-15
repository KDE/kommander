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
#include "fileselector.h"
#include "fileselectorkmdrdesignerplugin.h"

#include <QtPlugin>

FileSelectorKmdrDesignerPlugin::FileSelectorKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void FileSelectorKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool FileSelectorKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *FileSelectorKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new FileSelector(parent);
}

QString FileSelectorKmdrDesignerPlugin::name() const
{
  return "FileSelector";
}

QString FileSelectorKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon FileSelectorKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString FileSelectorKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString FileSelectorKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool FileSelectorKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString FileSelectorKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"FileSelector\" name=\"FileSelector\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>50</width>\n"
        "   <height>26</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a FileSelector</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The FileSelector provides an on dialog display of selections</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString FileSelectorKmdrDesignerPlugin::includeFile() const
{
  return "fileselector.h";
}
