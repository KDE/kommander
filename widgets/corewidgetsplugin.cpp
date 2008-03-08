/***************************************************************************
 *   Copyright (C) 2008 by Andras Mantia   *
 *   amantia@kde.org   *
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

#include "htmlpart.h"

#include <kommanderplugin.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include "htmlpart.h"

class HtmlPartPlugin : public KommanderPlugin
{
public:
    HtmlPartPlugin();
    virtual QWidget *create( const QString &className, QWidget *parent = 0, const char *name = 0);
};

HtmlPartPlugin::HtmlPartPlugin()
{
    addWidget( "HtmlPart", "Custom", i18n("Kommander HTML part plugin."), new QIconSet(KGlobal::iconLoader()->loadIcon("konqueror", KIcon::NoGroup, KIcon::SizeMedium)) );
}

QWidget *HtmlPartPlugin::create( const QString &className, QWidget *parent, const char *name)
{
  if (className == "HtmlPart")
    return new HtmlPart(parent, name);
  return 0;
}

KOMMANDER_EXPORT_PLUGIN(HtmlPartPlugin)
