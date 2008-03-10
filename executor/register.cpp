/***************************************************************************
        register.cpp  -  registration of functions/specials for intance
                             -------------------
    copyright            : (C) 2004 by Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "instance.h"
#include <dcopinformation.h>
#include <specialinformation.h>

#include <klocale.h>

void Instance::registerDCOP()
{
  DCOPInformation::insert("enableWidget(QString,bool)");
  DCOPInformation::insert("changeWidgetText(QString,QString)");
  DCOPInformation::insert("currentItem(QString)");
  DCOPInformation::insert("item(QString,int)");
  DCOPInformation::insert("removeListItem(QString,int)");
  DCOPInformation::insert("addListItem(QString,QString,int)");
  DCOPInformation::insert("addListItems(QString,QStringList,int)");
  DCOPInformation::insert("addUniqueItem(QString,QString)");
  DCOPInformation::insert("findItem(QString,QString)");
  DCOPInformation::insert("clearList(QString)");
  DCOPInformation::insert("setCurrentListItem(QString,QString)");
  DCOPInformation::insert("setCurrentTab(QString,int)");
  DCOPInformation::insert("setChecked(QString, bool)");
  DCOPInformation::insert("setAssociatedText(QString,QString)");
  DCOPInformation::insert("associatedText(QString)");
  DCOPInformation::insert("global(QString)");
  DCOPInformation::insert("setGlobal(QString,QString)");
}
