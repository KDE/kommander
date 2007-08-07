/***************************************************************************
                          pluginmanager.cpp  -  Plugin manager class
                             -------------------
    copyright            : (C) 2005 Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <kconfig.h>
#include <klibloader.h>
 
#include "pluginmanager.h"

PluginManager::PluginManager()
{
  m_cfg = new KConfig("kommanderrc");
  m_list = new QStringList(m_cfg->readListEntry("plugins"));
}

PluginManager::~PluginManager()
{
  m_cfg->writeEntry("plugins", *m_list);
  delete m_cfg;
  delete m_list;
}  
  
int PluginManager::count() const
{
  return m_list->count();  
}
  
QString PluginManager::item(int i) const
{
  return (*m_list)[i];
}
    
bool PluginManager::add(const QString& plugin, bool)
{
  QString plugName= libraryName(plugin);
  if (plugName.isNull())
    return false;
  if (m_list->contains(plugName))
    return false;
  m_list->append(plugName);
  return true;
}

bool PluginManager::remove(const QString& plugin)
{
  if (!m_list->contains(plugin))
    return false;  
  m_list->remove(plugin);
  return true;
}

int PluginManager::verify()
{
  QStringList verified;
  for (QStringList::ConstIterator it = m_list->begin(); it != m_list->end(); ++it)
    if (!libraryName(*it).isNull())
      verified.append(*it);
  int removed = count() - verified.count();
  m_list->clear();
  (*m_list) += verified;
  return removed;
}

QString PluginManager::libraryName(const QString& plugin) const
{
  KLibrary *l = KLibLoader::self()->library(plugin.toLatin1());
  QString libname;
  if (l && l->resolveSymbol("kommander_plugin")!=0)
    libname = l->fileName();
  return libname;
} 

QStringList PluginManager::items() const
{
  return (*m_list);
}

