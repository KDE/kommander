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

SpecialFunction::SpecialFunction(const QString& name, int minArgs, 
  const QString& description) : m_description(description)
{
  int lbracket = name.find('(');
  int rbracket = name.find(')');
  m_function = (lbracket != -1) ? name.left(lbracket) : name;
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
}

QString SpecialFunction::prototype() const
{
  if (!m_types.count())
    return m_function;
  else return QString("%1(%2)").arg(m_function).arg(m_types.join(", "));
}

QString SpecialFunction::longPrototype() const
{
  if (!m_types.count())
    return m_function;
  else {
    QStringList params;
    for (uint i=0; i<m_types.count(); i++)
      params.append(QString("%1 %2").arg(m_types[i]).arg(m_args[i]));
    return QString("%1(%2)").arg(m_function).arg(params.join(", "));
  }
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




int SpecialInformation::function(int group, const QString& fname) 
{
  if (m_functions.contains(group) && m_functions[group].contains(fname))
    return m_functions[group][fname];
  return -1;
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

QString SpecialInformation::prototype(int gname, int fname)
{
 if (isValid(gname, fname))
    return m_specials[gname][fname].prototype();
  return QString::null;
}

QString SpecialInformation::longPrototype(int gname, int fname)
{
 if (isValid(gname, fname))
    return m_specials[gname][fname].longPrototype();
  return QString::null;
}

bool SpecialInformation::insert(int id, const QString& function, int minArgs, 
    const QString description)
{
  if (isValid(m_defaultGroup, id))  /* function already defined */
    return false;
  if (m_functions[m_defaultGroup].contains(function))
    return false;                   /* function name already in use */
  SpecialFunction sf(function, minArgs, description);
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
  m_functions[m_defaultGroup][alias] = id;
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

QMap<int, QMap<int, SpecialFunction> > SpecialInformation::m_specials;
QMap<QString, int> SpecialInformation::m_groups;
QMap<int, QMap<QString, int> > SpecialInformation::m_functions;
int SpecialInformation::m_defaultGroup;

