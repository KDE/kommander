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
     i18n("Enables or disables widget."));
  insertAlias(DCOP::setEnabled, "enableWidget");
  insert(DCOP::setVisible, "setVisible(QString widget, bool visible)", 2,
     i18n("Shows/hides widget."));
  insert(DCOP::setText, "setText(QString widget, QString text)", 2,
     i18n("Sets widget's content."));
  insertAlias(DCOP::setText, "changeWidgetText");
  insert(DCOP::text, "text(QString widget)", 1,
     i18n("Returns content of widget."));
  insert(DCOP::selection, "selection(QString widget)", 1,
     i18n("Returns selected text or text of current item."));
  insert(DCOP::setSelection, "setSelection(QString widget, QString text)", 2,
     i18n("Selects given text or select item containing given text."));
  insertAlias(DCOP::setSelection, "setCurrentListItem");
  insert(DCOP::currentItem, "currentItem(QString widget)", 1,
     i18n("Returns index of current item."));
  insert(DCOP::setCurrentItem, "setCurrentItem(QString widget, int index)", 2,
     i18n("Selects the item at the specified index. Indexes are zero based."));
  insertAlias(DCOP::setCurrentItem, "setCurrentTab");
  insert(DCOP::setPixmap, "setPixmap(QString widget, QString iconName, int index)", 3,
     i18n("Sets pixmap at the given index to the specified icon. Use <i>index = -1</-> to set the pixmap for all items."));
  insert(DCOP::item, "item(QString widget, int index)", 2,
     i18n("Returns the text of the item at the given index."));
  insert(DCOP::itemPath, "itemPath(QString widget, int index)", 2,
     i18n("Returns the slash-separated path to the given item in the tree."));
  insert(DCOP::itemDepth, "itemDepth(QString widget, int index)", 2,
     i18n("Returns the depth of the current item in the tree. Root items have depth 0."));
  insert(DCOP::removeItem, "removeItem(QString widget, int index)", 2,
     i18n("Removes the item with the given index."));
  insertAlias(DCOP::removeItem, "removeListItem");
  insert(DCOP::insertItem, "insertItem(QString widget, QString item, int index)", 3,
     i18n("Inserts item at <i>index</i> position."));
  insertAlias(DCOP::insertItem, "addListItem");
  insert(DCOP::insertItems, "insertItems(QString widget, QStringList items, int index)", 3,
     i18n("Inserts multiple items (EOL-separated) at <i>index</i> position."));
  insertAlias(DCOP::insertItems, "addListItems");
  insert(DCOP::addUniqueItem, "addUniqueItem(QString widget, QString item)", 2,
     i18n("Inserts the item if it will not create a duplicate."));
  insert(DCOP::findItem, "findItem(QString widget, QString item)", 2,
     i18n("Returns the index of an item with the given text."));
  insert(DCOP::clear, "clear(QString widget)", 1,
     i18n("Removes all content from the widget."));
  insertAlias(DCOP::clear, "clearList");
  insert(DCOP::setChecked, "setChecked(QString widget, bool checked)", 2,
     i18n("Sets/unsets checkbox."));
  insert(DCOP::setAssociatedText, "setAssociatedText(QString widget, QString text)", 2,
     i18n("Sets scripts associated with widget. This is an advanced feature that would not be commonly used."));
  insert(DCOP::associatedText, "associatedText(QString widget)", 1,
     i18n("Returns scripts associated with widget. This is an advanced feature that would not be commonly used."));
  insert(DCOP::type, "type(QString widget)", 2,
     i18n("Returns type(class) of widget."));
  insert(DCOP::children, "children(QString parent, bool recursive)", 2,
     i18n("Returns the list of child widgets contained in the parent widget. Set the <i>recursive</i> parameter to <i>true</i> to include widgets contained by child widgets."));
  insert(DCOP::global, "global(QString variable)", 1,
     i18n("Returns value of a global variable."));
  insert(DCOP::setGlobal, "setGlobal(QString variable, QString value)", 2,
     i18n("Sets the value of a global variable. Global variables exist for the life of the Kommander window."));
  
  insertGroup(Group::Kommander, "Kommander");
  insert(Kommander::widgetText, "widgetText", 0,
     i18n("Returns current widget's content."));
  insert(Kommander::selectedWidgetText, "selectedWidgetText", 0,
     i18n("Returns selected text or text of current item."));
  insert(Kommander::null, "null", 0,
     i18n("Does nothing. This is useful if you request the @widgetText value from another widget for a CheckBox that returns nothing in it's unchecked state. The @null prevents an error indicating it is empty."));
  insert(Kommander::pid, "pid", 0,
     i18n("Returns the pid (process ID) of the current process."));
  insert(Kommander::dcopid, "dcopid", 0,
     i18n("Returns DCOP identifier of current process. This is shorthand for <i>kmdr-executor-@pid></i>."));
  insert(Kommander::parentPid, "parentPid", 0,
     i18n("Returns the pid of the parent Kommander window."));
  insert(Kommander::execBegin, "execBegin(QString shell)", 0,
     i18n("Executes a script block. Bash is used if no shell is given. It is primarily for use in non-button widgets where script actions are not expected. <p><i>If this is used inside a button it allows alternate script languages to be used and will return a value to the main script, which may be unexpected.</i>"));
  insert(Kommander::env, "env(QString variable)", 1,
     i18n("Returns value of an environment (shell) variable. Do not use <i>$</i> in the name. For example, <i>@env(PATH)</i>."));
  insert(Kommander::exec, "exec(QString command)", 1,
     i18n("Executes an external command."));
  insert(Kommander::forEachBlock, "forEach(QString variable, QString items)", 2,
     i18n("Executes loop: values from <i>items</i> list (passed as EOL-separated string) are assigned "
        "to the variable."));
  insert(Kommander::forBlock, "for(QString variable, int start, int end, int step)", 2,
     i18n("Executes loop: variable is set to <i>start</i> and is increased by <i>step</i> each "
        "time loop is executed. Execution stops when variable becomes larger then <i>end</i>."));
  insert(Kommander::global, "global(QString variable)", 1,
     i18n("Returns the value of a global variable."));
  insert(Kommander::i18n, "i18n(QString variable)", 1,
     i18n("Translates the string into the current language."));
  insert(Kommander::dialog, "dialog(QString file, QString args)", 1,
     i18n("Executes another Kommander dialog. Current dialog directory is used if no path is given. Arguments may be given as named arguments which will become global variables in the new dialog. For instance: <i>var=val</i>"));
  insert(Kommander::readSetting, "readSetting(QString key, QString default)", 2,
     i18n("Reads setting from configration file for this dialog."));
  insert(Kommander::setGlobal, "setGlobal(QString variable, QString value)", 2,
     i18n("Sets the value of a global variable. Global variables exist for the life of the Kommander window."));
  insert(Kommander::writeSetting, "writeSetting(QString key, QString value)", 2,
     i18n("Stores setting in configuration file for this dialog."));
  insert(Kommander::dcop, "dcop(QString id, QString interface, QString function, QString args)", 3,
     i18n("Executes an external DCOP call."));
  
  insertGroup(Group::Array, "Array");
  insert(Array::values, "values(QString array)", 1, 
    i18n("Returns an EOL-separated list of all values in the array."));
  insert(Array::keys,"keys(QString array)", 1, 
    i18n("Returns an EOL-separated list of all keys in the array."));
  insert(Array::clear,"clear(QString array)", 1, 
    i18n("Removes all elements from the array."));
  insert(Array::count,"count(QString array)", 1, 
    i18n("Returns the number of elements in the array."));
  insert(Array::value, "value(QString array, QString key)", 2, 
    i18n("Returns the value associated with the given key."));
  insert(Array::remove,"remove(QString array, QString key)", 2, 
    i18n("Removes element with the given key from the array."));
  insert(Array::setValue,"setValue(QString array, QString key, QString value)", 3, 
    i18n("Adds element with the given key and value to the array"));
  insert(Array::fromString, "fromString(QString array, QString string)", 2, 
    i18n("Adds all elements in the string to the array. "
    "String should have <i>key\\tvalue\\n</i> format."));
  insert(Array::toString, "toString(QString array)", 1, 
    i18n("Returns all elements in the array in <pre>key\\tvalue\\n</pre> format."));
  
  
  insertGroup(Group::String, "String");
  insert(String::length, "length(QString string)", 1, 
    i18n("Returns number of chars in the string."));
  insert(String::contains, "contains(QString string, QString substring)", 2, 
    i18n("Checks if the the string contains the given substring."));
  insert(String::find, "find(QString string, QString sought, int index)", 2, 
    i18n("Returns the position of a substring in the string, or -1 if it isn't found."));
  insert(String::left, "left(QString string, int n)", 2, 
    i18n("Returns the first <i>n</i> chars of the string."));
  insert(String::right, "right(QString string, int n)", 2, 
    i18n("Returns the last <i>n</i> chars of the string."));
  insert(String::mid, "mid(QString string, int start, int n)", 3, 
    i18n("Returns <i>n</i> chars of the string, starting from <i>start</i>."));
  insert(String::remove, "remove(QString string, QString substring)", 2, 
    i18n("Removes all occurencies of given substring."));
  insert(String::replace, "replace(QString string, QString substring, QString replacement)", 3, 
    i18n("Replaces all occurencies of the given substring with the given replacement."));
  insert(String::upper, "upper(QString string)", 1, 
    i18n("Converts the string to uppercase."));
  insert(String::lower, "lower(QString string)", 1, 
    i18n("Converts the string to lowercase."));
  insert(String::compare, "compare(QString string1, QString string2)", 2, 
    i18n("Compares two strings. Returns 0 if they are equal, "
    "-1 if the first one is lower, 1 if the first one is higher"));
  insert(String::isEmpty, "isEmpty(QString string)", 1, 
    i18n("Checks if the string is empty."));
  insert(String::isNumber, "isNumber(QString string)", 1, 
    i18n("Checks if the string is a valid number."));
  insert(String::section, "section(QString string, QString separator, int index)", 1, 
    i18n("Returns given section of a string."));
  insert(String::args, "args(QString string, QString arg1, QString arg2, QString arg3)", 2, 
    i18n("Returns the given string with %1, %2, %3 replaced with <i>arg1</i>, <i>arg2</i>, <i>arg3</i> accordingly."));
  
  
  insertGroup(Group::File, "File");
  insert(File::read, "read(QString)", 1, 
    i18n("Returns content of given file."));
  insert(File::write, "write(QString file, QString string)", 2, 
    i18n("Writes given string to a file."));
  insert(File::append, "append(QString file, QString string)", 2, 
    i18n("Appends given string to the end of a file."));
}
