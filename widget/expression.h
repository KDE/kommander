/***************************************************************************
                    expression.h - Expression parser
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

#ifndef _HAVE_EXPRESSION_H_
#define _HAVE_EXPRESSION_H_

#include <qstring.h>
#include <qstringlist.h>

class Expression
{
public:
  Expression();
  Expression(const QString& expr);
  int value(bool* valid = 0);
  void setString(const QString& s);
private:      
  int parseNumber();
  int parseBracket();
  int parseMultiply();
  int parseAdd();
  int parseNot();
  int parseComparison();
  int parseAnd();
  int parseOr();
  int parse();
  bool validate();
  QString next();
  void setError(int pos = -1);
  
  QStringList m_parts;
  int m_start;
  bool m_error;
  int m_errorPosition;
};

#endif

