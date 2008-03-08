/***************************************************************************
                        specials.cpp - known specials and DCOP calls
                             -------------------
    copyright            : (C) 2004-2005 by Michal Rudolf <mrudolf@kdewebdev.org>
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
#include "specials.h"

void SpecialInformation::registerSpecials()
{
  insertGroup(Group::DBUS, "DBUS", "");
  insert(DBUS::addUniqueItem, "addUniqueItem(QString widget, QString item)",
         i18n("Inserts the item if it will not create a duplicate."), 2);
  insert(DBUS::associatedText, "associatedText(QString widget)",
         i18n("Returns scripts associated with widget. This is an advanced feature that would not be commonly used."), 1);
  insert(DBUS::cancel, "cancel(QString widget)", i18n("Stops execution of the script associated with the widget."), 1);
  insert(DBUS::cellText, "cellText(QString widget, int row, int column)",
         i18n("Returns text of a cell in a table."), 3);
  insert(DBUS::checked, "checked(QString widget)",
         i18n("Returns 1 for checked boxes, 0 for unchecked."), 1);
  insert(DBUS::children, "children(QString widget, bool recursive)",
         i18n("Returns the list of child widgets contained in the parent widget. Set the <i>recursive</i> parameter to <i>true</i> to include widgets contained by child widgets."), 2);
  insert(DBUS::clear, "clear(QString widget)",
         i18n("Removes all content from the widget."), 1);
  insertAlias(DBUS::clear, "clearList");
  insert(DBUS::count, "count(QString widget)",
         i18n("Returns number of items in a widget such as combobox or listbox."), 1);
  insert(DBUS::currentColumn, "currentColumn(QString widget)", 
         i18n("Returns index of current column."), 1);
  insert(DBUS::currentItem, "currentItem(QString widget)", 
         i18n("Returns index of current item."), 1);
  insert(DBUS::currentRow, "currentRow(QString widget)", 
         i18n("Returns index of current row."), 1);
  insert(DBUS::execute, "execute(QString widget)", i18n("Executes the script associated with the widget."), 1);
  insert(DBUS::findItem, "findItem(QString widget, QString item)",
         i18n("Returns the index of an item with the given text."), 2);
  insert(DBUS::insertColumn, "insertColumn(QString widget, int column, int count)",
         i18n("Inserts new column (or <i>count</i> columns) at <i>column</i> position."), 2);
  insert(DBUS::insertItem, "insertItem(QString widget, QString item, int index)",
         i18n("Inserts item at <i>index</i> position."), 3);
  insertAlias(DBUS::insertItem, "addListItem");
  insert(DBUS::insertItems, "insertItems(QString widget, QStringList items, int index)",
         i18n("Inserts multiple items (EOL-separated) at <i>index</i> position."), 3);
  insertAlias(DBUS::insertItems, "addListItems");
  insert(DBUS::insertRow, "insertRow(QString widget, int row, int count)",
         i18n("Inserts new row (or <i>count</i> rows) at <i>row</i> position."), 2);
  insert(DBUS::item, "item(QString widget, int index)",
         i18n("Returns the text of the item at the given index."), 2);
  insert(DBUS::itemDepth, "itemDepth(QString widget, int index)",
         i18n("Returns the depth of the current item in the tree. Root items have depth 0."), 2);
  insert(DBUS::itemPath, "itemPath(QString widget, int index)",
         i18n("Returns the slash-separated path to the given item in the tree."), 2);
  insert(DBUS::removeColumn, "removeColumn(QString widget, int row, int count)",
         i18n("Removes the column (or <i>count</i> consecutive columns) with the given index."), 3);
  insert(DBUS::removeItem, "removeItem(QString widget, int index)",
         i18n("Removes the item with the given index."), 2);
  insertAlias(DBUS::removeItem, "removeListItem");
  insert(DBUS::removeRow, "removeRow(QString widget, int row, int count)",
         i18n("Removes the row (or <i>count</i> consecutive rows) with the given index."), 3);
  insertAlias(DBUS::removeItem, "removeListItem");
  insert(DBUS::selection, "selection(QString widget)", 
         i18n("Returns selected text or text of current item."), 1);
  insert(DBUS::setAssociatedText, "setAssociatedText(QString widget, QString text)",
         i18n("Sets scripts associated with widget. This is an advanced feature that would not be commonly used."), 2);
  insert(DBUS::setEnabled, "setEnabled(QString widget, bool enabled)", 
     i18n("Enables or disables widget."), 2);
  insertAlias(DBUS::setEnabled, "enableWidget");
  insert(DBUS::setCellText, "setCellText(QString widget, int row, int col, QString text)",
         i18n("Sets text of a cell in a table."), 4);
  insert(DBUS::setChecked, "setChecked(QString widget, bool checked)",
         i18n("Sets/unsets checkbox."), 2);
  insert(DBUS::setColumnCaption, "setColumnCaption(QString widget, int column, QString text)",
         i18n("Sets caption of the column <i>column</i>."), 3);
  insert(DBUS::setCurrentItem, "setCurrentItem(QString widget, int index)",
         i18n("Selects the item at the specified index. Indexes are zero based."), 2);
  insertAlias(DBUS::setCurrentItem, "setCurrentTab");
  insert(DBUS::setMaximum, "setMaximum(QString widget, int value)",
         i18n("Sets maximum numeric value"), 2);
  insert(DBUS::setPixmap, "setPixmap(QString widget, QString iconName, int index)",
         i18n("Sets pixmap at the given index to the specified icon. Use <i>index = -1</i> to set the pixmap for all items."), 3);
  insert(DBUS::setRowCaption, "setRowCaption(QString widget, int row, QString text)",
         i18n("Sets caption of the row <i>row</i>."), 3);
  insert(DBUS::setSelection, "setSelection(QString widget, QString text)", 
         i18n("Selects given text or select item containing given text."), 2);
  insertAlias(DBUS::setSelection, "setCurrentListItem");
  insert(DBUS::setText, "setText(QString widget, QString text)", 
         i18n("Sets widget's content."), 2);
  insertAlias(DBUS::setText, "changeWidgetText");
  insert(DBUS::setVisible, "setVisible(QString widget, bool visible)", 
         i18n("Shows/hides widget."), 2);
  insert(DBUS::text, "text(QString widget)", i18n("Returns content of widget."), 1);
  insert(DBUS::type, "type(QString widget)",
     i18n("Returns type(class) of widget."), 1);
    
  insertGroup(Group::Kommander, "Kommander", "");
  insert(Kommander::widgetText, "widgetText", 
    i18n("Returns current widget's content. This was required inside widget A to return widget A content when requested by widget B. The new method is to use @A.text inside B instead of just @A if you just want the unaltered text."), 0);
  insert(Kommander::selectedWidgetText, "selectedWidgetText", 
    i18n("Returns selected text or text of current item. This is deprecated for <i>@mywidget.selected</i>."), 0);
  insert(Kommander::null, "null", 
     i18n("Does nothing. This is useful if you request a CheckBox or RadioButton to return a value where a state, typically the unchecked state, has no value. The @null prevents an error indicating it is empty."), 0);
  insert(Kommander::pid, "pid", 
     i18n("Returns the pid (process ID) of the current process."), 0);
  //FIXME DCOP to DBUS
  insert(Kommander::dcopid, "dcopid", 
     i18n("Returns DCOP identifier of current process. This is shorthand for <i>kmdr-executor-@pid</i>."), 
     0);
  insert(Kommander::parentPid, "parentPid", 
     i18n("Returns the pid of the parent Kommander window."), 0);
  insert(Kommander::debug, "debug(QString text)",
         i18n("Writes <i>text</i> on stderr."), 1);
  insert(Kommander::echo, "echo(QString text)",
         i18n("Writes <i>text</i> on standard output."), 1);
  insert(Kommander::execBegin, "execBegin(QString shell)", 
     i18n("Executes a script block. Bash is used if no shell is given. It is primarily for use in non-button widgets where script actions are not expected. Full path is not required for the shell which may be useful for portability. <p><i>If this is used inside a button it allows alternate script languages to be used and will return a value to the main script, which may be unexpected.</i>"), 0);
  insert(Kommander::env, "env(QString variable)",
     i18n("Returns value of an environment (shell) variable. Do not use <i>$</i> in the name. For example, <i>@env(PATH)</i>."), 1);
  insert(Kommander::exec, "exec(QString command)",
     i18n("Executes an external shell command."), 1);
  insert(Kommander::expr, "expr(QString expression)",
     i18n("Parses an expression and returns computed value."), 1);
  insert(Kommander::forEachBlock, "forEach(QString variable, QString items)",
     i18n("Executes loop: values from <i>items</i> list (passed as EOL-separated string) are assigned to the variable. <br> <i>@forEach(i,A\\nB\\nC\\n)<br>  @# @i=A<br>@endif</i>"), 2);
  insert(Kommander::forBlock, "for(QString variable, int start, int end, int step)",
     i18n("Executes loop: variable is set to <i>start</i> and is increased by <i>step</i> each time loop is executed. Execution stops when variable becomes larger then <i>end</i>. <br><i>@for(i,1,10,1)<br>  @# @i=1<br>@endif</i>."), 3);
  insert(Kommander::global, "global(QString variable)",
     i18n("Returns the value of a global variable."), 1);
  insert(Kommander::i18n, "i18n(QString variable)",
     i18n("Translates the string into the current language. Texts in GUI would be automatically extracted for translation."), 1);
  insert(Kommander::ifBlock, "if(QString expression)",
     i18n("Executes block if expression is true (non-zero number or non-empty string.) <p>Close with <b>@endif</b></p>"), 1);
  insert(Kommander::dialog, "dialog(QString file, QString args)",
     i18n("Executes another Kommander dialog. Current dialog directory is used if no path is given. Arguments may be given as named arguments which will become global variables in the new dialog. For instance: <i>var=val</i>"), 1);
  insert(Kommander::readSetting, "readSetting(QString key, QString default)",
     i18n("Reads setting from configration file for this dialog."), 2);
  insert(Kommander::setGlobal, "setGlobal(QString variable, QString value)",
     i18n("Sets the value of a global variable. Global variables exist for the life of the Kommander window."), 2);
  insert(Kommander::writeSetting, "writeSetting(QString key, QString value)",
     i18n("Stores setting in configuration file for this dialog."), 2);
  insert(Kommander::switchBlock, "switch(QString expresion)",
     i18n("Begin of <b>switch</b> block. Following <b>case</b> values are compared to <i>expression</i>.<p>@switch()<br>@case()<br>@end"), 1);
  //FIXME DCOP to DBUS 
  insert(Kommander::dcop, "dcop(QString id, QString interface, QString function, QString args)",
     i18n("Executes an external DCOP call."), 3, 9);
  insert(Kommander::comment, "#",
     i18n("Adds a comment to EOL that Kommander will not parse"), 0);
     
  insertGroup(Group::Array, "Array", "array");
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
  
  
  insertGroup(Group::String, "String", "str");
  insert(String::length, "length(QString string)", 
    i18n("Returns number of chars in the string."), 1);
  insert(String::contains, "contains(QString string, QString substring)", 
    i18n("Checks if the the string contains the given substring."), 2);
  insert(String::find, "find(QString string, QString sought, int index)", 
    i18n("Returns the position of a substring in the string, or -1 if it is not found."), 2);
  insert(String::findRev, "findRev(QString string, QString sought, int index)", 
         i18n("Returns the position of a substring in the string, or -1 if it is not found. String is searched backwards"), 2);
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
    i18n("Returns the given string with %1, %2, %3 replaced with <i>arg1</i>, <i>arg2</i>, <i>arg3</i> accordingly.", QString("%1"), QString("%2"), QString("%3")), 2); // Placeholders must be substituted, so just put them back for the correct meaning of the sentence.
  
  insertGroup(Group::File, "File", "file");
  insert(File::read, "read(QString)", 
    i18n("Returns content of given file."), 1);
  insert(File::write, "write(QString file, QString string)", 
    i18n("Writes given string to a file."), 2);
  insert(File::append, "append(QString file, QString string)", 
    i18n("Appends given string to the end of a file."), 2);
  
  insertGroup(Group::Input, "Input", "input");
  insert(Input::color, "color", i18n("Shows color dialog. Returns color in #RRGGBB format."));
  insert(Input::text, "text(QString caption, QString label, QString default)", 
         i18n("Shows text selection dialog. Returns entered text."), 2);
  insert(Input::password, "password(QString caption, QString password)", 
         i18n("Shows a dialog asking user for password and returns it."), 1);
  insert(Input::value, "value(QString caption, QString label, int value, int min, int max, int step)", 
         i18n("Shows value selection dialog. Returns entered value."), 5);
  insert(Input::valueDouble, "double(QString caption, QString label, double value, double min, double max, double step)", 
         i18n("Shows float value selection dialog. Returns entered value."), 5);
  insert(Input::openfile, "openfile(QString startdir, QString filter, QString caption)", 
         i18n("Shows existing file selection dialog. Returns selected file."), 0);
  insert(Input::savefile, "savefile(QString startdir, QString filter, QString caption)", 
         i18n("Shows save file selection dialog. Returns selected file."), 0);
  insert(Input::directory, "directory(QString startdir, QString filter, QString caption)", 
         i18n("Shows directory selection dialog. Returns selected directory."), 0);
  insert(Input::openfiles, "openfiles(QString startdir, QString filter, QString caption)", 
         i18n("Shows multiple files selection dialog. Returns EOL-separated list of selected files."), 0);
  
  insertGroup(Group::Message, "Message", "message");
  insert(Message::info, "info(QString text, QString caption)", 
         i18n("Shows an information dialog."), 1);
  insert(Message::error, "error(QString text, QString caption)", 
         i18n("Shows an error dialog."), 1);
  insert(Message::warning, "warning(QString text, QString caption, QString button1, QString button2, QString button3)",
         i18n("Shows a warning dialog with up to three buttons. Returns number of selected button."), 1);
  insert(Message::question, "question(QString text, QString caption, QString button1, QString button2, QString button3)",
         i18n("Shows a question dialog with up to three buttons. Returns number of selected button."), 1);
  
}
