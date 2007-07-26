/***************************************************************************
                    function.cpp - Functions for internal parser
                             -------------------
    copyright          : (C) 2004      Michal Rudolf <mrudolf@kdewebdwev.org>
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software{} you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation{} either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "function.h"
 
using namespace Parse;

Function::Function() : m_function(0), m_minArgs(0), m_maxArgs(0)
{
}

Function::Function(FunctionPointer fp, ValueType value, const TypeList& params, int min, int max)
  : m_params(params)
{
  m_function = fp;
  m_returnValue = value;
  m_minArgs = min <= m_params.count() ? min : m_params.count();
  m_maxArgs = max >= m_minArgs ? max : m_params.count();
}
    
Function::Function(FunctionPointer fp, ValueType value, ValueType param1, int min, int max)
{
  m_function = fp;
  m_returnValue = value;
  m_params.append(param1);
  m_minArgs = min <= 1 ? min : 1;
  m_maxArgs = max >= m_minArgs ? max : 1;
}
    
Function::Function(FunctionPointer fp, ValueType value, ValueType param1, ValueType param2, int min , 
         int max)
{
  m_function = fp;
  m_returnValue = value;
  m_params.append(param1);
  m_params.append(param2);
  m_minArgs = min <= 2 ? min : 2;
  m_maxArgs = max >= m_minArgs ? max : 2;
}

Function::Function(FunctionPointer fp, ValueType value, ValueType param1, ValueType param2, ValueType param3, 
         int min, int max)
{
  m_function = fp;
  m_returnValue = value;
  m_params.append(param1);
  m_params.append(param2);
  m_params.append(param3);
  m_minArgs = min <= 3 ? min : 3;
  m_maxArgs = max >= m_minArgs ? max : 3;
}
    
Function::Function(FunctionPointer fp, ValueType value, ValueType param1, ValueType param2, ValueType param3, 
                   ValueType param4, int min, int max)
{
  m_function = fp;
  m_returnValue = value;
  m_params.append(param1);
  m_params.append(param2);
  m_params.append(param3);
  m_params.append(param4);
  m_minArgs = min <= 4 ? min : 4;
  m_maxArgs = max >= m_minArgs ? max : 4;
}

Function::Function(FunctionPointer fp, ValueType value, ValueType param1, ValueType param2, ValueType param3, 
                   ValueType param4, ValueType param5, int min, int max)
{
  m_function = fp;
  m_returnValue = value;
  m_params.append(param1);
  m_params.append(param2);
  m_params.append(param3);
  m_params.append(param4);
  m_params.append(param5);
  m_minArgs = min <= 5 ? min : 5;
  m_maxArgs = max >= m_minArgs ? max : 5;
}

bool Function::isVoid() const
{
  return returnValue() == ValueNone;
}
    
ValueType Function::returnValue() const
{
  return m_returnValue;
}
    
ValueType Function::argType(int i) const
{
  if (i < m_params.count())
    return m_params[i];
  else if (i < m_maxArgs)
    return m_params.last();
  else
    return ValueNone;
}
    
int Function::minArgs() const
{
  return m_minArgs;
}
    
int Function::maxArgs() const
{
  return m_maxArgs;
}
    
bool Function::isValid(const ParameterList& params) const
{
  return params.count() >= minArgs() && params.count() <= maxArgs();
}

ParseNode Function::execute(Parser* P, const ParameterList& params) const
{
  if (m_function)
    return m_function(P, params);
  else   
    return ParseNode();
}

