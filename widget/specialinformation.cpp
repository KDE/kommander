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
  int lbracket = name.indexOf('(');
  int rbracket = name.indexOf(')');
  m_function = (lbracket != -1) ? name.left(lbracket) : name;
  m_description = description;
  if (lbracket != -1 && rbracket != -1)
  {
    QString part = name.mid(lbracket+1, rbracket - lbracket - 1);
    QStringList args =  part.split(",");
    for (int i=0; i<args.count(); i++)
    {
      m_types.append(args[i].trimmed().section(' ', 0, 0));
      m_args.append(args[i].trimmed().section(' ', 1, 1));
    }
  }
  m_minArgs = (minArgs == -1) ? m_types.count() : minArgs;
  m_maxArgs = (maxArgs == -1) ? m_types.count() : maxArgs;
}

QString SpecialFunction::prototype(int prototypeFlags) const
{
  if (!m_types.count())
    return m_function;
  int start = (prototypeFlags & SkipFirstArgument) ? 1 : 0;
  QStringList params;
  for (int i=start; i<m_types.count(); i++)
    if (prototypeFlags & ShowArgumentNames)
      params.append(QString("%1 %2").arg(m_types[i]).arg(m_args[i]));
    else
      params.append(m_types[i]);
  if (!params.count())
    return m_function;
  else if (prototypeFlags & NoSpaces)
    return QString("%1(%2)").arg(m_function).arg(params.join(","));
  else
    return QString("%1(%2)").arg(m_function).arg(params.join(", "));
}

QString SpecialFunction::argumentName(int i) const
{
  if (i < m_args.count())
    return m_args[i];
  return QString();
}

QString SpecialFunction::argumentType(int i) const
{
  if (i < m_types.count())
    return m_types[i];
  return QString();
}

int SpecialFunction::argumentCount() const
{
  return m_types.count();
}




int SpecialInformation::function(int group, const QString& fname) 
{
  QString f = fname.toLower();
  if (m_functions.contains(group) && m_functions[group].contains(f))
    return m_functions[group][f];
  else if (m_aliases.contains(group) && m_aliases[group].contains(f))
    return m_aliases[group][f];
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
  return QString();
}

QString SpecialInformation::prototype(int gname, int fname, int flags)
{
 if (isValid(gname, fname))
    return m_specials[gname][fname].prototype(flags);
  return QString();
}

bool SpecialInformation::insert(int id, const QString& function, const QString description,
    int minArgs, int maxArgs)
{
  if (isValid(m_defaultGroup, id))  /* function already defined */
    return false;
  if (m_functions[m_defaultGroup].contains(function.toLower()))
    return false;                   /* function name already in use */
  if (m_aliases[m_defaultGroup].contains(function.toLower()))
    return false;                   /* function name already in use */
  SpecialFunction sf(function, description, minArgs, maxArgs);
  m_specials[m_defaultGroup][id] = sf;
  m_functions[m_defaultGroup][sf.name().toLower()] = id;
  return true;
}
  
bool SpecialInformation::insertAlias(int id, const QString& alias)
{
  if (!isValid(m_defaultGroup, id))  /* function doesn't exists */
    return false;
  if (m_functions[m_defaultGroup].contains(alias.toLower()))
    return false;
  if (m_aliases[m_defaultGroup].contains(alias.toLower()))
    return false;
  m_aliases[m_defaultGroup][alias] = id;
  return true;
}

void SpecialInformation::setDefaultGroup(int gname)  
{
  m_defaultGroup = gname;
}

void SpecialInformation::insertGroup(int id, const QString& name, const QString& parserName)
{
  if (group(name) == -1) {
    m_groups[name] = id;
    m_parserGroups[name] = parserName;
    m_defaultGroup = id;
  }
}

QString SpecialInformation::parserGroupName(const QString& name)
{
  if (m_parserGroups.contains(name))
    return m_parserGroups[name];
  else
    return name;
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
  {
    QStringList list;
    const QMap<int, SpecialFunction> fgroup = m_specials[gid];
    for(QMap<int, SpecialFunction>::ConstIterator it = fgroup.begin(); it != fgroup.end(); ++it)
       list.append(it.value().name());
    return list;
  }
}

QMap<int, QMap<int, SpecialFunction> > SpecialInformation::m_specials;
QMap<QString, int> SpecialInformation::m_groups;
QMap<QString, QString> SpecialInformation::m_parserGroups;
QMap<int, QMap<QString, int> > SpecialInformation::m_functions;
QMap<int, QMap<QString, int> > SpecialInformation::m_aliases;
int SpecialInformation::m_defaultGroup;

