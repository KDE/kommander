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
  QVariant value(bool* valid = 0);
  void setString(const QString& s);
private:
  enum Type {Int, Double, String};
  
  /* parsing function - top-down approach */
  QVariant parseNumber();
  QVariant parseMinus();
  QVariant parseBracket();
  QVariant parseMultiply();
  QVariant parseAdd();
  QVariant parseNot();
  QVariant parseComparison();
  QVariant parseAnd();
  QVariant parseOr();
  /* starting point of parsing */
  QVariant parse();
  
  bool validate();
  QString next() const;
  void setError(int pos = -1);
  int compare(const QVariant v1, const QVariant v2) const; 
  Type commonType(const QVariant v1, const QVariant v2) const; 
  
  QValueList<QVariant> m_parts;
  int m_start;
  bool m_error;
  int m_errorPosition;

};

#endif

