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
#include "timer.h"
#include "timerkmdrdesignerplugin.h"

#include <QtPlugin>

TimerKmdrDesignerPlugin::TimerKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void TimerKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool TimerKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *TimerKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new Timer(parent);
}

QString TimerKmdrDesignerPlugin::name() const
{
  return "Timer";
}

QString TimerKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon TimerKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString TimerKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString TimerKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool TimerKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString TimerKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"Timer\" name=\"Timer\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>32</width>\n"
        "   <height>32</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a Timer</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The Timer offers repeating or single shot timed actions</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString TimerKmdrDesignerPlugin::includeFile() const
{
  return "timer.h";
}
