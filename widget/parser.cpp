/***************************************************************************
                    parser.cpp - Internal parser
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

#include <klocale.h> 
 
#include "parser.h"
#include "parserdata.h"
#include "kommanderwidget.h"

using namespace Parse;

QString unescape(QString s)
{
  return s.replace("\\\"", "\"").replace("\\t", "\t").replace("\\n", "\n").replace("\\\\", "\\");
}

Parser::Parser(ParserData* pData) : m_data(pData), m_start(0), m_error(QString::null), m_errorPosition(0), 
  m_widget(0)
{
}
  
Parser::Parser(ParserData* pData, const QString& expr) : m_data(pData), m_start(0), 
  m_error(QString::null),  m_errorPosition(0), m_widget(0)
{
  setString(expr);
}

void Parser::setString(const QString& s)
{
  reset();
  m_parts.clear();
  uint lines = 0;
  uint start = 0;
  uint i = 0;
  while (start < s.length())
  {
    if (s[start].isSpace() && s[start] != '\n')
      start++;
    else if (s[start] == '\\' && start < s.length() - 1 && s[start+1] == '\n')
      start += 2;
    else if (s[start] == ';')
    {
      insertNode(Semicolon, lines);
      start++;
    }
    else if (s[start] == '\n')
    {
      if (m_parts.count() && !m_parts.last().isKeyword(Semicolon))
        insertNode(Semicolon, lines);
      lines++;
      start++;
    }
    else if (s[start] == '\"')       // quoted string: "abc"
    {
      bool escaped = false;
      for (i = start + 1; i < s.length() && (s[i] != '\"' || s[i-1] == '\\'); i++)
        if (!escaped) 
          escaped = s[i] == '\\';
      
      if (escaped)
        insertNode(unescape(s.mid(start + 1, i - start - 1)), lines);
      else
        insertNode(s.mid(start + 1, i - start - 1), lines);
      start = i+1;
    }
    else if (s[start].isDigit())    // number: 1000 or 2.45
    {
      bool decimal = false;
      for (i = start+1; s[i].isDigit() || (!decimal && s[i] == QChar('.')); i++)
        if (s[i] == '.')
          decimal = true;
      if (decimal)
        insertNode(s.mid(start, i - start).toDouble(), lines);
      else
        insertNode(s.mid(start, i - start).toInt(), lines);
      start = i;
    }
    else if (s[start].isLetter() || s[start] == '_')
    {
      for (i = start+1; s[i].isLetterOrNumber() || s[i] == '_'; i++)
        ;
      QString name = s.mid(start, i - start);
      insertNode(ParseNode(m_data->stringToKeyword(name), name), lines);
      start = i;
    }                               // comment
    else if (s[start] == '#' || (s[start] == '/' && start < s.length() +1 && s[start+1] == '/'))
    {
      while (start < s.length() && s[start] != '\n')
        start++;
    }                              // special keyword: <>
    else if (m_data->stringToKeyword(s.mid(start, 2)) <= LastRealKeyword)
    {
      insertNode(m_data->stringToKeyword(s.mid(start, 2)), lines);
      start += 2;
    }                             // special keyword: <
    else if (m_data->stringToKeyword(s.mid(start, 1)) <= LastRealKeyword)
    {
      insertNode(m_data->stringToKeyword(s.mid(start, 1)), lines);
      start++;
    }
    else                          // Bad character
    {
      setError(i18n("Invalid character: '%1'").arg(s[start]), m_parts.count());
      return;
    }
  }
  
  /*
  for (uint i=0; i<m_parts.count(); i++)
  {
    if (m_parts[i].isVariable())
      qDebug("%d. variable/%s", i, m_parts[i].variableName().latin1()); 
    else if (m_parts[i].isKeyword())
  qDebug("%d. keyword/%s", i, m_data->keywordToString(m_parts[i].keyword()).latin1()); 
    else qDebug("%d. %d/%s", i, m_parts[i].type(), m_parts[i].toString().latin1()); 
   }
  */
}

void Parser::setWidget(KommanderWidget* w)
{
  m_widget = w;  
}

void Parser::insertNode(ParseNode p, int line)
{
  p.setContext(line);
  m_parts.append(p);
}
  

QString Parser::errorMessage()
{
  return m_error;
}

QString Parser::function(ParserData* data, const QString& name, const QStringList& params)
{
  ParameterList par;
  for (QStringList::ConstIterator Iter = params.begin(); Iter != params.end(); ++Iter)
    par.append(*Iter);
  Function f = data->function(name);
  return f.execute(0, par).toString();
}

  
QString Parser::expression(Mode mode)
{
  reset();
  ParseNode p = parseExpression(mode);
  if (!isError())
    return p.toString();
  else
    return QString::null;
}

bool Parser::isError() const
{
  return m_error != QString::null;
}


bool Parser::command(Mode mode)
{
  reset();
  parseCommand(mode);
  return !isError();
}

bool Parser::parse(Mode mode)
{
  reset();
  parseBlock(mode);
  return !isError();
}

int Parser::errorLine() const
{
  if (isError())
    return m_parts[m_errorPosition].context();
  else 
    return -1;
  
}

ParseNode Parser::parseValue(Mode mode)
{
  ParseNode p = next();
  if (isFunction())
    return parseFunction();
  else if (isWidget())
    return parseWidget(mode);
  else if (tryVariable(CheckOnly))
  {
    if (tryKeyword(LeftBracket, CheckOnly))
    {
      QString index = parseValue(mode).toString();
      tryKeyword(RightBracket);
      QString arr = p.variableName();
      return (isArray(arr) && array(arr).contains(index)) ? array(arr)[index] : QString::null;
    }
    else if (tryKeyword(Dot, CheckOnly))
    {
      setError(i18n("'%1' is not a widget").arg(p.variableName()));
      return ParseNode();
    }
    else if (tryKeyword(LeftParenthesis, CheckOnly))
    {
      setError(i18n("'%1' is not a function").arg(p.variableName()));
      return ParseNode();
    }
    else
      p = variable(p.variableName());
  }
  else if (tryKeyword(False, CheckOnly))
    return ParseNode(0);
  else if (tryKeyword(True, CheckOnly))
    return ParseNode(1);
  else if (p.isKeyword())
    setError(i18n("Expected value"));
  else // single value
    m_start++;
  return p;
}

ParseNode Parser::parseMultiply(Mode mode)
{
  ParseNode p = parseParenthesis(mode);
  while (m_data->keywordGroup(next().keyword()) == GroupMultiply)
  {
    Keyword k = next().keyword();
    m_start++;
    ParseNode p2 = parseParenthesis(mode);
    ValueType type = p.commonType(p2);
    if (k == Multiply)
      if (type == ValueDouble)
        p = p.toDouble() * p2.toDouble();
      else
        p = p.toInt() * p2.toInt();
    else if (k == Divide)
      if (type == ValueDouble)
        p = p.toDouble() / p2.toDouble();
      else
        p = p.toInt() / p2.toInt();
    else  /* k == Mod */
      p = p.toInt() % p2.toInt();
  }
  return p;
}

ParseNode Parser::parseAdd(Mode mode)
{
  ParseNode p = parseMultiply(mode);
  while (m_data->keywordGroup(next().keyword()) == GroupAdd)
  {
    Keyword k = next().keyword();
    m_start++;
    ParseNode p2 = parseMultiply(mode);
    ValueType type = p.commonType(p2);
    if (k == Plus)
      if (type == ValueString)
        p = QString(p.toString() + p2.toString());
      else if (type == ValueDouble)
        p = p.toDouble() + p2.toDouble();
      else
        p = p.toInt() + p2.toInt();
    else  /* k == Minus */
      if (type == ValueDouble)
        p = p.toDouble() - p2.toDouble();
      else
        p = p.toInt() - p2.toInt();
  }
  return p;
}

ParseNode Parser::parseSignedNumber(Mode mode)
{
  if (tryKeyword(Minus, CheckOnly))
  {
    ParseNode p = parseValue(mode);
    if (p.type() == ValueDouble)
      return ParseNode(-p.toDouble());
    else
      return ParseNode(-p.toInt());
  }
  else 
    return parseValue(mode);
}

ParseNode Parser::parseComparison(Mode mode)
{
  ParseNode p1 = parseAdd(mode);
  if (m_data->keywordGroup(next().keyword()) == GroupComparison)
  {
    Keyword k = next().keyword();
    m_start++;
    ParseNode p2 = parseAdd(mode);
    switch (k) {
      case Less:          return ParseNode(p1 < p2);
      case LessEqual:     return ParseNode(p1 <= p2);
      case Equal:         return ParseNode(p1 == p2);
      case NotEqual:      return ParseNode(p1 != p2);
      case GreaterEqual:  return ParseNode(p1 >= p2);
      case Greater:       return ParseNode(p1 > p2);
      default:            ;
    }
  }
  return p1;
}

ParseNode Parser::parseParenthesis(Mode mode)
{
  if (tryKeyword(LeftParenthesis, CheckOnly))
  {
    ParseNode p = parseExpression(mode);
    tryKeyword(RightParenthesis);
    return p;
  }
  else 
    return parseSignedNumber(mode);
}


ParseNode Parser::parseNot(Mode mode)
{
  if (tryKeyword(Not, CheckOnly))
    return !parseComparison(mode).toBool();
  else 
    return parseComparison(mode);
}

ParseNode Parser::parseAnd(Mode mode)
{
  ParseNode p = parseNot(mode);
  while (tryKeyword(And, CheckOnly))
  {
    if (p == false)
      parseNot(CheckOnly);
    else
      p = parseNot(mode);
  }
  return p;
}    

ParseNode Parser::parseOr(Mode mode)
{
  ParseNode p = parseNot(mode);
  while (tryKeyword(Or, CheckOnly))
  {
    if (p == true)
      parseAnd(CheckOnly);
    else
      p = parseAnd(mode);
  }
  return p;
}

ParseNode Parser::parseCondition(Mode mode)
{
  return parseOr(mode);
}
  
ParseNode Parser::parseExpression(Mode mode)
{
  return parseOr(mode);
}

ParseNode Parser::parseFunction(Mode mode)
{
  QString name = next().variableName();
  Function f = m_data->function(name);
  m_start++;
  ParameterList params;
  
  if (tryKeyword(LeftParenthesis, CheckOnly) && !tryKeyword(RightParenthesis, CheckOnly))
  {
    do {
      params.append(parseExpression(mode));
    } while (tryKeyword(Comma, CheckOnly));
    tryKeyword(RightParenthesis);
  }
  if (f.minArgs() > params.count())
    setError(i18n("Too few parameters for function '%1'").arg(name));
  else if (f.maxArgs() < params.count())
    setError(i18n("Too many parameters for function '%1'").arg(name));
  else if (mode == Execute)
    return f.execute(this, params);
  return ParseNode();
}

ParseNode Parser::parseWidget(Mode mode)
{
  QString widget = nextVariable();
  Function f = m_data->function("dcop");
  
  if (!tryKeyword(Dot))
    return ParseNode();
  QString var = nextVariable();
  if (var.isNull())
    return ParseNode();
  ParameterList params;
  params.append(var);
  params.append(widget);
  
  if (tryKeyword(LeftParenthesis, CheckOnly) && !tryKeyword(RightParenthesis, CheckOnly))
  {
    do {
      params.append(parseExpression(mode));
    } while (tryKeyword(Comma, CheckOnly));
    tryKeyword(RightParenthesis);
  }
  if (mode == Execute)
    return f.execute(this, params);
  return ParseNode();
}


void Parser::parseAssignment(Mode mode)
{
  QString var = nextVariable();
  if (tryKeyword(LeftBracket, CheckOnly))
  {
    QString index = parseValue(mode).toString();
    tryKeyword(RightBracket);
    tryKeyword(Assign);
    ParseNode p = parseExpression(mode);
    setArray(var, index, p);
  }
  else if (tryKeyword(Assign, CheckOnly))
  {
    ParseNode p = parseExpression(mode);
    if (mode == Execute)
      setVariable(var, p);
  }
  else if (tryKeyword(Dot, CheckOnly))
    setError(i18n("'%1' is not a widget").arg(var));
  else if (tryKeyword(LeftParenthesis, CheckOnly))
    setError(i18n("'%1' is not a function").arg(var));
  else 
    setError(i18n("Unexpected symbol after variable '%1'").arg(var));
}

Flow Parser::parseIf(Mode mode)
{
  ParseNode p = next();
  Flow flow = FlowStandard;
  bool matched = false;
  do {
    m_start++;
    p = parseCondition(mode);
    tryKeyword(Then);
    bool condition = !matched && p.toBool();
    if (condition)
      flow = parseBlock(mode);
    else 
      parseBlock(CheckOnly);
    matched = matched || p.toBool();
  } while (next().isKeyword(Elseif));
  if (tryKeyword(Else, CheckOnly))
  {
    if (!matched)
      flow = parseBlock(mode);
    else
      parseBlock(CheckOnly);
  }
  tryKeyword(Endif);
  return flow;
}

void Parser::parseWhile(Mode mode)
{
  next();
  m_start++;
  int start = m_start;
  bool running = true;
  while (running)
  {
    m_start = start;
    ParseNode p = parseCondition(mode);
    if (!tryKeyword(Do))
      break;
    running = p.toBool();
    if (parseBlock(running ? mode : CheckOnly) == FlowBreak)
      break;
  }
  tryKeyword(End);
}

void Parser::parseFor(Mode mode)
{
  next();
  m_start++;
  QString var = nextVariable();
  tryKeyword(Assign);
  int start = parseExpression(mode).toInt();
  tryKeyword(To);
  int end = parseExpression(mode).toInt();
  int step = 1;
  if (tryKeyword(Step, CheckOnly))
    step = parseExpression(mode).toInt();
  tryKeyword(Do);
  int block = m_start;
  for (int i = start; i <= end; i+=step)
  {
    m_start = block;
    setVariable(var, ParseNode(i));
    if (parseBlock(mode) == FlowBreak)
      break;
  }
  tryKeyword(End);
}
  
void Parser::parseForeach(Mode mode)
{
  next();
  m_start++;
  QString var = nextVariable();
  tryKeyword(In);
  QString arr = nextVariable();
  tryKeyword(Do);
  int start = m_start;
  if (isArray(arr) && array(arr).count())
  {
    const QMap<QString, ParseNode> A = array(arr);
    for (QMapConstIterator<QString, ParseNode> It = A.begin(); It != A.end(); ++It)
    {
      m_start = start;
      setVariable(var, It.key());
      Flow flow = parseBlock(mode);
      if (flow == FlowBreak)
        break;
    }
  }
  else 
    parseBlock(CheckOnly);
  tryKeyword(End);
}

Flow Parser::parseCommand(Mode mode)
{
  ParseNode p = next();
  if (next().isKeyword(If))
    return parseIf(mode);
  else if (next().isKeyword(While))
    parseWhile(mode);
  else if (next().isKeyword(For))
    parseFor(mode);
  else if (next().isKeyword(Foreach))
    parseForeach(mode);
  else if (tryKeyword(Continue, CheckOnly))
    return FlowContinue;
  else if (tryKeyword(Break, CheckOnly))
    return FlowBreak;
  else if (isFunction())
    parseFunction(mode);
  else if (isWidget())
    parseWidget(mode);
  else if (next().isVariable()) 
    parseAssignment(mode);
  else if (tryKeyword(Exit, CheckOnly))
  {
    if (mode == Execute)
      setError("Exit");
#warning FIXME!    
    return FlowBreak;
  }
  return FlowStandard;
}
  
Flow Parser::parseBlock(Mode mode)
{
  Flow flow = parseCommand(mode);
  while (tryKeyword(Semicolon, CheckOnly))
  {
    if (flow == FlowStandard)
      flow = parseCommand(mode);
    else
      parseCommand(CheckOnly);
  }    
  return flow;
}




ParseNode Parser::next() const
{
  if (isError() || m_start >= m_parts.count())
    return ParseNode();
  return m_parts[m_start];
}

bool Parser::tryKeyword(Keyword k, Mode mode)
{
  if (next().isKeyword(k))
  {
    m_start++;
    return true;
  }
  if (mode == Execute)
    setError(i18n("Expected '%1'").arg(m_data->keywordToString(k)));
  return false;
}

bool Parser::tryVariable(Mode mode)
{
  if (next().isVariable())
  {
    QString name = next().variableName();
    m_start++;
    return true;
  }
  if (mode == Execute)
    setError(i18n("Expected variable"));
  return false;
}

QString Parser::nextVariable()
{
  if (next().isVariable())
  {
    QString name = next().variableName();
    m_start++;
    return name;
  }
  else 
    setError(i18n("Expected variable"));
  return QString::null;
}


bool Parser::isFunction() const
{
  return next().isVariable() && m_data->isFunction(next().variableName());
}

bool Parser::isWidget() const
{
  return m_widget && next().isVariable() && m_widget->isWidget(next().variableName());
}

void Parser::reset()
{
  m_start = 0;
  m_error = QString::null;
  m_errorPosition = 0;
}

void Parser::setError(const QString& msg)
{
  setError(msg, m_start);
}

void Parser::setError(const QString& msg, int pos)
{
  if (m_error == QString::null)
  {
    m_errorPosition = pos;
    m_error = msg;
  } 
}

void Parser::setVariable(const QString& name, ParseNode value)
{
  if (isGlobal(name))
    m_globalVariables[name] = value;
  else
    m_variables[name] = value;
}

ParseNode Parser::variable(const QString& name) const
{
  if (isGlobal(name))
    return m_globalVariables.contains(name) ? m_globalVariables[name] : ParseNode();  
  else
    return m_variables.contains(name) ? m_variables[name] : ParseNode();  
}

bool Parser::isGlobal(const QString& name) const
{
  return !name.isEmpty() && name[0] == '_';
}

bool Parser::isVariable(const QString& name) const
{
  return m_variables.contains(name) || m_globalVariables.contains(name);
}

void Parser::unsetVariable(const QString& key)
{
  if (isGlobal(key))
    m_globalVariables.remove(key);
  else
    m_variables.remove(key);
}

const QMap<QString, ParseNode>& Parser::array(const QString& name)
{
  if (isGlobal(name))
    return m_globalArrays[name];
  else
    return m_arrays[name];
}

bool Parser::isArray(const QString& name) const
{
  return m_arrays.contains(name) || m_globalArrays.contains(name);
}

void Parser::setArray(const QString& name, const QString& key, ParseNode value)
{
  if (isGlobal(name))
    m_globalArrays[name][key] = value;
  else
    m_arrays[name][key] = value;
}
  
void Parser::unsetArray(const QString& name, const QString& key)
{
  if (isGlobal(name))
  {
    if (key == QString::null)
      m_globalArrays.remove(name);
    else if (isArray(name))
      m_globalArrays[name].remove(key);
  }
  else
  {
    if (key == QString::null)
      m_arrays.remove(name);
    else if (isArray(name))
      m_arrays[name].remove(key);
  }
}



KommanderWidget* Parser::currentWidget() const
{
  return m_widget;  
}
  
QMap<QString, ParseNode> Parser::m_globalVariables;
QMap<QString, QMap<QString, ParseNode> > Parser::m_globalArrays;

