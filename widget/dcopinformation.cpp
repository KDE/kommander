/***************************************************************************
                          dcopinformation.cpp - internal DCOP functions information
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

#include "dcopinformation.h"

DCOPFunction::DCOPFunction(const QString& prototype, const QString& description)
{
  m_prototype = prototype;
  m_description = description;
  m_args = prototype.contains(',') + 1;
  m_function = prototype;
  int bracket = m_function.find('(');
  if (bracket)
    m_function.truncate(bracket);
}

QString DCOPInformation::prototype(const QString& name)
{
  if (m_prototypes.contains(name))
    return m_prototypes[name].prototype();
  return QString::null;
}

int DCOPInformation::arguments(const QString& name)
{
  if (m_prototypes.contains(name))
    return m_prototypes[name].arguments();
  return -1;
}

QString DCOPInformation::function(const QString& name)
{
  if (m_prototypes.contains(name))
    return m_prototypes[name].function();
  return QString::null;
}

QString DCOPInformation::description(const QString& name)
{
  if (m_prototypes.contains(name))
    return m_prototypes[name].description();
  return QString::null;
}

void DCOPInformation::insert(const DCOPFunction& function)
{
  m_prototypes[function.function()] = function;
}

void DCOPInformation::insert(const QString& function, const QString description)
{
  DCOPInformation::insert(DCOPFunction(function, description));
}
  

QMap<QString, DCOPFunction> DCOPInformation::m_prototypes;
