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

#ifndef _HAVE_PARSENODE_H_
#define _HAVE_PARSENODE_H_

#include <qstring.h>
 
namespace Parse
{
  enum Keyword {For, To, Step, End, While, Do, Foreach, In, If, Then, Else, Elseif, Endif, Switch, Case, 
    Break, Continue, Semicolon, Comma, Assign, Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual, 
    Not, And, Or, LeftParenthesis, RightParenthesis, LeftBracket, RightBracket,
    Plus, Minus, Multiply, Divide, Mod, LastRealKeyword = Mod,
    Variable, None, Value, Invalid, IncorrectParams};

  enum KeywordGroup {GroupComparison, GroupAdd, GroupMultiply, GroupMisc};
    
  enum ValueType {ValueString, ValueInt, ValueDouble, ValueKeyword, ValueNone = ValueKeyword};

  enum Mode{Execute, CheckOnly};

  enum Flow{FlowStandard, FlowContinue, FlowBreak};
};

class ParseNode {
public:
  /* Default constructor */
  ParseNode();
  /* Create node from string */
  ParseNode(const QString& s);
  /* Create node from integer */
  ParseNode(int i);
  /* Create node from integer */
  ParseNode(uint i);
  /* Create node from double */
  ParseNode(double d);
  /* Create node from keyword */
  ParseNode(Parse::Keyword k);
  /* Create node from keyword and variable name */
  ParseNode(Parse::Keyword k, const QString& s);
  
  /* Return current type */
  Parse::ValueType type() const;
  /* Return current keyword if appropriate */
  Parse::Keyword keyword() const;
  /* Cast value to string */
  QString toString() const;
  /* Cast value to integer */
  int toInt() const;
  /* Cast value to double */
  double toDouble() const;
  /* Cast value to bool */
  bool toBool() const;
  /* Check if a value is valid */
  bool isValid() const;
  /* Check if current value is a keyword */
  bool isKeyword() const;
  /* Check if current value is a given keyword */
  bool isKeyword(Parse::Keyword k) const;
  /* Check if current value is a variable */
  bool isVariable() const;
  /* Return the name of variable */
  QString variableName() const;
  /* Calculate common type for two nodes */
  Parse::ValueType commonType(const ParseNode& p) const;
  /* Find common type and compare values */
  int compare(const ParseNode& p) const;    
  /* Various comparing functions */
  bool operator==(int i) const;
  bool operator==(bool b) const;
  bool operator==(const QString& s) const;
  bool operator==(const ParseNode& p) const;
  bool operator!=(const ParseNode& p) const;
  bool operator>=(const ParseNode& p) const;
  bool operator<=(const ParseNode& p) const;
  bool operator>(const ParseNode& p) const;
  bool operator<(const ParseNode& p) const;
  /* set value as integer */
  void setValue(int i);
  /* set value as double */
  void setValue(double d);
  /* set value as string */
  void setValue(const QString& s);
  /* set value as variable */
  void setVariable(const QString& name);
  /* for setting some context information, f. e. for bug reporting */
  void setContext(int c);
  /* get current context */
  int context();
  
private:
  Parse::ValueType m_type;
  union {
    int m_int;
    double m_double;
    Parse::Keyword m_keyword;
  };
  QString m_string;
  int m_context;
};


#endif

