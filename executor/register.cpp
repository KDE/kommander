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
  SpecialInformation::insert("widgetText", 0, 0);
  SpecialInformation::insert("selectedWidgetText", 0, 0);
  SpecialInformation::insert("pid", 0, 0);
  SpecialInformation::insert("dcopid", 0, 0);
  SpecialInformation::insert("parentPid", 0, 0);
  SpecialInformation::insert("execBegin", 0, 1);
  SpecialInformation::insert("env", 1, 1);
  SpecialInformation::insert("exec", 1, 1);
  SpecialInformation::insert("global", 1, 1);
  SpecialInformation::insert("dialog", 1, 2);
  SpecialInformation::insert("readSetting", 2, 2);  
  SpecialInformation::insert("setGlobal", 2, 2);
  SpecialInformation::insert("writeSetting", 2, 2);
  SpecialInformation::insert("dcop", 4, 10);
  
  /* array specials */
  SpecialInformation::setCurrentObject("Array");
  SpecialInformation::insert("values", 1, 1, "Return EOL-separated list of all values in the array.");
  SpecialInformation::insert("keys", 1, 1, "Return EOL-separated list of all keys in the array.");
  SpecialInformation::insert("clear", 1, 1, "Remove all elements from the array.");
  SpecialInformation::insert("count", 1, 1, "Return number of elements in the array.");
  SpecialInformation::insert("value", 2, 2, "Return the value associated with given key.");
  SpecialInformation::insert("remove", 2, 2, "Remove element with given key from the array.");
  SpecialInformation::insert("setValue", 3, 3, "Add element with given key and value to the array");
  SpecialInformation::insert("read", 2, 2, "Add all elements in the string to the array. "
    "String should have <i>key>\\tvalue\\n</i> format.");
  SpecialInformation::insert("write", 2, 2, "Return all elements in the array in "
    "<i>key>\\tvalue\\n</i> format.");
  
  SpecialInformation::setCurrentObject("String");
  SpecialInformation::insert("length", 1, 1, "Return number of chars in the string.");
  SpecialInformation::insert("contains", 2, 2, "Check if the string contains given substring.");
  SpecialInformation::insert("find", 2, 2, "Return position of a substring in the string, "
    "or -1 if it isn't found.");
  SpecialInformation::insert("left", 2, 2, "Return first n chars of the string.");
  SpecialInformation::insert("right", 2, 2, "Return last n chars of the string.");
  SpecialInformation::insert("mid", 3, 3, "Return substring of the string, starting from "
    "given position.");
  SpecialInformation::insert("remove", 2, 2, "Replace all occurencies of given substring.");
  SpecialInformation::insert("replace", 3, 3, "Replace all occurencies of given substring "
    "with given replacement.");
  SpecialInformation::insert("upper", 1, 1, "Convert the string to uppercase.");
  SpecialInformation::insert("lower", 1, 1, "Convert the string to lowercase.");
  SpecialInformation::insert("compare", 2, 2, "Compare two strings. Return 0 if they are equal, "
    "-1 if the first one is lower, 1 if the first one is higher");
  SpecialInformation::insert("isEmpty", 1, 1, "Check if string is empty.");
  SpecialInformation::insert("isNumber", 1, 1, "Check if string is a valid number.");
  
  SpecialInformation::setCurrentObject("File");
  SpecialInformation::insert("read", 1, 1, "Return content of given file.");
  SpecialInformation::insert("write", 2, 2, "Write given string to a file.");
  SpecialInformation::insert("append", 2, 2, "Append given string to the end of a file.");
}
