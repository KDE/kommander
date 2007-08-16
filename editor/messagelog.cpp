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

#include <kapplication.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <k3listbox.h>
#include <kmessagebox.h>
#include <kmenu.h>
#include <k3process.h>

#include <qclipboard.h>
#include <qfile.h>
#include <q3textstream.h>

MessageLog::MessageLog(QWidget* parent, const char* name) : QTabWidget(parent, name)
{
  m_popupMenu = new KMenu(this);
  m_popupMenu->insertItem(SmallIconSet("edit-copy"), i18n("Copy Current &Line"), this, SLOT(copyLine()));
  m_popupMenu->insertItem(SmallIconSet("edit-copy"), i18n("&Copy Content"), this, SLOT(copyContent()));
  m_popupMenu->insertItem(SmallIconSet("document-save-as"), i18n("&Save As..."), this, SLOT(saveToFile()));
  m_popupMenu->insertSeparator();
  m_popupMenu->insertItem(SmallIconSet("edit-clear"), i18n("Clear"), this, SLOT(clearContent()));
  
  for (int i = 0; i < m_listCount; i++)
  {
    m_lists[i] = new K3ListBox(this);
    addTab(m_lists[i], m_listNames[i]);
    m_seenEOL[i] = false;
    connect(m_lists[i], SIGNAL(contextMenuRequested(Q3ListBoxItem*, const QPoint&)),
            this, SLOT(showMenu(Q3ListBoxItem*, const QPoint&)));
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

QString MessageLog::content()
{
  QString p_content;
  K3ListBox* list = m_lists[currentPageIndex()];
  for (uint i=0; i < list->count(); i++)
    p_content.append(list->text(i) + "\n");
  return p_content;
}

void MessageLog::clear(InfoType i)
{
  if (i != All)
  {
    m_lists[(int)i]->clear(); 
    m_seenEOL[i] = false;
  }
  else 
    for (int i = 0; i < m_listCount; i++)
      clear((InfoType)i);
}

void MessageLog::receivedStdout(K3Process*, char* buffer, int buflen)
{
  insertItem(Stdout, QString::fromLocal8Bit(buffer, buflen));
}

void MessageLog::receivedStderr(K3Process*, char* buffer, int buflen)
{
  insertItem(Stderr, QString::fromLocal8Bit(buffer, buflen));
}

void MessageLog::clearContent()
{
  clear((InfoType)currentPageIndex()); 
}

void MessageLog::copyLine()
{
  if (m_lists[currentPageIndex()]->count())
    kapp->clipboard()->setText(m_lists[currentPageIndex()]->currentText(), QClipboard::Clipboard);
}

void MessageLog::copyContent()
{
  kapp->clipboard()->setText(content(), QClipboard::Clipboard);
}

void MessageLog::saveToFile()
{
  KUrl url=KFileDialog::getSaveUrl(QDir::currentPath(),
                                   i18n("*.log|Log Files (*.log)\n*|All Files"), this, i18n("Save Log File"));
  if (url.isEmpty())
    return;
  QFileInfo fileinfo(url.path());
  if (fileinfo.exists() && KMessageBox::warningContinueCancel(0,
      i18n("<qt>File<br><b>%1</b><br>already exists. Overwrite it?</qt>",
           url.path()), QString::null, i18n("Overwrite")) == KMessageBox::Cancel)	//krazy:exclude=nullstrassign for old broken gcc
    return;
  QFile file(url.path());
  if (!file.open(QIODevice::WriteOnly)) {
    KMessageBox::error(0, i18n("<qt>Cannot save log file<br><b>%1</b></qt>",
         url.url()));
    return;
  }
  Q3TextStream textfile(&file);
  textfile << content();
  file.close();
}

void MessageLog::showMenu(Q3ListBoxItem*, const QPoint& l_point)
{
  m_popupMenu->exec(l_point);
}

QString MessageLog::m_listNames[m_listCount] = {i18n("Stdout"), i18n("Stderr")};

#include "messagelog.moc"

