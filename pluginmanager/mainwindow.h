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
    void toolButton( int id );
    void remove();
    void add();
    void add( const QString &plugin );

private:
    KConfig *m_cfg;
    KListBox *m_list;
};

#endif
