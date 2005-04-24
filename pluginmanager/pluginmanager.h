/***************************************************************************
                          pluginmanager.h  -  Plugin manager class
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

 
#ifndef __HAVE_PLUGINMANAGER_H
#define __HAVE_PLUGINMANAGER_H

#include <qstringlist.h>

class KConfig;

class PluginManager
{
public:  
  PluginManager();
  ~PluginManager();
  
  // number of known plugins
  int count() const;
  // n-th plugin
  QString item(int i) const;
  // adds a plugin if valid; 
  // FIXME: if overwrite is true, plugin with the same base name will be removed
  bool add(const QString& plugin, bool overwrite = true);
  // removes given plugin
  bool remove(const QString& plugin);
  // remove invalid plugins, returns number of removed plugins
  int verify();
  // list of plugins
  QStringList items() const;
private:
  // returns expanded library name or null if it is invalid
  QString libraryName(const QString& plugin) const;
  
  QStringList* m_list;
  KConfig *m_cfg;
};


#endif

