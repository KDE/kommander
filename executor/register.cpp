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

void Instance::registerSpecials()
{
  /* global specials */
  SpecialInformation::insert("widgetText");
  SpecialInformation::insert("selectedWidgetText");
  SpecialInformation::insert("pid");
  SpecialInformation::insert("dcopid");
  SpecialInformation::insert("parentPid");
  SpecialInformation::insert("execBegin", 0, 1);
  SpecialInformation::insert("env", 1);
  SpecialInformation::insert("exec", 1);
  SpecialInformation::insert("global", 1);
  SpecialInformation::insert("dialog", 1, 2);
  SpecialInformation::insert("readSetting", 2);  
  SpecialInformation::insert("setGlobal", 2);
  SpecialInformation::insert("writeSetting", 2);
  SpecialInformation::insert("dcop", 4, 10);
  
  /* array specials */
  SpecialInformation::setCurrentObject("Array");
  SpecialInformation::insert("values", 1);
  SpecialInformation::insert("keys", 1);
  SpecialInformation::insert("clear", 1);
  SpecialInformation::insert("count", 1);
  SpecialInformation::insert("value", 2);
  SpecialInformation::insert("remove", 2);
  SpecialInformation::insert("setValue", 3);
  SpecialInformation::insert("read", 2);
  SpecialInformation::insert("write", 2);
  
  SpecialInformation::setCurrentObject("String");
  SpecialInformation::insert("length", 1);
  SpecialInformation::insert("contains", 2);
  SpecialInformation::insert("find", 2);
  SpecialInformation::insert("left", 2);
  SpecialInformation::insert("right", 2);
  SpecialInformation::insert("mid", 3);
  SpecialInformation::insert("remove", 2);
  SpecialInformation::insert("replace", 3);
  SpecialInformation::insert("lower", 1);
  SpecialInformation::insert("upper", 1);
  SpecialInformation::insert("lower", 1);
  SpecialInformation::insert("compare", 2);
  SpecialInformation::insert("isEmpty", 1);
  SpecialInformation::insert("isNumber", 1);
  
  SpecialInformation::setCurrentObject("File");
  SpecialInformation::insert("read", 1);
  SpecialInformation::insert("write", 2);
  SpecialInformation::insert("append", 2);
}
