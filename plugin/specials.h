/***************************************************************************
                        specials.h - known specials and DCOP calls
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

#ifndef _HAVE_SPECIALS_H_
#define _HAVE_SPECIALS_H_

/** This file contains the global Kommander defined. In case a new Group, method, etc. is
added it MUST be added to the end of the enum lists in order to keep compatibility with the
existing plugins. */


namespace Group
{
  enum {DCOP, Kommander, String, Array, File, Input, Message, Slots};
}

namespace DCOP
{
  enum {addUniqueItem, associatedText, cancel, cellText, clear, checked, children, columnCount, count, currentColumn,
    currentItem, currentRow, execute, findItem, global, insertColumn, insertItem, insertItems, insertRow, 
    item, itemDepth, itemPath, removeColumn, removeItem, removeRow, selection, setAssociatedText, setChecked, 
    setCellText, setCurrentItem, insertTab, setColumnCaption, setEnabled, setGlobal, setMaximum, setPixmap, 
    setRowCaption, setSelection, setText, 
    setVisible, text, type, setCellWidget, cellWidget, setEditable, geometry, hasFocus};
}

namespace Kommander
{
  enum {widgetText, selectedWidgetText, null, pid, dcopid, parentPid, debug,
  echo, env, exec, expr, global, i18n, dialog, readSetting, setGlobal, writeSetting, dcop,
  switchBlock, execBegin, forBlock, forEachBlock, ifBlock, comment, createWidget, connect, disconnect, widgetExists, exit, Break, Continue, Return, execBackground};
}

namespace Array
{
  enum {values, keys, clear, count, value, remove, setValue, fromString, toString, indexedFromString, indexedToString, indexedRemoveElements, indexedInsertElements};
}

namespace String
{
  enum {length, contains, find, findRev, left, right, mid, remove, replace, upper, lower,
  compare, isEmpty, isNumber, section, args, toInt, toDouble, round};
}

namespace File
{
  enum {read, write, append, exists};
}

namespace Input
{
  enum {color, text, password, value, valueDouble, openfile, openfiles, savefile, directory};
}

namespace Message
{
  enum {info, error, question, warning};
}

#endif
