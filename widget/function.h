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
#include <qvector.h>

class Parser;

typedef QVector<ParseNode> ParameterList;
typedef QVector<Parse::ValueType> TypeList;
typedef ParseNode(*FunctionPointer)(Parser*, const ParameterList&);

class Function
{
  public:
    /* default constructor - empty function */
    Function();
    /* construct a function from parameterlist */
    Function(FunctionPointer fp, Parse::ValueType value, const TypeList& params, int min = 99999, 
             int max = 0);
    /* construct a function from parameters */
    Function(FunctionPointer fp, Parse::ValueType value, Parse::ValueType param1, int min = 99999, 
             int max = 0);
    Function(FunctionPointer fp, Parse::ValueType value, Parse::ValueType param1, Parse::ValueType param2, 
             int min = 99999, int max = 0);
    Function(FunctionPointer fp, Parse::ValueType value, Parse::ValueType param1, Parse::ValueType param2, 
             Parse::ValueType param3, int min = 99999, int max = 0);
    Function(FunctionPointer fp, Parse::ValueType value, Parse::ValueType param1, Parse::ValueType param2, 
             Parse::ValueType param3, Parse::ValueType param4, int min = 99999, int max = 0);
    Function(FunctionPointer fp, Parse::ValueType value, Parse::ValueType param1, Parse::ValueType param2, 
             Parse::ValueType param3, Parse::ValueType param4, Parse::ValueType param5, 
             int min = 99999, int max = 0);
    /* if function returns value */
    bool isVoid() const;
    /* type of returned value */
    Parse::ValueType returnValue() const;
    /* type of i-th argument */
    Parse::ValueType argType(int i) const;
    /* minimum number of arguments */
    int minArgs() const;
    /* maximum number of arguments */
    int maxArgs() const;
    /* check whether given list is appropriate for this function */
    bool isValid(const ParameterList& params) const;
    /* execute */
    ParseNode execute(Parser* P, const ParameterList& params) const;
  
private:
    FunctionPointer m_function;
    TypeList m_params;
    Parse::ValueType m_returnValue;
    int m_minArgs;
    int m_maxArgs;
};

#endif

