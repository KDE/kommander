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

class KListBox;
class KProcess;
  
   

class MessageLog : public QTabWidget
{
  Q_OBJECT
public:  
  enum InfoType {Stdout, Stderr, All};
  MessageLog(QWidget* parent = 0, const char* name = 0);
  ~MessageLog();
  virtual void insertItem(InfoType i, QString text);
  virtual void clear(InfoType i = All);
public slots:  
  void receivedStdout(KProcess *proc, char *buffer, int buflen);
  void receivedStderr(KProcess *proc, char *buffer, int buflen);
private:
  static const int m_listCount = 2;
  static QString m_listNames[m_listCount];
  KListBox* m_lists[m_listCount];
  bool m_seenEOL[m_listCount];
};

#endif
