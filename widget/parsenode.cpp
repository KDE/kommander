/***************************************************************************
                    parsenode.cpp - Single parsed item
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

#include "parsenode.h"

using namespace Parse;

ParseNode::ParseNode() : m_type(ValueNone), m_context(-1)
{
}

ParseNode::ParseNode(const QString& s) : m_type(ValueString), m_string(s), m_context(-1)
{
}

ParseNode::ParseNode(int i) : m_type(ValueInt), m_int(i), m_context(-1)
{  
}
    
ParseNode::ParseNode(uint i) : m_type(ValueInt), m_int(i), m_context(-1)
{  
}

ParseNode::ParseNode(double d) : m_type(ValueDouble), m_double(d), m_context(-1)
{  
}

ParseNode::ParseNode(Keyword k) : m_type(ValueKeyword), m_keyword(k), m_string(QString::null), m_context(-1)
{
}
  
ParseNode::ParseNode(Keyword k, const QString& name) : m_type(ValueKeyword), m_keyword(k), m_context(-1)
{
  m_string = (k == Variable) ? name : QString::null;
}

ParseNode ParseNode::error(const QString& s)
{
  ParseNode p;
  p.m_string = s;
  p.m_type = ValueError;
  return p;
}

ValueType ParseNode::type() const
{
  return m_type;
}
  
Keyword ParseNode::keyword() const
{
  return isKeyword() ? m_keyword : Invalid;
}

QString ParseNode::toString() const
{
  switch(type()) {
    case ValueString: return m_string;
    case ValueInt:    return QString::number(m_int);
    case ValueDouble: return QString::number(m_double);
    default:          return QString();
  }
}

int ParseNode::toInt() const
{
  switch(type()) {
    case ValueInt:    return m_int;
    case ValueDouble: return (int)m_double;
    case ValueString: return m_string.toInt();
    default:          return 0;
  }
}
    
double ParseNode::toDouble() const
{
  switch(type()) {
    case ValueDouble: return m_double;
    case ValueInt:    return (double)m_int;
    case ValueString: return m_string.toDouble();
    default:          return 0.0;
  }  
}

bool ParseNode::toBool() const
{
  return toInt() != 0;
}    
    
bool ParseNode::isValid() const
{
  return type() != ValueError;
}    
    
bool ParseNode::isKeyword() const
{
  return type() == ValueKeyword;  
}

bool ParseNode::isKeyword(Keyword k) const
{
  return type() == ValueKeyword && keyword() == k;
}

bool ParseNode::isVariable() const
{
  return type() == ValueKeyword && keyword() == Variable;
}

bool ParseNode::isArray() const
{
  return type() == ValueKeyword && keyword() == Array;
}

QString ParseNode::variableName() const
{
  return isVariable() ? m_string : QString();
}

QString ParseNode::arrayName() const
{
  return isArray() ? m_string : QString();
}

QString ParseNode::errorMessage() const
{
  return isValid() ? QString() : m_string;
}


ValueType ParseNode::commonType(const ParseNode& p) const
{
  if (type() == ValueKeyword || p.type() == ValueKeyword)
    return ValueKeyword;
  else if (type() == ValueString || p.type() == ValueString)
    return ValueString;
  else if (type() == ValueDouble || p.type() == ValueDouble)
    return ValueDouble;
  return ValueInt;
}

static int parsenode_compareDouble(const double A, const double B)
{ 
  return A<B ? -1 : (A==B ? 0 : 1);
}

int ParseNode::compare(const ParseNode& p) const
{
  switch (commonType(p)) 
  {
    case ValueString: return toString().compare(p.toString());
    case ValueDouble: return parsenode_compareDouble(toDouble(), p.toDouble());
    case ValueInt:    return toInt() - p.toInt();
    default:          return 0;
  }
}    

bool ParseNode::operator==(int i) const
{
  return toInt() == i;
}

bool ParseNode::operator==(bool b) const
{
  return toBool() == b;
}

bool ParseNode::operator==(const QString& s) const
{
  return toString() == s;
}
      
bool ParseNode::operator==(const ParseNode& p) const
{
  return compare(p) == 0; 
}
      
bool ParseNode::operator!=(const ParseNode& p) const
{
  return compare(p) != 0;
}

bool ParseNode::operator>=(const ParseNode& p) const
{
  return compare(p) >= 0;
}

bool ParseNode::operator<=(const ParseNode& p) const
{
  return compare(p) <= 0;
}

bool ParseNode::operator>(const ParseNode& p) const
{
  return compare(p) > 0;
}

bool ParseNode::operator<(const ParseNode& p) const
{
  return compare(p) < 0;
}

void ParseNode::setValue(int i)
{
  m_type = ValueInt;
  m_int = i;
}

void ParseNode::setValue(double d)
{
  m_type = ValueDouble;
  m_double = d;
}

void ParseNode::setValue(const QString& s)
{
  m_type = ValueString;
  m_string = s;
}

void ParseNode::setVariable(const QString& name)
{
  m_type = ValueKeyword;
  m_keyword = Variable;
  m_string = name;
}

void ParseNode::setArray(const QString& name)
{
  m_type = ValueKeyword;
  m_keyword = Array;
  m_string = name;
}

bool ParseNode::isValue() const
{
  return m_type <= ValueValue;
}

void ParseNode::setContext(int c)
{
  m_context = c;
}

int ParseNode::context() const
{
  return m_context;
}

