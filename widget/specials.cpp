/***************************************************************************
                        specials.cpp - known specials and DCOP calls
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

#include "specials.h"

void SpecialInformation::registerSpecials()
{
  insertGroup(Group::DCOP, "DCOP");
  insert(DCOP::setEnabled, "enableWidget(QString widget, bool enabled)");
  insertAlias(DCOP::setEnabled, "setEnabled");
  insert(DCOP::setVisible, "setVisible(QString widget, bool visible)");
  insert(DCOP::setText, "changeWidgetText(QString widget, QString text)");
  insertAlias(DCOP::setText, "setText");
  insert(DCOP::text, "text(QString widget)");
  insert(DCOP::selection, "selection(QString widget)");
  insert(DCOP::setSelection, "setCurrentListItem(QString widget, QString text)");
  insertAlias(DCOP::setSelection, "setSelection");
  insert(DCOP::currentItem, "currentItem(QString, widget)");
  insert(DCOP::item, "item(QString widget, int index)");
  insert(DCOP::removeItem, "removeListItem(QString widget, int index)");
  insertAlias(DCOP::removeItem, "removeListItem");
  insert(DCOP::insertItem, "addListItem(QString widget, QString item, int index)");
  insertAlias(DCOP::insertItem, "insertItem");
  insert(DCOP::insertItems, "addListItems(QString widget, QStringList items, int index)");
  insertAlias(DCOP::insertItems, "insertItems");
  insert(DCOP::addUniqueItem, "addUniqueItem(QString widget, QString item)");
  insert(DCOP::findItem, "findItem(QString widget, QString item)");
  insert(DCOP::clear, "clearList(QString widget)");
  insertAlias(DCOP::clear, "clear");
  insert(DCOP::setCurrentItem, "setCurrentTab(QString widget, int index)");
  insertAlias(DCOP::setCurrentItem, "setCurrentItem");
  insert(DCOP::setChecked, "setChecked(QString widget, bool checked)");
  insert(DCOP::setAssociatedText, "setAssociatedText(QString,QString)");
  insert(DCOP::associatedText, "associatedText(QString)");
  insert(DCOP::global, "global(QString)");
  insert(DCOP::setGlobal, "setGlobal(QString,QString)");
  
  
  insertGroup(Group::Kommander, QString::null);
  insert(Kommander::widgetText, "widgetText");
  insert(Kommander::selectedWidgetText, "selectedWidgetText");
  insert(Kommander::null, "null");
  insert(Kommander::pid, "pid");
  insert(Kommander::dcopid, "dcopid");
  insert(Kommander::parentPid, "parentPid");
  insert(Kommander::execBegin, "execBegin(QString shell)", 0);
  insert(Kommander::env, "env(QString variable)");
  insert(Kommander::exec, "exec(QString command)");
  insert(Kommander::global, "global(QString variable)");
  insert(Kommander::dialog, "dialog(QString file, QString args)", 1);
  insert(Kommander::readSetting, "readSetting(QString key, QString default)");
  insert(Kommander::setGlobal, "setGlobal(QString variable, QString value)");
  insert(Kommander::writeSetting, "writeSetting(QString key, QString value)");
  insert(Kommander::dcop, "dcop(QString id, QString interface, QString function, QString args)");
  
  
  insertGroup(Group::Array, "Array");
  insert(Array::values, "values(QString array)", 1, 
    "Return EOL-separated list of all values in the array.");
  insert(Array::keys,"keys(QString array)", 1, 
    "Return EOL-separated list of all keys in the array.");
  insert(Array::clear,"clear(QString array)", 1, 
    "Remove all elements from the array.");
  insert(Array::count,"count(QString array)", 1, 
    "Return number of elements in the array.");
  insert(Array::value, "value(QString array, QString key)", 2, 
    "Return the value associated with given key.");
  insert(Array::remove,"remove(QString array, QString key)", 2, 
    "Remove element with given key from the array.");
  insert(Array::setValue,"setValue(QString array, QString key, QString value)", 3, 
    "Add element with given key and value to the array");
  insert(Array::fromString, "fromString(QString array, QString string)", 2, 
    "Add all elements in the string to the array. "
    "String should have <i>key>\\tvalue\\n</i> format.");
  insert(Array::toString, "toString(QString array)", 1, 
    "Return all elements in the array in <i>key>\\tvalue\\n</i> format.");
  
  
  insertGroup(Group::String, "String");
  insert(String::length, "length(QString string)", 1, 
    "Return number of chars in the string.");
  insert(String::contains, "contains(QString string, QString substring)", 2, 
    "Check if the string contains given substring.");
  insert(String::find, "find(QString string, QString sought)", 2, 
    "Return position of a substring in the string, or -1 if it isn't found.");
  insert(String::left, "left(QString string, int n)", 2, 
    "Return first n chars of the string.");
  insert(String::right, "right(QString string, int n)", 2, 
    "Return last n chars of the string.");
  insert(String::mid, "mid(QString string, int start, int n)", 3, 
    "Return n chars of the string, starting from start.");
  insert(String::remove, "remove(QString string, QString substring)", 2, 
    "Replace all occurencies of given substring.");
  insert(String::replace, "replace(QString string, QString substring, QString replacement)", 3, 
    "Replace all occurencies of given substring with given replacement.");
  insert(String::upper, "upper(QString string)", 1, 
    "Convert the string to uppercase.");
  insert(String::lower, "lower(QString string)", 1, 
    "Convert the string to lowercase.");
  insert(String::compare, "compare(QString string1, QString string2)", 2, 
    "Compare two strings. Return 0 if they are equal, "
    "-1 if the first one is lower, 1 if the first one is higher");
  insert(String::isEmpty, "isEmpty(QString string)", 1, 
    "Check if string is empty.");
  insert(String::isNumber, "isNumber(QString string)", 1, 
    "Check if string is a valid number.");
  
  
  insertGroup(Group::File, "File");
  insert(File::read, "read(QString)", 1, 
    "Return content of given file.");
  insert(File::write, "write(QString file, QString string)", 2, 
    "Write given string to a file.");
  insert(File::append, "append(QString file, QString string)", 2, 
    "Append given string to the end of a file.");
}
