/***************************************************************************
                        specialinformation.cpp - internal commands information
                             -------------------
    copyright            : (C) 2004 by Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#include "specialinformation.h" 

SpecialFunction::SpecialFunction(const QString& name, const QString& description,
    int minArgs, int maxArgs)
{
  int lbracket = name.find('(');
  int rbracket = name.find(')');
  m_function = (lbracket != -1) ? name.left(lbracket) : name;
  m_description = description;
  if (lbracket != -1 && rbracket != -1)
  {
    QString part = name.mid(lbracket+1, rbracket - lbracket - 1);
    QStringList args = QStringList::split(",", part);
    for (uint i=0; i<args.count(); i++)
    {
      m_types.append(args[i].stripWhiteSpace().section(' ', 0, 0));
      m_args.append(args[i].stripWhiteSpace().section(' ', 1, 1));
    }
  }
  m_minArgs = (minArgs == -1) ? m_types.count() : minArgs;
  m_maxArgs = (maxArgs == -1) ? m_types.count() : maxArgs;
}

QString SpecialFunction::prototype(uint prototypeFlags) const
{
  if (!m_types.count())
    return m_function;
  int start = (prototypeFlags & SkipFirstArgument) ? 1 : 0;
  QStringList params;
  for (uint i=start; i<m_types.count(); i++)
    if (prototypeFlags & ShowArgumentNames)
      params.append(QString("%1 %2").arg(m_types[i]).arg(m_args[i]));
    else
      params.append(m_types[i]);
  if (params.count())
     return QString("%1(%2)").arg(m_function).arg(params.join(", "));
  else
     return m_function;
}

QString SpecialFunction::argumentName(uint i) const
{
  if (i < m_args.count())
    return m_args[i];
  return QString::null;
}

QString SpecialFunction::argumentType(uint i) const
{
  if (i < m_types.count())
    return m_types[i];
  return QString::null;
}

int SpecialFunction::argumentCount() const
{
  return m_types.count();
}




int SpecialInformation::function(int group, const QString& fname) 
{
  if (m_functions.contains(group) && m_functions[group].contains(fname))
    return m_functions[group][fname];
  else if (m_aliases.contains(group) && m_aliases[group].contains(fname))
    return m_aliases[group][fname];
  return -1;
}
  
SpecialFunction SpecialInformation::functionObject(const QString& gname, const QString& fname)
{
  int gid = group(gname);
  return m_specials[gid][function(gid, fname)];
}

int SpecialInformation::group(const QString& gname) 
{
 if (m_groups.contains(gname))
    return m_groups[gname];
  return -1;  
}

bool SpecialInformation::isValid(int gname, int fname) 
{
  return m_specials.contains(gname) && m_specials[gname].contains(fname);
}

bool SpecialInformation::isValid(const QString& gname, const QString& fname) 
{
  return function(group(gname), fname) != -1;
}

int SpecialInformation::minArg(int gname, int fname) 
{
  if (isValid(gname, fname))
    return m_specials[gname][fname].minArg();
  return -1;
}

int SpecialInformation::maxArg(int gname, int fname) 
{
  if (isValid(gname, fname))
    return m_specials[gname][fname].maxArg();
  return -1;
}

int SpecialInformation::argCount(int gname, int fname) 
{
  if (isValid(gname, fname))
    return m_specials[gname][fname].argumentCount();
  return -1;
}

bool SpecialInformation::isValidArg(int gname, int fname, int args) 
{
  if (isValid(gname, fname))
    return m_specials[gname][fname].isValidArg(args);
  return -1;
}

QString SpecialInformation::description(int gname, int fname)
{
 if (isValid(gname, fname))
    return m_specials[gname][fname].description();
  return QString::null;
}

QString SpecialInformation::prototype(int gname, int fname, uint flags)
{
 if (isValid(gname, fname))
    return m_specials[gname][fname].prototype(flags);
  return QString::null;
}

bool SpecialInformation::insert(int id, const QString& function, const QString description,
    int minArgs, int maxArgs)
{
  if (isValid(m_defaultGroup, id))  /* function already defined */
    return false;
  if (m_functions[m_defaultGroup].contains(function))
    return false;                   /* function name already in use */
  if (m_aliases[m_defaultGroup].contains(function))
    return false;                   /* function name already in use */
  SpecialFunction sf(function, description, minArgs, maxArgs);
  m_specials[m_defaultGroup][id] = sf;
  m_functions[m_defaultGroup][sf.name()] = id;
  return true;
}
  
bool SpecialInformation::insertAlias(int id, const QString& alias)
{
  if (!isValid(m_defaultGroup, id))  /* function doesn't exists */
    return false;
  if (m_functions[m_defaultGroup].contains(alias))
    return false;
  if (m_aliases[m_defaultGroup].contains(alias))
    return false;
  m_aliases[m_defaultGroup][alias] = id;
  return true;
}

void SpecialInformation::setDefaultGroup(int gname)  
{
  m_defaultGroup = gname;
}

void SpecialInformation::insertGroup(int id, const QString& name)
{
  if (group(name) == -1) {
    m_groups[name] = id;
    m_defaultGroup = id;
  }
}

QStringList SpecialInformation::groups()
{
  return m_groups.keys();
}

QStringList SpecialInformation::functions(const QString& g)
{
  int gid = group(g);
  if (gid == -1)
    return QStringList();
  else
    return m_functions[gid].keys();
}

QMap<int, QMap<int, SpecialFunction> > SpecialInformation::m_specials;
QMap<QString, int> SpecialInformation::m_groups;
QMap<int, QMap<QString, int> > SpecialInformation::m_functions;
QMap<int, QMap<QString, int> > SpecialInformation::m_aliases;
int SpecialInformation::m_defaultGroup;

