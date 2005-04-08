/***************************************************************************
                    messagelog.cpp - Kommander dialog output
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

#include "messagelog.h"

#include <klistbox.h>
#include <kprocess.h>

MessageLog::MessageLog(QWidget* parent, const char* name) : QTabWidget(parent, name)
{
  for (int i = 0; i < m_listCount; i++)
  {
    m_lists[i] = new KListBox(this);
    addTab(m_lists[i], m_listNames[i]);
    m_seenEOL[i] = false;
  }
}
  
MessageLog::~MessageLog()
{
}
  
void MessageLog::insertItem(InfoType i, QString text)
{
  bool seenEOL = text.endsWith("\n");
  if (seenEOL)
    text.truncate(text.length() - 1);
  QStringList items(QStringList::split('\n', text));
  for (QStringList::ConstIterator it = items.begin(); it != items.end(); ++it ) 
  {
    if (!m_seenEOL[i] && m_lists[i]->count() && it == items.begin())
      m_lists[i]->changeItem(m_lists[i]->text(m_lists[i]->count() - 1) + *it, m_lists[i]->count() - 1);
    else
      m_lists[i]->insertItem(*it);
  }
  m_seenEOL[i] = seenEOL;
  m_lists[i]->setCurrentItem(m_lists[i]->count()-1);  
  m_lists[i]->ensureCurrentVisible();
}

void MessageLog::clear(InfoType i)
{
  if (i < m_listCount)
  {
    m_lists[(int)i]->clear(); 
    m_seenEOL[i] = false;
  }
  else 
    for (int i = 0; i < m_listCount; i++)
      clear((InfoType)i);
}

void MessageLog::receivedStdout(KProcess*, char* buffer, int buflen)
{
  insertItem(Stdout, QString::fromLocal8Bit(buffer, buflen));
}

void MessageLog::receivedStderr(KProcess*, char* buffer, int buflen)
{
  insertItem(Stderr, QString::fromLocal8Bit(buffer, buflen));
}

QString MessageLog::m_listNames[m_listCount] = {i18n("Stdout"), i18n("Stderr")};

#include "messagelog.moc"

