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

#include <klocale.h>
 
#include "specials.h"

void SpecialInformation::registerSpecials()
{
  insertGroup(Group::DCOP, "DCOP");
  insert(DCOP::setEnabled, "setEnabled(QString widget, bool enabled)", 2,
     i18n("Enable or disable widget."));
  insertAlias(DCOP::setEnabled, "enableWidget");
  insert(DCOP::setVisible, "setVisible(QString widget, bool visible)", 2,
     i18n("Show/hide widget."));
  insert(DCOP::setText, "setText(QString widget, QString text)", 2,
     i18n("Set widget's  content."));
  insertAlias(DCOP::setText, "changeWidgetText");
  insert(DCOP::text, "text(QString widget)", 1,
     i18n("Return content of widget."));
  insert(DCOP::selection, "selection(QString widget)", 1,
     i18n("Return selected part of widget."));
  insert(DCOP::setSelection, "setSelection(QString widget, QString text)", 2,
     i18n("Set selected part of widget."));
  insertAlias(DCOP::setSelection, "setCurrentListItem");
  insert(DCOP::currentItem, "currentItem(QString widget)", 1,
     i18n("Return index of current item in widget."));
  insert(DCOP::setCurrentItem, "setCurrentItem(QString widget, int index)", 2,
     i18n("Sets index of current item in widget."));
  insertAlias(DCOP::setCurrentItem, "setCurrentTab");
  insert(DCOP::setPixmap, "setPixmap(QString widget, QString iconName, int index)", 3,
     i18n("Set pixmap for given icon. Use <i>index = -1</-> to set pixmap for all items."));
  insert(DCOP::item, "item(QString widget, int index)", 2,
     i18n("Return item with given index."));
  insert(DCOP::itemPath, "itemPath(QString widget, int index)", 2,
     i18n("Return slash-separated path to given item in tree widget."));
  insert(DCOP::itemDepth, "itemDepth(QString widget, int index)", 2,
     i18n("Return depth of current item in tree widget. Root items have depth 0."));
  insert(DCOP::removeItem, "removeItem(QString widget, int index)", 2,
     i18n("Remove item with given index."));
  insertAlias(DCOP::removeItem, "removeListItem");
  insert(DCOP::insertItem, "insertItem(QString widget, QString item, int index)", 3,
     i18n("Insert item at <i>index</i> position."));
  insertAlias(DCOP::insertItem, "addListItem");
  insert(DCOP::insertItems, "insertItems(QString widget, QStringList items, int index)", 3,
     i18n("Insert multiple items (EOL-separated) at <i>index</i> position."));
  insertAlias(DCOP::insertItems, "addListItems");
  insert(DCOP::addUniqueItem, "addUniqueItem(QString widget, QString item)", 2,
     i18n("Insert item if it doesn't already exists."));
  insert(DCOP::findItem, "findItem(QString widget, QString item)", 2,
     i18n("Return index of item with given text."));
  insert(DCOP::clear, "clear(QString widget)", 1,
     i18n("Remove content of widget."));
  insertAlias(DCOP::clear, "clearList");
  insert(DCOP::setChecked, "setChecked(QString widget, bool checked)", 2,
     i18n("Set/unset checkbox."));
  insert(DCOP::setAssociatedText, "setAssociatedText(QString widget, QString text)", 2,
     i18n("Set scripts associated with widget. You probably don't need to use this."));
  insert(DCOP::associatedText, "associatedText(QString widget)", 1,
     i18n("Return scripts associated with widget. You probably don't need to use this."));
  insert(DCOP::type, "type(QString widget)", 2,
     i18n("Return type(class) of widget."));
  insert(DCOP::children, "children(QString parent, bool recursive)", 2,
     i18n("Return list (optionally recursive) of widgets contained in widget."));
  insert(DCOP::global, "global(QString variable)", 1,
     i18n("Return value of a global variable."));
  insert(DCOP::setGlobal, "setGlobal(QString variable, QString value)", 2,
     i18n("Set value of a global variable."));
  
  insertGroup(Group::Kommander, "Kommander");
  insert(Kommander::widgetText, "widgetText", 0,
     i18n("Return current widget's content."));
  insert(Kommander::selectedWidgetText, "selectedWidgetText", 0,
     i18n("Return selected part of current widget's content."));
  insert(Kommander::null, "null", 0,
     i18n("Do nothing. Used when script is expected and empty text is not enough."));
  insert(Kommander::pid, "pid", 0,
     i18n("Return pid of current process."));
  insert(Kommander::dcopid, "dcopid", 0,
     i18n("Return DCOP identifier of current process."));
  insert(Kommander::parentPid, "parentPid", 0,
     i18n("Return pid of a parent dialog."));
  insert(Kommander::execBegin, "execBegin(QString shell)", 0,
     i18n("Execute block. Bash is used if no shell is given."));
  insert(Kommander::env, "env(QString variable)", 1,
     i18n("Return value of environmental (shell) variable."));
  insert(Kommander::exec, "exec(QString command)", 1,
     i18n("Execute external command."));
  insert(Kommander::forEachBlock, "forEach(QString variable, QString items)", 2,
     i18n("Execute loop: values from <i>items</i> list (passed as EOL-separated string) are assigned "
        "to the variable."));
  insert(Kommander::forBlock, "for(QString variable, int start, int end, int step)", 2,
     i18n("Execute loop: variable is set to <i>start</i> and is increased by <i>step</i> each "
        "time loop is executed. Execution stops when variable becomes larger then <i>end</i>."));
  insert(Kommander::global, "global(QString variable)", 1,
     i18n("Return value of a global variable."));
  insert(Kommander::i18n, "i18n(QString variable)", 1,
     i18n("Translate string into current language."));
  insert(Kommander::dialog, "dialog(QString file, QString args)", 1,
     i18n("Execute another Kommander dialog. Current dialog directory is used if no path is given."));
  insert(Kommander::readSetting, "readSetting(QString key, QString default)", 2,
     i18n("Read setting from configration file for this dialog."));
  insert(Kommander::setGlobal, "setGlobal(QString variable, QString value)", 2,
     i18n("Set value of a global variable."));
  insert(Kommander::writeSetting, "writeSetting(QString key, QString value)", 2,
     i18n("Store setting in configuration file for this dialog."));
  insert(Kommander::dcop, "dcop(QString id, QString interface, QString function, QString args)", 3,
     i18n("Execute external DCOP call."));
  
  insertGroup(Group::Array, "Array");
  insert(Array::values, "values(QString array)", 1, 
    i18n("Return EOL-separated list of all values in the array."));
  insert(Array::keys,"keys(QString array)", 1, 
    i18n("Return EOL-separated list of all keys in the array."));
  insert(Array::clear,"clear(QString array)", 1, 
    i18n("Remove all elements from the array."));
  insert(Array::count,"count(QString array)", 1, 
    i18n("Return number of elements in the array."));
  insert(Array::value, "value(QString array, QString key)", 2, 
    i18n("Return the value associated with given key."));
  insert(Array::remove,"remove(QString array, QString key)", 2, 
    i18n("Remove element with given key from the array."));
  insert(Array::setValue,"setValue(QString array, QString key, QString value)", 3, 
    i18n("Add element with given key and value to the array"));
  insert(Array::fromString, "fromString(QString array, QString string)", 2, 
    i18n("Add all elements in the string to the array. "
    "String should have <i>key>\\tvalue\\n</i> format."));
  insert(Array::toString, "toString(QString array)", 1, 
    i18n("Return all elements in the array in <i>key>\\tvalue\\n</i> format."));
  
  
  insertGroup(Group::String, "String");
  insert(String::length, "length(QString string)", 1, 
    i18n("Return number of chars in the string."));
  insert(String::contains, "contains(QString string, QString substring)", 2, 
    i18n("Check if the string contains given substring."));
  insert(String::find, "find(QString string, QString sought, int index)", 2, 
    i18n("Return position of a substring in the string, or -1 if it isn't found."));
  insert(String::left, "left(QString string, int n)", 2, 
    i18n("Return first n chars of the string."));
  insert(String::right, "right(QString string, int n)", 2, 
    i18n("Return last n chars of the string."));
  insert(String::mid, "mid(QString string, int start, int n)", 3, 
    i18n("Return n chars of the string, starting from start."));
  insert(String::remove, "remove(QString string, QString substring)", 2, 
    i18n("Replace all occurencies of given substring."));
  insert(String::replace, "replace(QString string, QString substring, QString replacement)", 3, 
    i18n("Replace all occurencies of given substring with given replacement."));
  insert(String::upper, "upper(QString string)", 1, 
    i18n("Convert the string to uppercase."));
  insert(String::lower, "lower(QString string)", 1, 
    i18n("Convert the string to lowercase."));
  insert(String::compare, "compare(QString string1, QString string2)", 2, 
    i18n("Compare two strings. Return 0 if they are equal, "
    "-1 if the first one is lower, 1 if the first one is higher"));
  insert(String::isEmpty, "isEmpty(QString string)", 1, 
    i18n("Check if string is empty."));
  insert(String::isNumber, "isNumber(QString string)", 1, 
    i18n("Check if string is a valid number."));
  insert(String::section, "section(QString string, QString separator, int index)", 1, 
    i18n("Returns given section of a string."));
  
  
  insertGroup(Group::File, "File");
  insert(File::read, "read(QString)", 1, 
    i18n("Return content of given file."));
  insert(File::write, "write(QString file, QString string)", 2, 
    i18n("Write given string to a file."));
  insert(File::append, "append(QString file, QString string)", 2, 
    i18n("Append given string to the end of a file."));
}
