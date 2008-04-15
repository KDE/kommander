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
#include "wizard.h"
#include "wizardkmdrdesignerplugin.h"

#include <QtPlugin>

WizardKmdrDesignerPlugin::WizardKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void WizardKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool WizardKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *WizardKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new Wizard(parent);
}

QString WizardKmdrDesignerPlugin::name() const
{
  return "Wizard";
}

QString WizardKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon WizardKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString WizardKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString WizardKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool WizardKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString WizardKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"Wizard\" name=\"Wizard\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>400</width>\n"
        "   <height>300</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a Wizard</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The Wizard steps users through a process</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString WizardKmdrDesignerPlugin::includeFile() const
{
  return "wizard.h";
}
