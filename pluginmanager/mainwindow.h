/***************************************************************************
  mainwindow.h -  Kommander plugin manager mainwindow class definition 
                             -------------------
    begin                : Tue Aug 13 09:31:50 EST 2002
    copyright            : (C) 2004 by Marc Britton
    email                : consume@optushome.com.au
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HAVE_MAINWINDOW_H
#define HAVE_MAINWINDOW_H

#include <kmainwindow.h>

class KConfig;
class KListBox;

class MainWindow : public KMainWindow
{
  Q_OBJECT
public:
  MainWindow( QWidget* parent = 0, const char *name = 0, WFlags f = WType_TopLevel | WDestructiveClose );
  ~MainWindow();

protected slots:
  // Call function asigned to tollbutton (0 = add, 1 = remove)
  void toolButton( int id );
  // Remove currently selected plugin 
  void remove();
  // Show dialogbox for selecting plugin, then add it
  void add();
  // Add given plugin if is not added yet
  void add( const QString &plugin );

private:
  // For writing plugin list on exit
  KConfig *m_cfg;
  // List of available plugins 
  KListBox *m_list;
};

#endif
