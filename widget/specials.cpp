/***************************************************************************
                        specials.cpp - known specials and DCOP calls
                             -------------------
    copyright            : (C) 2004 by Michal Rudolf <mrudolf@kdewebdev.org>
    copyright            : (C) 2004 by Eric Laffoon <sequitur@kde.org>
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

#if 0
#define NEW_STRING(x) i18n(x)
#else
#define NEW_STRING(x) QString::null
#endif

 
#include "specials.h"

void SpecialInformation::registerSpecials()
{
  insertGroup(Group::DCOP, "DCOP");
  insert(DCOP::setEnabled, "setEnabled(QString widget, bool enabled)", 
     i18n("Enables or disables widget."), 2);
  insertAlias(DCOP::setEnabled, "enableWidget");
  insert(DCOP::setVisible, "setVisible(QString widget, bool visible)", 
     i18n("Shows/hides widget."), 2);
  insert(DCOP::setText, "setText(QString widget, QString text)", 
     i18n("Sets widget's content."), 2);
  insertAlias(DCOP::setText, "changeWidgetText");
  insert(DCOP::text, "text(QString widget)", i18n("Returns content of widget."), 1);
  insert(DCOP::selection, "selection(QString widget)", 
     i18n("Returns selected text or text of current item."), 1);
  insert(DCOP::setSelection, "setSelection(QString widget, QString text)", 
     i18n("Selects given text or select item containing given text."), 2);
  insertAlias(DCOP::setSelection, "setCurrentListItem");
  insert(DCOP::currentItem, "currentItem(QString widget)", 
     i18n("Returns index of current item."), 1);
  insert(DCOP::setCurrentItem, "setCurrentItem(QString widget, int index)",
     i18n("Selects the item at the specified index. Indexes are zero based."), 2);
  insertAlias(DCOP::setCurrentItem, "setCurrentTab");
  insert(DCOP::setPixmap, "setPixmap(QString widget, QString iconName, int index)",
     i18n("Sets pixmap at the given index to the specified icon. Use <i>index = -1</i> to set the pixmap for all items."), 3);
  insert(DCOP::item, "item(QString widget, int index)",
     i18n("Returns the text of the item at the given index."), 2);
  insert(DCOP::itemPath, "itemPath(QString widget, int index)",
     i18n("Returns the slash-separated path to the given item in the tree."), 2);
  insert(DCOP::itemDepth, "itemDepth(QString widget, int index)",
     i18n("Returns the depth of the current item in the tree. Root items have depth 0."), 2);
  insert(DCOP::removeItem, "removeItem(QString widget, int index)",
     i18n("Removes the item with the given index."), 2);
  insertAlias(DCOP::removeItem, "removeListItem");
  insert(DCOP::insertItem, "insertItem(QString widget, QString item, int index)",
     i18n("Inserts item at <i>index</i> position."), 3);
  insertAlias(DCOP::insertItem, "addListItem");
  insert(DCOP::insertItems, "insertItems(QString widget, QStringList items, int index)",
     i18n("Inserts multiple items (EOL-separated) at <i>index</i> position."), 3);
  insertAlias(DCOP::insertItems, "addListItems");
  insert(DCOP::addUniqueItem, "addUniqueItem(QString widget, QString item)",
     i18n("Inserts the item if it will not create a duplicate."), 2);
  insert(DCOP::findItem, "findItem(QString widget, QString item)",
     i18n("Returns the index of an item with the given text."), 2);
  insert(DCOP::clear, "clear(QString widget)",
     i18n("Removes all content from the widget."), 1);
  insertAlias(DCOP::clear, "clearList");
  insert(DCOP::setChecked, "setChecked(QString widget, bool checked)",
     i18n("Sets/unsets checkbox."), 2);
  insert(DCOP::setAssociatedText, "setAssociatedText(QString widget, QString text)",
     i18n("Sets scripts associated with widget. This is an advanced feature that would not be commonly used."), 2);
  insert(DCOP::associatedText, "associatedText(QString widget)",
     i18n("Returns scripts associated with widget. This is an advanced feature that would not be commonly used."), 1);
  insert(DCOP::type, "type(QString widget)",
     i18n("Returns type(class) of widget."), 2);
  insert(DCOP::children, "children(QString widget, bool recursive)",
     i18n("Returns the list of child widgets contained in the parent widget. Set the <i>recursive</i> parameter to <i>true</i> to include widgets contained by child widgets."), 2);
  insert(DCOP::global, "global(QString variable)",
     i18n("Returns value of a global variable."), 1);
  insert(DCOP::setGlobal, "setGlobal(QString variable, QString value)",
     i18n("Sets the value of a global variable. Global variables exist for the life of the Kommander window."), 2);
  /* functions with missing description - to be added after string freeze */
  insert(DCOP::setMaximum, "setMaximum(QString widget, int value)",
     NEW_STRING("Sets maximum numeric value"), 2);
  
  insertGroup(Group::Kommander, "Kommander");
  insert(Kommander::widgetText, "widgetText", i18n("Returns current widget's content."), 0);
  insert(Kommander::selectedWidgetText, "selectedWidgetText", 
    i18n("Returns selected text or text of current item."), 0);
  insert(Kommander::null, "null", 
     i18n("Does nothing. This is useful if you request the @widgetText value from another widget for a CheckBox that returns nothing in its unchecked state. The @null prevents an error indicating it is empty."), 0);
  insert(Kommander::pid, "pid", 
     i18n("Returns the pid (process ID) of the current process."), 0);
  insert(Kommander::dcopid, "dcopid", 
     i18n("Returns DCOP identifier of current process. This is shorthand for <i>kmdr-executor-@pid></i>."), 
     0);
  insert(Kommander::parentPid, "parentPid", 
     i18n("Returns the pid of the parent Kommander window."), 0);
  insert(Kommander::execBegin, "execBegin(QString shell)", 
     i18n("Executes a script block. Bash is used if no shell is given. It is primarily for use in non-button widgets where script actions are not expected. <p><i>If this is used inside a button it allows alternate script languages to be used and will return a value to the main script, which may be unexpected.</i>"), 0);
  insert(Kommander::env, "env(QString variable)",
     i18n("Returns value of an environment (shell) variable. Do not use <i>$</i> in the name. For example, <i>@env(PATH)</i>."), 1);
  insert(Kommander::exec, "exec(QString command)",
     i18n("Executes an external command."), 1);
  insert(Kommander::expr, "expr(QString expression)",
     NEW_STRING("Parses an expression and returns computed value."), 1);
  insert(Kommander::forEachBlock, "forEach(QString variable, QString items)",
     i18n("Executes loop: values from <i>items</i> list (passed as EOL-separated string) are assigned "
        "to the variable."), 2);
  insert(Kommander::forBlock, "for(QString variable, int start, int end, int step)",
     i18n("Executes loop: variable is set to <i>start</i> and is increased by <i>step</i> each "
        "time loop is executed. Execution stops when variable becomes larger then <i>end</i>."), 2);
  insert(Kommander::global, "global(QString variable)",
     i18n("Returns the value of a global variable."), 1);
  insert(Kommander::i18n, "i18n(QString variable)",
     i18n("Translates the string into the current language. Texts in GUI would be automatically extracted for translation."), 1);
  insert(Kommander::dialog, "dialog(QString file, QString args)",
     i18n("Executes another Kommander dialog. Current dialog directory is used if no path is given. Arguments may be given as named arguments which will become global variables in the new dialog. For instance: <i>var=val</i>"), 1);
  insert(Kommander::readSetting, "readSetting(QString key, QString default)",
     i18n("Reads setting from configration file for this dialog."), 2);
  insert(Kommander::setGlobal, "setGlobal(QString variable, QString value)",
     i18n("Sets the value of a global variable. Global variables exist for the life of the Kommander window."), 2);
  insert(Kommander::writeSetting, "writeSetting(QString key, QString value)",
     i18n("Stores setting in configuration file for this dialog."), 2);
  insert(Kommander::dcop, "dcop(QString id, QString interface, QString function, QString args)",
     i18n("Executes an external DCOP call."), 3, 9);
  
  insertGroup(Group::Array, "Array");
  insert(Array::values, "values(QString array)", 
    i18n("Returns an EOL-separated list of all values in the array."), 1);
  insert(Array::keys,"keys(QString array)", 
    i18n("Returns an EOL-separated list of all keys in the array."), 1);
  insert(Array::clear,"clear(QString array)", 
    i18n("Removes all elements from the array."), 1);
  insert(Array::count,"count(QString array)", 
    i18n("Returns the number of elements in the array."), 1);
  insert(Array::value, "value(QString array, QString key)", 
    i18n("Returns the value associated with the given key."), 2);
  insert(Array::remove,"remove(QString array, QString key)", 
    i18n("Removes element with the given key from the array."), 2);
  insert(Array::setValue,"setValue(QString array, QString key, QString value)", 
    i18n("Adds element with the given key and value to the array"), 3);
  insert(Array::fromString, "fromString(QString array, QString string)", 
    i18n("Adds all elements in the string to the array. "
    "String should have <i>key\\tvalue\\n</i> format."), 2);
  insert(Array::toString, "toString(QString array)", 
    i18n("Returns all elements in the array in <pre>key\\tvalue\\n</pre> format."), 1);
  
  
  insertGroup(Group::String, "String");
  insert(String::length, "length(QString string)", 
    i18n("Returns number of chars in the string."), 1);
  insert(String::contains, "contains(QString string, QString substring)", 
    i18n("Checks if the the string contains the given substring."), 2);
  insert(String::find, "find(QString string, QString sought, int index)", 
    i18n("Returns the position of a substring in the string, or -1 if it is not found."), 2);
  insert(String::left, "left(QString string, int n)", 
    i18n("Returns the first <i>n</i> chars of the string."), 2);
  insert(String::right, "right(QString string, int n)", 
    i18n("Returns the last <i>n</i> chars of the string."), 2);
  insert(String::mid, "mid(QString string, int start, int n)", 
    i18n("Returns <i>n</i> chars of the string, starting from <i>start</i>."), 3);
  insert(String::remove, "remove(QString string, QString substring)", 
    i18n("Removes all occurrences of given substring."), 2);
  insert(String::replace, "replace(QString string, QString substring, QString replacement)", 
    i18n("Replaces all occurrences of the given substring with the given replacement."), 3);
  insert(String::upper, "upper(QString string)", 
    i18n("Converts the string to uppercase."), 1);
  insert(String::lower, "lower(QString string)", 
    i18n("Converts the string to lowercase."), 1);
  insert(String::compare, "compare(QString string1, QString string2)", 
    i18n("Compares two strings. Returns 0 if they are equal, "
    "-1 if the first one is lower, 1 if the first one is higher"), 2);
  insert(String::isEmpty, "isEmpty(QString string)", 
    i18n("Checks if the string is empty."), 1);
  insert(String::isNumber, "isNumber(QString string)", 
    i18n("Checks if the string is a valid number."), 1);
  insert(String::section, "section(QString string, QString separator, int index)", 
    i18n("Returns given section of a string."), 1);
  insert(String::args, "args(QString string, QString arg1, QString arg2, QString arg3)", 
    i18n("Returns the given string with %1, %2, %3 replaced with <i>arg1</i>, <i>arg2</i>, <i>arg3</i> accordingly."), 2);
  
  
  insertGroup(Group::File, "File");
  insert(File::read, "read(QString)", 
    i18n("Returns content of given file."), 1);
  insert(File::write, "write(QString file, QString string)", 
    i18n("Writes given string to a file."), 2);
  insert(File::append, "append(QString file, QString string)", 
    i18n("Appends given string to the end of a file."), 2);
}
