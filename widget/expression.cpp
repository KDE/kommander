/***************************************************************************
                    expression.cpp - Expression parser
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

#include "expression.h"

Expression::Expression() : m_start(0), m_error(false)
{
}

Expression::Expression(const QString& expr)
{
   setString(expr);
}

void Expression::setString(const QString& s)
{
   m_start = 0;
   m_error = false;
   const QString single = "()<>!+-/*%";
   int start = 0;
   int len = s.length();
   int i = 0;
   while (i < len)
   {
      if ((s[i] == '>' || s[i] == '<' || s[i] == '=') && 
        s[i+1] == '=')
      {
        m_parts.append(s.mid(i, 2));
	i += 2;
      }
      else if (s[i].isDigit())
      {
	 i++;
         while (i<len && s[i].isDigit())
	   i++;
	 m_parts.append(s.mid(start, i-start));
      }
      else if (single.contains(s[i]))
        m_parts.append(s[i++]);
      else if (s[i] == '\"')
      {
        while (i<len && s[i] != '\"')
	  i++;
	m_parts.append(s.mid(start, i-start));
      }
      else if (s[i].isSpace())
        while (i<len && s[i].isSpace())
	  i++;
      else 
      {
        while (i<len && !s[i].isSpace())
	  i++;
	m_parts.append(s.mid(start, i-start));
      }
      start = i;
   }
}

QString Expression::next()
{
  if (m_start < m_parts.count())
    return m_parts[m_start];
  else 
    return QString::null;
}

bool Expression::validate()
{
  if (m_start >= m_parts.count())
    setError();
  return !m_error;
}

void Expression::setError(int pos)
{
  m_errorPosition = pos == -1 ? m_start : pos;
  m_error = true;
}

int Expression::parseNumber()
{
  if (!validate()) return -1;
  bool sign = next() == "-";
  if (sign)
    m_start++;
  bool ok;
  int value = next().toInt(&ok);
  if (!ok)
    setError(m_start-1);
  m_start++;
  return sign ? -value : value;
}


int Expression::parseBracket()
{
  if (!validate()) return -1;
  if (next() == "(")
  {
    m_start++;
    int value = parse();
    if (next() == ")")
      m_start++;
    else
      setError();
    return value;  
  }
  else 
    return parseNumber();
}


int Expression::parseMultiply()
{
  if (!validate()) return -1;
  int value = parseBracket();
  QString op = next();
  while (op == "*" || op == "/" || op == "%")
  {
    m_start++;
    if (op == "*")
      value *= parseBracket();
    else if (op == "/")
      value /= parseBracket();
    else 
      value %= parseBracket();
    op = next();
  }
  return value;
}

int Expression::parseAdd()
{
  if (!validate()) return -1;
  int value = parseMultiply();
  while (next() == "+" || next() == "-")
  {
    bool add = (next() == "+");
    m_start++;
    if (add)
      value += parseMultiply();
    else 
      value -= parseMultiply();
  }
  return value;
}

int Expression::parseComparison()
{
  if (!validate()) return -1;
  int value = parseAdd();
  QString cmp = next();
  if (cmp == "<" || cmp == "<=" || cmp == "==" || cmp == ">=" || cmp == ">")
  {
    m_start++;
    if (cmp == "<")
      return value < parseAdd();
    else if (cmp == "<=")
      return value <= parseAdd();
    else if (cmp == "==")
      return value == parseAdd();
    else if (cmp == ">=")
      return value >= parseAdd();
    if (cmp == ">")
      return value > parseAdd();
  }
  else
    return value;
}

int Expression::parseNot()
{
  if (next() == "!" || next() == "not")
  {
    m_start++;    
    return !parseComparison();
  }
  else 
    return parseComparison();
}

int Expression::parseAnd()
{
  if (!validate()) return -1;
  int value = parseNot();
  while (next() == "&&" || next() == "and")
  {
    m_start++;    
    value == parseNot() && value;
  }
  return value;
}

int Expression::parseOr()
{
  if (!validate()) return -1;
  int value = parseAnd();
  while (next() == "||" || next() == "or")
  {
    m_start++;    
    value = parseAnd() || value;
  }
  return value;
}

int Expression::parse()
{ 
  return parseOr();
}

int Expression::value(bool* valid)
{
  m_start = 0;
  m_error = false;
  int val = parse();
  if (valid)
     *valid = !m_error && m_start == m_parts.count();
  return val;
}

