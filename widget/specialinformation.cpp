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

SpecialFunction::SpecialFunction(const QString& name, int minArgs, int maxArgs, 
  const QString& description) : m_function(name), m_description(description)
{
  m_args.first = minArgs;
  m_args.second = maxArgs > minArgs ? maxArgs : minArgs;
}



bool SpecialInformation::isValid(const QString& function, const QString& objectName)
{
  if (!m_specials.contains(objectName))
    return false;
  return m_specials[objectName].contains(function);  
}

bool SpecialInformation::isObject(const QString& objectName)
{
  return m_specials.contains(objectName);
}


int SpecialInformation::minArg(const QString& function, const QString& objectName)
{
  if (isValid(function, objectName))
    return m_specials[objectName][function].minArg();
  return -1;  
}

int SpecialInformation::maxArg(const QString& function, const QString& objectName)
{
  if (isValid(function, objectName))
    return m_specials[objectName][function].maxArg();
  return -1;  
}

bool SpecialInformation::validArg(const QString& function, int arg, const QString& objectName)
{
  if (isValid(function, objectName))
    return m_specials[objectName][function].validArg(arg);
  return -1;  
}

QString SpecialInformation::description(const QString& function, const QString& objectName)
{
  if (isValid(function, objectName))
    return m_specials[objectName][function].description();
  return QString::null;  
}

void SpecialInformation::insert(const SpecialFunction& function)
{
  m_specials[m_currentObject][function.function()] = function;
}

void SpecialInformation::insert(const QString& function, int minArgs, int maxArgs,
    const QString description)
{    
  m_specials[m_currentObject][function] = SpecialFunction(function, minArgs,
    maxArgs, description);
}   
  
QMap<QString, QMap<QString, SpecialFunction> > SpecialInformation::m_specials;
QString SpecialInformation::m_currentObject;

