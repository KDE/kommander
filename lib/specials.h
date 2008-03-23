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

#include "specialinformation.h"

namespace Special
{
  namespace Group
  {
    enum {DBUS, Kommander, String, Array, File, Input, Message};
  }
  
  namespace DBUS
  {
    enum {addUniqueItem, associatedText, cancel, cellText, clear, checked, children, count, currentColumn,
      currentItem, currentRow, execute, findItem, global, insertColumn, insertItem, insertItems, insertRow, 
      item, itemDepth, itemPath, removeColumn, removeItem, removeRow, selection, setAssociatedText, setChecked, 
      setCellText, setCurrentItem, setColumnCaption, setEnabled, setGlobal, setMaximum, setPixmap, 
      setRowCaption, setSelection, setText, 
      setVisible, text, type, setCellWidget, cellWidget, setEditable};
  }
//FIXME DCOP to DBUS conversion
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
      compare, isEmpty, isNumber, section, args, toInt, toDouble};
  }
  
  namespace File
  {
    enum {read, write, append};
  }
  
  namespace Input
  {
    enum {color, text, password, value, valueDouble, openfile, openfiles, savefile, directory};
  }
  
  namespace Message
  {
    enum {info, error, question, warning};
  }
  
}
   

using namespace Special;

#endif