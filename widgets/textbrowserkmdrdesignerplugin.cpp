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
#include "textbrowser.h"
#include "textbrowserkmdrdesignerplugin.h"

#include <QtPlugin>

TextBrowserKmdrDesignerPlugin::TextBrowserKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void TextBrowserKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool TextBrowserKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *TextBrowserKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new TextBrowser(parent);
}

QString TextBrowserKmdrDesignerPlugin::name() const
{
  return "TextBrowser";
}

QString TextBrowserKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon TextBrowserKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString TextBrowserKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString TextBrowserKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool TextBrowserKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString TextBrowserKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"TextBrowser\" name=\"TextBrowser\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>100</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a TextBrowser</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The TextBrowser offers a limited HTML scope read only widget that can handle links</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString TextBrowserKmdrDesignerPlugin::includeFile() const
{
  return "textbrowser.h";
}
