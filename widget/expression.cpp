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
  *this = expr;
}

Expression& Expression::operator=(const QString& s)
{
  m_start = 0;
  m_error = false;
  m_parts.clear();
  const QString single = "()<>!+-/*%";
  int start = 0;
  int len = s.length();
  int i = 0;
  while (i < len)
  {
    if (((s[i] == '>' || s[i] == '<' || s[i] == '=' || s[i] == '!') &&
            s[i + 1] == '=') || (s[i] == '<' && s[i + 1] == '>'))
    {
      m_parts.append(QVariant(s.mid(i, 2)));
      i += 2;
    } else if (s[i].isDigit())
    {
      i++;
      bool decimal = false;
      while (i < len && (s[i].isDigit() || (!decimal && s[i] == QChar('.'))))
      {
        if (s[i] == '.')
          decimal = true;
        i++;
      }
      if (decimal)
        m_parts.append(QVariant(s.mid(start, i - start).toDouble()));
      else
        m_parts.append(QVariant(s.mid(start, i - start).toInt()));
    } else if (single.contains(s[i]))
      m_parts.append(QVariant(QString(s[i++])));
    else if (s[i] == '\"')
    {
      i++;
      while (i < len && s[i] != '\"')
        i++;
      m_parts.append(QVariant(s.mid(start + 1, i - start - 1)));
      i++;
    } else if (s[i].isSpace())
      while (i < len && s[i].isSpace())
        i++;
    else
    {
      while (i < len && !s[i].isSpace())
        i++;
      m_parts.append(QVariant(s.mid(start, i - start)));
    }
    start = i;
  }
  return *this;
}

QString Expression::next() const
{
  if (m_start < m_parts.count())
    return m_parts[m_start].toString();
  else 
    return QString::null;
}

bool Expression::validate()
{
  if (m_start >= m_parts.count())
    setError();
  return !m_error;
}

Expression::Type Expression::commonType(const QVariant v1, const QVariant v2) const
{
  if (v1.type() == QVariant::String || v2.type() == QVariant::String)
    return TypeString;
  else if (v1.type() == QVariant::Double || v2.type() == QVariant::Double)
    return TypeDouble;
  return TypeInt;
}

int compareDouble(const double A, const double B)
{ 
  return A<B ? -1 : (A==B ? 0 : 1);
}


int Expression::compare(const QVariant v1, const QVariant v2) const
{
  switch (commonType(v1, v2))  {
    case TypeString: return v1.toString().compare(v2.toString());
    case TypeDouble: return compareDouble(v1.toDouble(), v2.toDouble());
    case TypeInt:    return v1.toInt() - v2.toInt();
    default:	 return 0;
  }
}


void Expression::setError(int pos)
{
  m_errorPosition = pos == -1 ? m_start : pos;
  m_error = true;
}

QVariant Expression::parseNumber()
{
  if (!validate()) 
    return -1;
  return m_parts[m_start++];
}

QVariant Expression::parseMinus()
{
  if (!validate()) return -1;
  bool sign = next() == "-";
  if (sign)
  {
    m_start++;
    QVariant value = parseNumber();
    if (value.type() == QVariant::Double)
      return -value.toDouble();
    else
      return -value.toInt();
  }
  else 
    return parseNumber();
}



QVariant Expression::parseBracket()
{
  if (!validate()) return -1;
  if (next() == "(")
  {
    m_start++;
    QVariant value = parse();
    if (next() == ")")
      m_start++;
    else
      setError();
    return value;  
  }
  else 
    return parseMinus();
}


QVariant Expression::parseMultiply()
{
  if (!validate()) return -1;
  QVariant value = parseBracket();
  QString op = next();
  while (op == "*" || op == "/" || op == "%")
  {
    m_start++;
    QVariant value2 = parseBracket();
    Type mode = commonType(value, value2);
    if (op == "*")
      if (mode == TypeDouble)
        value = value.toDouble() * value2.toDouble();
      else
        value = value.toInt() * value2.toInt();
    else if (op == "/")
      if (mode == TypeDouble)
        value = value.toDouble() / value2.toDouble();
      else
        value = value.toInt() / value2.toInt();
    else 
      if (mode == TypeDouble)
        value = value.toDouble() / value2.toInt();
      else
        value = value.toInt() / value2.toInt();
    op = next();
  }
  return value;
}

QVariant Expression::parseAdd()
{
  if (!validate()) return -1;
  QVariant value = parseMultiply();
  QString op = next();
  while (next() == "+" || next() == "-")
  {
    m_start++;
    QVariant value2 = parseMultiply();
    Type mode = commonType(value, value2);
    if (op == "+")
      if (mode == TypeDouble)
        value = value.toDouble() + value2.toDouble();
      else
        value = value.toInt() + value2.toInt();
    else
      if (mode == TypeDouble)
        value = value.toDouble() - value2.toDouble();
      else
        value = value.toInt() - value2.toInt();
  }
  return value;
}

QVariant Expression::parseComparison()
{
  if (!validate()) return -1;
  QVariant value = parseAdd();
  QString cmp = next();
  if (cmp == "<" || cmp == "<=" || cmp == "==" || cmp == ">=" || cmp == ">" || cmp == "<>" || cmp == "!=")
  {
    m_start++;
    QVariant value2 = parseAdd();
    if (cmp == "<")
      return compare(value, value2) < 0;
    else if (cmp == "<=")
      return compare(value, value2) <= 0;
    else if (cmp == "==")
      return compare(value, value2) == 0;
    else if (cmp == ">=")
      return compare(value, value2) >= 0;
    else if (cmp == "<>" || cmp == "!=")
      return compare(value, value2) != 0;
    else 
      return compare(value, value2) > 0;
  }
  return value;
}

QVariant Expression::parseNot()
{
  if (next() == "!" || next() == "not")
  {
    m_start++;    
    return !parseComparison().asBool();
  }
  else 
    return parseComparison();
}

QVariant Expression::parseAnd()
{
  if (!validate()) return -1;
  QVariant value = parseNot();
  while (next() == "&&" || next() == "and")
  {
    m_start++;    
    value = parseNot().toBool() && value.toBool();
  }
  
  return value;
}

QVariant Expression::parseOr()
{
  if (!validate()) return -1;
  QVariant value = parseAnd();
  while (next() == "||" || next() == "or")
  {
    m_start++;    
    value = parseAnd().toBool() || value.toBool();
  }
  return value;
}

QVariant Expression::parse()
{ 
  return parseOr();
}

QVariant Expression::value(bool* valid)
{
  m_start = 0;
  m_error = false;
  QVariant val = parse();
  if (valid)
     *valid = !m_error && m_start == m_parts.count();
  return val;
}

QVariant Expression::value(const QString& s, bool* valid)
{
  *this = s;
  return value(valid); 
}

bool Expression::isTrue(const QString& s, bool* valid)
{
  QVariant v = value(s, valid);
  return (v.type() == QVariant::String && !v.toString().isNull()) || 
      (v.type() != QVariant::String && v.toInt() != 0);
}

