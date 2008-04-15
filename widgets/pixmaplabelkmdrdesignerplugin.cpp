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
#include "pixmaplabel.h"
#include "pixmaplabelkmdrdesignerplugin.h"

#include <QtPlugin>

PixmapLabelKmdrDesignerPlugin::PixmapLabelKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void PixmapLabelKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool PixmapLabelKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *PixmapLabelKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new PixmapLabel(parent);
}

QString PixmapLabelKmdrDesignerPlugin::name() const
{
  return "PixmapLabel";
}

QString PixmapLabelKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon PixmapLabelKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString PixmapLabelKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString PixmapLabelKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool PixmapLabelKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString PixmapLabelKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"PixmapLabel\" name=\"PixmapLabel\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>100</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a PixmapLabel</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The PixmapLabel displays images when provided an image address</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString PixmapLabelKmdrDesignerPlugin::includeFile() const
{
  return "pixmaplabel.h";
}
