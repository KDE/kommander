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
#include "fontdialog.h"
#include "fontdialogkmdrdesignerplugin.h"

#include <QtPlugin>

FontDialogKmdrDesignerPlugin::FontDialogKmdrDesignerPlugin(QObject *parent): QObject(parent)
{
  initialized = false;
}

void FontDialogKmdrDesignerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool FontDialogKmdrDesignerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *FontDialogKmdrDesignerPlugin::createWidget(QWidget *parent)
{
  return new FontDialog(parent);
}

QString FontDialogKmdrDesignerPlugin::name() const
{
  return "FontDialog";
}

QString FontDialogKmdrDesignerPlugin::group() const
{
  return "Display Widgets [Kommander]";
}

QIcon FontDialogKmdrDesignerPlugin::icon() const
{
  return QIcon();
}

QString FontDialogKmdrDesignerPlugin::toolTip() const
{
  return "";
}

QString FontDialogKmdrDesignerPlugin::whatsThis() const
{
  return "";
}

bool FontDialogKmdrDesignerPlugin::isContainer() const
{
  return false;
}

QString FontDialogKmdrDesignerPlugin::domXml() const
{
  return "<widget class=\"FontDialog\" name=\"FontDialog\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>32</width>\n"
        "   <height>32</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Insert a FontDialog</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>The FontDialog enables use of the standard KDE font features</string>\n"
        " </property>\n"
        "</widget>\n";
}

QString FontDialogKmdrDesignerPlugin::includeFile() const
{
  return "fontdialog.h";
}
