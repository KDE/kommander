/***************************************************************************
                    function.h - Functions for internal parser
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

#ifndef _HAVE_FUNCTION_H_
#define _HAVE_FUNCTION_H_

#include "parsenode.h"
#include <qvaluevector.h>

typedef QValueVector<ParseNode> ParameterList;
typedef QValueVector<Parse::ValueType> TypeList;
typedef ParseNode(*FunctionPointer)(const ParameterList&);

class Function
{
  public:
    /* default constructor - empty function */
    Function();
    /* construct a function from parameterlist */
    Function(FunctionPointer fp, Parse::ValueType value, const TypeList& params, uint min = 99999, 
             uint max = 0);
    /* construct a function from parameters */
    Function(FunctionPointer fp, Parse::ValueType value, Parse::ValueType param1, uint min = 99999, 
             uint max = 0);
    Function(FunctionPointer fp, Parse::ValueType value, Parse::ValueType param1, Parse::ValueType param2, 
             uint min = 99999, uint max = 0);
    Function(FunctionPointer fp, Parse::ValueType value, Parse::ValueType param1, Parse::ValueType param2, 
             Parse::ValueType param3, uint min = 99999, uint max = 0);
    Function(FunctionPointer fp, Parse::ValueType value, Parse::ValueType param1, Parse::ValueType param2, 
             Parse::ValueType param3, Parse::ValueType param4, uint min = 99999, uint max = 0);
    /* if function returns value */
    bool isVoid() const;
    /* type of returned value */
    Parse::ValueType returnValue() const;
    /* type of i-th argument */
    Parse::ValueType argType(uint i) const;
    /* minimum number of arguments */
    uint minArgs() const;
    /* maximum number of arguments */
    uint maxArgs() const;
    /* check whether given list is appropriate for this function */
    bool isValid(const ParameterList& params) const;
    /* execute */
    ParseNode execute(const ParameterList& params) const;
  
private:
    FunctionPointer m_function;
    TypeList m_params;
    Parse::ValueType m_returnValue;
    uint m_minArgs;
    uint m_maxArgs;
};

#endif

