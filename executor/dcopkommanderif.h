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
  virtual void enableWidget(const QString& widgetName, bool enable) = 0;  
  virtual void changeWidgetText(const QString& widgetName, const QString& text) = 0;      
  virtual void removeListItem(const QString &widgetName, int index) = 0;
  virtual void addListItem(const QString &widgetName, const QString &item, int index) = 0;
  virtual void addListItems(const QString &widgetName, const QStringList &items, int index) = 0;
  virtual void clearList(const QString &widgetName) = 0;
  virtual void setCurrentListItem(const QString& widgetName, int index) = 0;
  virtual void setCurrentTab(const QString &widgetName, int index) = 0;
  virtual void setChecked(const QString &widgetName, bool checked) = 0;
  virtual void setAssociatedText(const QString &widgetName, const QString &text) = 0;
  virtual QStringList associatedText(const QString &widgetName) = 0;
  
};

#endif
