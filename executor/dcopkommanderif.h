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

/** Set the current text/value of the widget. Works for:
      QLabel
      QButton
      QGroupBox: changes the group title
      QLineEdit: changes the lineedit text
      QTextEdit: changes the content of the edit widget
      QSpinBox: changes the value of the spinbox.
      FileSelector: changes value of current file  */
  virtual void changeWidgetText(const QString& widgetName, const QString& text) = 0;

/** Returns current item or -1 if there is no item selected. Works for:
      QListBox
      QComboBox  */  
virtual int currentItem(const QString &widgetName) = 0;  
  
/** Returns text of given item. Works for:
      QListBox
      QComboBox  */  
virtual QString item(const QString &widgetName, int index) = 0;  


/** Removes the item at position index. Works for:
      QListBox  
      QComboBox  */            
virtual void removeListItem(const QString &widgetName, int index) = 0;

/** Inserts a text item with text item, at position index. The item will be appended
    if index is negative. Works for:
      QListBox
      QComboBox  */        
virtual void addListItem(const QString &widgetName, const QString &item, int index) = 0;

/** Inserts the list of strings at position index. The items will be appended if index
    is negative. Works for:
      QListBox
      QComboBox  */        
virtual void addListItems(const QString &widgetName, const QStringList &items, int index) = 0;

/** Remove all items from the list. Works for:  
      QListBox
      QComboBox */        
virtual void clearList(const QString &widgetName) = 0;

/** Sets the cuurent item to the item with name if exists. Otherwise doesn't do
anything. Works for:  
      QListBox
      QComboBox
*/        
virtual void setCurrentListItem(const QString& widgetName, const QString &name) = 0;

/** Sets the current tab to the index position. Works for:
      QTabWidget  */        
virtual void setCurrentTab(const QString &widgetName, int index) = 0;

/** Checks/unchecks the widget. Works for:
      QCheckBox
      QRadioButton  */         
virtual void setChecked(const QString &widgetName, bool checked) = 0;

/** Sets the associated text of the widget. Works for all Kommander widgets. */  
virtual void setAssociatedText(const QString &widgetName, const QString &text) = 0;

/** Returns the associated text of the widget. Works for all Kommander widgets. */ 
virtual QStringList associatedText(const QString &widgetName) = 0;

/** Returns the value of global variable */ 
virtual QString global(const QString& variableName) = 0;

/** Sets the value of global variable */ 
virtual void setGlobal(const QString& variableName, const QString& value) = 0;

/** Returns the value of array element */ 
virtual QString arrayValue(const QString& arrayName, const QString& key) const = 0;

/** Adds/modifies the value of array element */ 
virtual void setArrayValue(const QString& arrayName, const QString& key, const QString& value) = 0;

/** Returns spaces-separated list of array keys */ 
virtual QString array(const QString& arrayName) const = 0;

};

#endif
