/***************************************************************************
                       dcopkommanderif.h  -  description
                             -------------------
    begin                : Tue Jan 27 2004
    copyright            : (C) 2004 by Andras Mantia <amantia@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

#ifndef DCOPKOMMANDERIF_H
#define DCOPKOMMANDERIF_H

#include <dcopobject.h>
#include <qstringlist.h>

class DCOPKommanderIf : virtual public DCOPObject
{
  K_DCOP
  
  k_dcop:
/** Enable or disable the widget. Works for all widgets based on QWidget.*/
virtual void enableWidget(const QString& widgetName, bool enable) = 0;  

  /** Shows or hides the widget. Works for all widgets based on QWidget.*/
virtual void setVisible(const QString& widgetName, bool enable) = 0;  

/** Set the current text/value of the widget. Works for:
      QLabel
      Button: sets button title
      GroupBox: changes the group title
      LineEdit: changes the lineedit text
      TextEdit: changes the content of the edit widget
      SpinBoxInt: changes the value of the spinbox.
      FileSelector: changes value of current file  
      ListBox:  sets content of list (items are EOL-separated)
      ComboBox: sets content of combo (items are EOL-separated) */
virtual void changeWidgetText(const QString& widgetName, const QString& text) = 0;

/** Returns the current text/value of the widget. Works for all Kommander widgets - implemented in widget */
virtual QString text(const QString& widgetName) = 0;

/** Selects given text in the widget. Works for all Kommander widgets - implemented in widget */
virtual void setSelection(const QString& widgetName, const QString& text) = 0;

/** Returns the selected text/value of the widget. Works for all Kommander widgets - implemented in widget */
virtual QString selection(const QString& widgetName) = 0;


/** Returns current item or -1 if there is no item selected. Works for:
      ListBox
      ComboBox  */  
virtual int currentItem(const QString &widgetName) = 0;  
  
/** Returns text of given item. Works for:
      ListBox
      ComboBox  */  
virtual QString item(const QString &widgetName, int index) = 0;  


/** Removes the item at position index. Works for:
      ListBox  
      ComboBox  */            
virtual void removeListItem(const QString &widgetName, int index) = 0;

/** Inserts a text item with text item, at position index. The item will be appended
    if index is negative. Works for:
      ListBox
      ComboBox  */        
virtual void addListItem(const QString &widgetName, const QString &item, int index) = 0;

/** Inserts a pixmap for given item. Works for:
      ListBox
      TreeView  */        
virtual void setPixmap(const QString &widgetName, const QString &iconName, int index) = 0;

/** Inserts the list of strings at position index. The items will be appended if index
    is negative. Works for:
      ListBox
      ComboBox  */        
virtual void addListItems(const QString &widgetName, const QStringList &items, int index) = 0;

/** Inserts a text item with text item if it is not present now. Works for:
      ListBox */        
virtual void addUniqueItem(const QString &widgetName, const QString &item) = 0;

/** Returns index of item with text 'item' or -1. Search is done in following order:
 1. exact, 2. begins with 3. contains. Works for:
      ListBox */        
virtual int findItem(const QString &widgetName, const QString &item) = 0;

/** Remove all items from the list. Works for:  
      ListBox
      ComboBox */        
virtual void clearList(const QString &widgetName) = 0;

/** Sets the cuurent item to the item with name if exists. Otherwise doesn't do
anything. Works for:  
      ListBox
      ComboBox
*/        
virtual void setCurrentListItem(const QString& widgetName, const QString &name) = 0;

/** Sets the current tab to the index position. Works for:
      TabWidget  */        
virtual void setCurrentTab(const QString &widgetName, int index) = 0;

/** Checks/unchecks the widget. Works for:
      CheckBox
      RadioButton  */         
virtual void setChecked(const QString &widgetName, bool checked) = 0;

/** Sets the associated text of the widget. Works for all Kommander widgets. */  
virtual void setAssociatedText(const QString &widgetName, const QString &text) = 0;

/** Returns the associated text of the widget. Works for all Kommander widgets. */ 
virtual QStringList associatedText(const QString &widgetName) = 0;

/** Returns class of widget */
virtual QString type(const QString& widget) = 0;

/** Returns list of widgets that are children of given parent */
virtual QStringList children(const QString& parent, bool recursive) = 0;

/** Returns the value of global variable */ 
virtual QString global(const QString& variableName) = 0;

/** Sets the value of global variable */ 
virtual void setGlobal(const QString& variableName, const QString& value) = 0;
  
};

#endif
