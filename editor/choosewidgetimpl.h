/***************************************************************************
                          choosewidgetimpl.h  - dialog to choose widget
                             -------------------
    begin                :  Thu 13 Apr 2004
    copyright            : (C) 2000 Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#ifndef _HAVE_CHOOSEWIDGETIMPL_H_
#define _HAVE_CHOOSEWIDGETIMPL_H_

#include "choosewidget.h"

class ChooseWidget : public ChooseWidgetBase
{
  Q_OBJECT
public:
  ChooseWidget(QWidget*, const char* = 0, bool=true);
  ~ChooseWidget();
  // Put current widget and all of its children in a tree
  void setWidget(QWidget *);
  // Return current widget
  QString selection();
 
public slots:
  // Filter text changed - find matching widget
  void textChanged(const QString&);
  // Double clicked on listviwew
  void selectedItem(QListViewItem *);
private:
  // Check if given widget is a Kommander widget
  bool isKommanderWidget(QObject* w);

};

#endif

