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
    enum {DCOP, Kommander, String, Array, File};
  }
  
  namespace DCOP
  {
    enum {setEnabled, setVisible, setText, text, selection, setSelection,
      currentItem, item, insertItem, insertItems, addUniqueItem, removeItem,
      findItem, clear, setCurrentItem, setChecked, setAssociatedText, 
      associatedText, type, children, global, setGlobal};
  }

  namespace Kommander
  {   
    enum {widgetText, selectedWidgetText, null, pid, dcopid, parentPid, execBegin, 
    env, exec, global, dialog, readSetting, setGlobal, writeSetting, dcop};
  }
 
  namespace Array
  {
    enum {values, keys, clear, count, value, remove, setValue, fromString, toString};
  }
   
  namespace String
  {
    enum {length, contains, find, left, right, mid, remove, replace, upper, lower,
    compare, isEmpty, isNumber};
  }
  
  namespace File
  {
    enum {read, write, append};
  }

  
}
   

using namespace Special;

#endif
