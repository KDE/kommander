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

/* See widgets/specials.cpp for description. Deprecated methods are replaced by methods above. */

class DCOPKommanderIf : virtual public DCOPObject
{
  K_DCOP
  
  k_dcop:
virtual void setEnabled(const QString& widgetName, bool enable) = 0;  
virtual void enableWidget(const QString& widgetName, bool enable) = 0;    /* DEPRECATED */
virtual void setVisible(const QString& widgetName, bool enable) = 0;  
virtual void setText(const QString& widgetName, const QString& text) = 0; 
virtual void changeWidgetText(const QString& widgetName, const QString& text) = 0; /* DEPRECATED */
virtual QString text(const QString& widgetName) = 0;
virtual void setSelection(const QString& widgetName, const QString& text) = 0;
virtual void setCurrentListItem(const QString& widgetName, const QString& text) = 0; /* DEPRECATED */
virtual QString selection(const QString& widgetName) = 0;
virtual int currentItem(const QString &widgetName) = 0;  
virtual QString item(const QString &widgetName, int index) = 0;  
virtual int itemDepth(const QString &widgetName, int index) = 0;  
virtual QString itemPath(const QString &widgetName, int index) = 0;  
virtual void removeItem(const QString &widgetName, int index) = 0;
virtual void removeListItem(const QString &widgetName, int index) = 0; /* DEPRECATED */
virtual void insertItem(const QString &widgetName, const QString &item, int index) = 0;
virtual void addListItem(const QString &widgetName, const QString &item, int index) = 0; /* DEPRECATED */
virtual void setPixmap(const QString &widgetName, const QString &iconName, int index) = 0;
virtual void insertItems(const QString &widgetName, const QStringList &items, int index) = 0;
virtual void addListItems(const QString &widgetName, const QStringList &items, int index) = 0;/*DEPRECATED*/
virtual void addUniqueItem(const QString &widgetName, const QString &item) = 0;
virtual int findItem(const QString &widgetName, const QString &item) = 0;
virtual void clear(const QString &widgetName) = 0;
virtual void clearList(const QString &widgetName) = 0;  /* DEPRECATED */
virtual void setCurrentItem(const QString &widgetName, int index) = 0;
virtual void setCurrentTab(const QString &widgetName, int index) = 0; /* DEPRECATED */
virtual void insertTab(const QString &widgetName, const QString &label, int index) = 0;
virtual bool checked(const QString &widgetName) = 0;
virtual void setChecked(const QString &widgetName, bool checked) = 0;
virtual void setAssociatedText(const QString &widgetName, const QString &text) = 0;
virtual QStringList associatedText(const QString &widgetName) = 0;
virtual QString type(const QString& widget) = 0;
virtual QStringList children(const QString& parent, bool recursive) = 0;
virtual QString global(const QString& variableName) = 0;
virtual void setGlobal(const QString& variableName, const QString& value) = 0;
virtual void setMaximum(const QString &widgetName, int value) = 0;
virtual void execute(const QString &widgetName) = 0;   
virtual void cancel(const QString &widgetName) = 0;   
virtual int count(const QString &widgetName) = 0;
virtual int currentColumn(const QString &widgetName) = 0;
virtual int currentRow(const QString &widgetName) = 0;
virtual void insertRow(const QString &widgetName, int row, int count) = 0;
virtual void insertColumn(const QString &widgetName, int column, int count) = 0;
virtual void setCellText(const QString &widgetName, int row, int column, const QString& text) = 0;
virtual QString cellText(const QString &widgetName, int row, int column) = 0;
virtual void removeRow(const QString &widgetName, int row, int count) = 0;
virtual void removeColumn(const QString &widgetName, int column, int count) = 0;
virtual void setRowCaption(const QString &widgetName, int row, const QString& text) = 0;
virtual void setColumnCaption(const QString &widgetName, int column, const QString& text) = 0;
virtual int getWinID() = 0;
};

#endif
