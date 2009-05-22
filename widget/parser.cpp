/***************************************************************************
                    parser.cpp - Internal parser
                             -------------------
    copyright          : (C) 2004-2006  Michal Rudolf <mrudolf@kdewebdwev.org>

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
  m_error(QString::null), m_errorPosition(0), m_widget(0)
{
  setString(expr);
}

bool Parser::setString(const QString& s)
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
    else if (s[start].isLetter() || s[start] == '_')  // keyword
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
      insertNode(s.mid(start, 1), lines);
      setError(i18n("Invalid character: '%1'").arg(s[start]), m_parts.count()-1);
      return false;
    }
  }
  return true;
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

QString Parser::errorMessage() const
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
    return QString();
}

bool Parser::isError() const
{
  return !m_error.isNull();
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

ParseNode Parser::parseConstant(Parse::Mode)
{
  ParseNode p = next();
  m_start++;
  if (!p.isValue())
  {
    setError(i18n("Constant value expected"));
    return ParseNode();
    }
  return p;
}

ParseNode Parser::parseValue(Mode mode)
{
  ParseNode p = next();
  //QString p2 = QString(p.toString());
  //qDebug("parseValue p2 = "+p2);
  if (isFunction())
    return parseFunction(mode);
  else if (isWidget())
    return parseWidget(mode);
  else if (tryVariable(CheckOnly))
  {
    if (tryKeyword(LeftBracket, CheckOnly))
    {
      QString index = parseValue(mode).toString();
      tryKeyword(RightBracket);
      QString arr = p.variableName();
      return arrayValue(arr, index);
    }
    else if (tryKeyword(Dot, CheckOnly))
    {
      QString value = variable(p.variableName()).toString();
      if (m_widget && m_widget->isWidget(value))
      {
        m_start--;
        return parseWidget(mode, value);
      }else if (mode == Execute)
      {
        setError(i18n("'%1' (%2) is not a widget").arg(p.variableName()).arg(variable(p.variableName()).toString()));
        return ParseNode();
      } else
      {
        //this means it looks like a widget, but it is unknown. As we only check
        //the syntax, we should ignore the error an parse as a widget. 
        m_start = m_start - 2;
        return parseWidget(mode); 
      }
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
/*  else if (isArray(p2))
  {
    return p2;
  }*/
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
    if (mode == Execute)
    {
      if (k == Multiply)
        if (type == ValueInt)
          p = p.toInt() * p2.toInt();
        else
          p = p.toDouble() * p2.toDouble();
      else if (k == Divide)
      {
        if (p2.toDouble() == 0.0)
          setError(i18n("Divide by zero"));
        else 
        if (type == ValueInt)
          p = p.toInt() / p2.toInt();
        else
          p = p.toDouble() / p2.toDouble();
      }
      else  /* k == Mod */
      {
        if (p2.toInt() == 0)
          setError(i18n("Divide by zero"));
        else
          p = p.toInt() -  p.toInt() / p2.toInt() * p2.toInt();
      }
    }
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
    if (mode == Execute)
    {
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
  ParseNode p = parseAnd(mode);
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
  int pos = m_start;
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
    setError(i18n("in function '%1': %2").arg(name).arg(i18n("too few parameters")), pos);
  else if (f.maxArgs() < params.count())
    setError(i18n("in function '%1': %2").arg(name).arg(i18n("too many parameters")), pos);
  else if (mode == Execute)
  {
    ParseNode p = f.execute(this, params);
    if (!p.isValid())
    {
      setError(i18n("in function '%1': %2").arg(name).arg(p.errorMessage()), pos);
      return ParseNode();
    }
    else
      return p;
  }
  return ParseNode();
}

ParseNode Parser::parseWidget(Mode mode, const QString &widgetName)
{
  int pos = m_start;
  QString widget;
  if (widgetName.isNull())
   widget = nextVariable(mode);
  else
   widget = widgetName; 
  Function f = m_data->function("internalDcop");

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
  {
    ParseNode p = f.execute(this, params);
    if (!p.isValid())
    {
      setError(i18n("in widget function '%1.%2': %3").arg(widget).arg(var).arg(p.errorMessage()), pos);
      return ParseNode();
    }
    else
      return p;
  }
  return ParseNode();
}


ParseNode Parser::parseAssignment(Mode mode)
{
  QString var = nextVariable();
  //qDebug("var = "+var);
  if (tryKeyword(LeftBracket, CheckOnly))
  {
    QString index = parseValue(mode).toString();
    tryKeyword(RightBracket);
    tryKeyword(Assign);
    ParseNode p = parseExpression(mode);
    if (mode == Execute)
      setArray(var, index, p);
  }
  else if (tryKeyword(Assign, CheckOnly))
  {
    ParseNode p = parseExpression(mode);
    if (mode == Execute)
    {
      QString p2 = QString(p.toString()); //arrays are not found?
      //qDebug("p2 = "+p2);
      if (isArray(p2))
        const QMap<QString, ParseNode> var = array(p2);
      else
        setVariable(var, p);
    }
  }
  else if (tryKeyword(PlusEqual, CheckOnly))
  {
    ParseNode p = parseExpression(mode);
    if (mode == Execute)
    {
      ParseNode p2 = variable(var);
      if (p2.type() == ValueString)
        p = QString(p2.toString() + p.toString());
      else if (p2.type() == ValueDouble)
        p = p2.toDouble() + p.toDouble();
      else
        p = p2.toInt() + p.toInt();
      setVariable(var, p);
    }
  }
  else if (tryKeyword(MinusEqual, CheckOnly))
  {
    ParseNode p = parseExpression(mode);
    if (mode == Execute)
    {
      ParseNode p2 = variable(var);
      if (p2.type() == ValueDouble)
        p = p2.toDouble() - p.toDouble();
      else
        p = p2.toInt() - p.toInt();
      setVariable(var, p);
    }
  }
  else if (tryKeyword(Increment, CheckOnly))
  {
    //ParseNode p = parseExpression(mode);
    if (mode == Execute)
    {
      ParseNode p = variable(var);
      p = p.toInt() + 1;
      setVariable(var, p);
    }
  }
  else if (tryKeyword(Decrement, CheckOnly))
  {
    //ParseNode p = parseExpression(mode);
    if (mode == Execute)
    {
      ParseNode p = variable(var);
      p = p.toInt() - 1;
      setVariable(var, p);
    }
  }
  else if (tryKeyword(Dot, CheckOnly))
  {
    QString value = variable(var).toString();
    if (m_widget && m_widget->isWidget(value))
    {
      m_start--;
      return parseWidget(mode, value);
    } else
    if (mode == CheckOnly)
    {
      //this means it looks like a widget, but it is unknown. As we only check
      //the syntax, we should ignore the error an parse as a widget. 
      m_start = m_start - 2;
      return parseWidget(mode); 
    } else
      setError(i18n("'%1' is not a widget").arg(var));
  }
  else if (tryKeyword(LeftParenthesis, CheckOnly))
    setError(i18n("'%1' is not a function").arg(var));
  else 
    setError(i18n("Unexpected symbol after variable '%1'").arg(var));

  return ParseNode();
}

Flow Parser::parseIf(Mode mode)
{
  ParseNode p = next();
  Flow flow = FlowStandard;
  bool matched = false;
  bool thenFound = false;
  do {
    m_start++;
    Mode m = matched ? CheckOnly : mode;
    p = parseCondition(m);
    thenFound = tryKeyword(Then, CheckOnly);
    if (!thenFound)
      tryKeyword(LeftCurlyBrace);
    bool condition = !matched && p.toBool();
    if (condition)
    {
      flow = parseBlock(mode);
      if (flow == FlowExit)
        return flow;
    }
    else 
      parseBlock(CheckOnly);
    matched = matched || p.toBool();
    if (!thenFound)
      tryKeyword(RightCurlyBrace);
  } while (next().isKeyword(Elseif));
  bool braceFound = false;
  if (tryKeyword(Else, CheckOnly))
  {
    braceFound = tryKeyword(LeftCurlyBrace, CheckOnly);
    if (!matched)
      flow = parseBlock(mode);
    else
      parseBlock(CheckOnly);
  }
  if (braceFound)
    tryKeyword(RightCurlyBrace);
  if (thenFound)
  tryKeyword(Endif);
  return flow;
}

Parse::Flow Parser::parseWhile(Mode mode)
{
  m_start++;
  int start = m_start;
  bool running = true;
  Parse::Flow flow = FlowStandard;
  bool doFound = false;
  while (running)
  {
    m_start = start;
    ParseNode p = parseCondition(mode);
    doFound = tryKeyword(Do, CheckOnly);
    if (!doFound && !tryKeyword(LeftCurlyBrace))
      break;
    running = p.toBool();
    flow = parseBlock(running ? mode : CheckOnly);
    if ( flow == FlowBreak || flow == FlowExit)
      break;
  }
  if (flow != FlowExit)
  {
    if (doFound)
      tryKeyword(End);
    else
      tryKeyword(RightCurlyBrace);
    return FlowStandard;
  }
  else 
    return FlowExit;
}

Parse::Flow Parser::parseFor(Mode mode)
{
  m_start++;
  QString var = nextVariable();
  tryKeyword(Assign);
  int start = parseExpression(mode).toInt();
  tryKeyword(To);
  int end = parseExpression(mode).toInt();
  int step = 1;
  if (tryKeyword(Step, CheckOnly))
    step = parseExpression(mode).toInt();

  bool doFound = tryKeyword(Do, CheckOnly);
  if (!doFound)
    tryKeyword(LeftCurlyBrace);
  int block = m_start;
  Parse::Flow flow = FlowStandard;
  if (end >= start && step > 0)
  {
    for (int i = start; i <= end; i+=step)
    {
      m_start = block;
      setVariable(var, ParseNode(i));
      flow = parseBlock(mode);
      if (flow == FlowBreak || flow == FlowExit)
        break;
    }
  } else if (end <= start && step < 0)
  {
    for (int i = start; i >= end; i+=step)
    {
      m_start = block;
      setVariable(var, ParseNode(i));
      flow = parseBlock(mode);
      if (flow == FlowBreak || flow == FlowExit)
        break;
    }
  } else
    parseBlock(Parse::CheckOnly);
  if (flow != FlowExit)
  {
    if (doFound)
      tryKeyword(End);
    else
      tryKeyword(RightCurlyBrace);
    return FlowStandard;
  }
  else 
    return FlowExit;
}

Parse::Flow Parser::parseForeach(Mode mode)
{
  m_start++;
  QString var = nextVariable();
  tryKeyword(In);
  QString arr = nextVariable();
  bool doFound = tryKeyword(Do, CheckOnly);
  if (!doFound)
    tryKeyword(LeftCurlyBrace);
  int start = m_start;
  Parse::Flow flow = FlowStandard;
  if (isArray(arr) && array(arr).count())
  {
    const QMap<QString, ParseNode> A = array(arr);
    for (QMapConstIterator<QString, ParseNode> It = A.begin(); It != A.end(); ++It)
    {
      m_start = start;
      setVariable(var, It.key());
      flow = parseBlock(mode);
      if (flow == FlowBreak || flow == FlowExit)
        break;
    }
  }
  else 
    parseBlock(CheckOnly);
  if (flow != FlowExit)
  {
    if (doFound)
      tryKeyword(End);
    else
      tryKeyword(RightCurlyBrace);
    return FlowStandard;
  }
  else 
    return FlowExit;
}

void Parser::parseSwitch(Mode mode)
{
  m_start++;
  QString var = nextVariable();
  ParseNode caseValue = variable(var);
  bool executed = false;
  bool braceFound = false;
  braceFound = tryKeyword(LeftCurlyBrace, CheckOnly);
  tryKeyword(Semicolon, CheckOnly);
  while (tryKeyword(Case, CheckOnly))
  {
    ParseNode p = parseConstant();
    bool matched = mode == Execute && p == caseValue;
    parseBlock(matched ? Execute : CheckOnly);
    if (matched)
      executed = true;
  }
  if (tryKeyword(Else, CheckOnly))
    parseBlock(executed ? CheckOnly : mode);
  if (!braceFound)
    tryKeyword(End);
  else
    tryKeyword(RightCurlyBrace);
}

Flow Parser::parseCommand(Mode mode)
{
  ParseNode p = next();
  if (next().isKeyword(If))
    return parseIf(mode);
  else if (next().isKeyword(While))
    return parseWhile(mode);
  else if (next().isKeyword(For))
    return parseFor(mode);
  else if (next().isKeyword(Foreach))
    return parseForeach(mode);
  else if (next().isKeyword(Switch))
    parseSwitch(mode);
  else if (tryKeyword(Continue, CheckOnly))
    return FlowContinue;
  else if (tryKeyword(Break, CheckOnly))
    return FlowBreak;  
  else if (isFunction())
  {
    QString name = next().variableName();    
    parseFunction(mode);
    if (name == "return" && mode == Execute)
      return FlowExit;
  }
  else if (isWidget())
    parseWidget(mode);
  else if (next().isVariable())
    parseAssignment(mode);
  else if (tryKeyword(Exit, CheckOnly))
  {
     if (mode == Execute)
       return FlowExit;
  } 
  return FlowStandard;
}

Flow Parser::parseBlock(Mode mode)
{
  Flow flow = parseCommand(mode);
  while (tryKeyword(Semicolon, CheckOnly) && flow != FlowExit)
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
  {
    if (k == Dot)
      setError(i18n("Expected '%1'<br><br>Possible cause of the error is having a variable with the same name as a widget").arg(m_data->keywordToString(k)));
    else
     setError(i18n("Expected '%1' got '%2'.").arg(m_data->keywordToString(k)).arg(next().toString()));
  }
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

QString Parser::nextVariable(Mode mode)
{
  if (next().isVariable())
  {
    QString name = next().variableName();
    m_start++;
    return name;
  }
  else if (mode == Execute)
    setError(i18n("Expected variable"));
  return QString();
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
  if (m_error.isNull())
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

const QMap<QString, ParseNode>& Parser::array(const QString& name) const
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
    if (key.isNull())
      m_globalArrays.remove(name);
    else if (isArray(name))
      m_globalArrays[name].remove(key);
  }
  else
  {
    if (key.isNull())
      m_arrays.remove(name);
    else if (isArray(name))
      m_arrays[name].remove(key);
  }
}

ParseNode Parser::arrayValue(const QString& name, const QString& key) const
{
  if (!isArray(name))
    return ParseNode();
  if (isGlobal(name))
    return m_globalArrays[name].contains(key) ? m_globalArrays[name][key] : ParseNode();
  else
    return m_arrays[name].contains(key) ? m_arrays[name][key] : ParseNode();
}



KommanderWidget* Parser::currentWidget() const
{
  return m_widget;
}

QMap<QString, ParseNode> Parser::m_globalVariables;
QMap<QString, QMap<QString, ParseNode> > Parser::m_globalArrays;


