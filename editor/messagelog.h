/***************************************************************************
                    messagelog.h - Kommander dialog output
                             -------------------
    copyright          : (C) 2004      Michal Rudolf <mrudolf@kdewebdwev.org>
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include <qtabwidget.h>

#include <klocale.h>

class K3ListBox;
class K3Process;
class KMenu;
class Q3ListBoxItem;
   
class MessageLog : public QTabWidget
{
  Q_OBJECT
public:  
  enum InfoType {Stdout, Stderr, All};
  MessageLog(QWidget* parent = 0, const char* name = 0);
  ~MessageLog();
  virtual void insertItem(InfoType i, QString text);
  virtual void clear(InfoType i = All);
  QString content();
public slots:  
  void receivedStdout(K3Process *proc, char *buffer, int buflen);
  void receivedStderr(K3Process *proc, char *buffer, int buflen);
  void clearContent();
  void copyLine();
  void copyContent();
  void saveToFile();
  void showMenu(Q3ListBoxItem*, const QPoint& l_point);
private:
  static const int m_listCount = 2;
  static QString m_listNames[m_listCount];
  K3ListBox* m_lists[m_listCount];
  bool m_seenEOL[m_listCount];
  KMenu* m_popupMenu;
};

#endif
