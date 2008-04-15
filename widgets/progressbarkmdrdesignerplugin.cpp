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
#include "progressbar.h"
#include "progressbarkmdrdesignerplugin.h"

#include <QtPlugin>

ProgressBarKmdrDesignerPlugin::ProgressBarKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void ProgressBarKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool ProgressBarKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *ProgressBarKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new ProgressBar(parent);
}

QString ProgressBarKmdrDesignerPlugin::name() const
{
  return "ProgressBar";
}

QString ProgressBarKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon ProgressBarKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString ProgressBarKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString ProgressBarKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool ProgressBarKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString ProgressBarKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"ProgressBar\" name=\"ProgressBar\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>50</width>\n"
        "   <height>32</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a ProgressBar</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The ProgressBar gives visual feedback of loop completion</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString ProgressBarKmdrDesignerPlugin::includeFile() const
{
  return "progressbar.h";
}
