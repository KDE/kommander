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
#include <qvaluelist.h>
#include <qvariant.h>

class Expression
{
public:
  Expression();
  Expression(const QString& expr);
  /* set string to parse */
  Expression& operator=(const QString& s);
  /* calculate value */
  QVariant value(bool* valid = 0);
  /* equivalent of setString(s) and value(valid) */
  QVariant value(const QString& s, bool* valid = 0);
  /* equivalent of setString(s) and checking if value(valid) is true */
  bool isTrue(const QString& s, bool* valid = 0);
  
  
private:
  enum Type {TypeInt, TypeDouble, TypeString};
  
  /* parsing function - top-down approach */
    
  /* parse terminal (number or string) */
  QVariant parseNumber();
  /* parse -x expression */
  QVariant parseMinus();
  /* parse (x) expression */
  QVariant parseBracket();
  /* parse x*y, x/y and x%y expressions */
  QVariant parseMultiply();
  /* parse x+y and x-y expressions */
  QVariant parseAdd();
  /* parse !x and (equivalent) not x expressions */
  QVariant parseNot();
  /* parse x==y, x<=y, x>=y, x<y and x>y expressions */
  QVariant parseComparison();
  /* parse x && y, (equivalent) x and y expressions */
  QVariant parseAnd();
  /* parse x || y and  (equivalent) x or y expressions */
  QVariant parseOr();
  /* starting point of parsing - just call first function above */
  QVariant parse();
  
  /* check if we still have next argument */
  bool validate();
  /* return next argument to parse or null if there is none */
  QString next() const;
  /* set error position for future error reporting */
  void setError(int pos = -1);
  /* compare various types of QVariant (strings, floats, ints */
  int compare(const QVariant v1, const QVariant v2) const; 
  /* return common type for binary operations */
  Type commonType(const QVariant v1, const QVariant v2) const; 
        
  QValueList<QVariant> m_parts;
  uint m_start;
  bool m_error;
  uint m_errorPosition;

};

#endif

