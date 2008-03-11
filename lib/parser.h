/***************************************************************************
                    parser.h - Internal parser
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

#ifndef _HAVE_PARSER_H_
#define _HAVE_PARSER_H_

#include "parsenode.h"
#include <qvector.h>
#include <qstringlist.h>
#include <qmap.h>

class KommanderWidget;
class ParserData;

class Parser
{
public:
  Parser(ParserData* data);
  Parser(ParserData* data, const QString& expr);
  // set string to parse
  void setString(const QString& s);
  // set Kommander widget associated with parser
  void setWidget(KommanderWidget* w);
  
  // parse generic expression
  QString expression(Parse::Mode mode = Parse::Execute);
  // execute single command; return true if ok
  bool command(Parse::Mode mode = Parse::Execute);
  // special class method to execute single parser function without creating parser object
  static QString function(ParserData* data, const QString& name, const QStringList& params);
  // execute whole block; return true if ok
  bool parse(Parse::Mode mode = Parse::Execute);
  // return line of errorneous node
  int errorLine() const;
  // return error message
  QString errorMessage();
  
  // check if this is a name of standard variable
  bool isVariable(const QString& name);
  // set variable value
  void setVariable(const QString& name, ParseNode value);
  // unset variable
  void unsetVariable(const QString& key);
  // get variable value
  ParseNode variable(const QString& name) const;
  // access associative array 
  const QMap<QString, ParseNode>& array(const QString& name);
  // check if this is a name of an array
  bool isArray(const QString& name) const;
  // set array key
  void setArray(const QString& name, const QString& key, ParseNode value);
  // unset array key or whole array
  void unsetArray(const QString& name, const QString& key = QString());
  // array value 
  ParseNode arrayValue(const QString& name, const QString& key) const;    
  // get associated widget
  KommanderWidget* currentWidget() const;

private:
  // parsing function - top-down approach
  
  // parse value (literal or variable)
  ParseNode parseValue(Parse::Mode mode = Parse::Execute);
  // parse multiplication, division and mod (x*y, x/y, x%y)
  ParseNode parseMultiply(Parse::Mode mode = Parse::Execute);
  // parse sum (x+y, x-y)
  ParseNode parseAdd(Parse::Mode mode = Parse::Execute);
  // parse signed numeric (+x, -x)
  ParseNode parseSignedNumber(Parse::Mode mode = Parse::Execute);

  /*  
  // parse string expression
  ParseNode parseStringValue(Parse::Mode mode = Parse::Execute);
  // parse string concatenation (x+y)
  ParseNode parseConcatenation(Parse::Mode mode = Parse::Execute);
  */
    
  // parse comparisons (x==y, x<y, x>y, x!=y, x<>y, x<=y, x>=y
  ParseNode parseComparison(Parse::Mode mode = Parse::Execute);
  // parse boolean not (!x, not x)
  ParseNode parseNot(Parse::Mode mode = Parse::Execute);
  // parse boolean and (x&&y, x and y)
  ParseNode parseAnd(Parse::Mode mode = Parse::Execute);
  // parse boolean or (x||y, x or y)
  ParseNode parseOr(Parse::Mode mode = Parse::Execute);
  // parse generic condition
  ParseNode parseCondition(Parse::Mode mode = Parse::Execute);
  
  // parse (x) expression
  ParseNode parseParenthesis(Parse::Mode mode = Parse::Execute);
  // parse generic expression
  ParseNode parseExpression(Parse::Mode mode = Parse::Execute);
  // parse parameters
  ParseNode parseFunction(Parse::Mode mode = Parse::Execute);
  // parse widget function
  ParseNode parseWidget(Parse::Mode mode = Parse::Execute);
  
  // parse assignment
  void parseAssignment(Parse::Mode mode = Parse::Execute);
  // parse conditional
  Parse::Flow parseIf(Parse::Mode mode = Parse::Execute);
  // parse assignment
  Parse::Flow parseCommand(Parse::Mode mode = Parse::Execute);
  // parse while loop
  void parseWhile(Parse::Mode mode = Parse::Execute);
  // parse for loop
  void parseFor(Parse::Mode mode = Parse::Execute);
  // parse foreach loop
  void parseForeach(Parse::Mode mode = Parse::Execute);
  // parse whole block
  Parse::Flow parseBlock(Parse::Mode mode = Parse::Execute);
  
  // insert next node
  void insertNode(ParseNode p, int line);
  // next item to be parsed
  ParseNode next() const;
  // check if next item is keyword k, if so - go further, if no, set error
  bool tryKeyword(Parse::Keyword k, Parse::Mode mode = Parse::Execute);
  // check if next item is a variable, if so, return its name
  bool tryVariable(Parse::Mode mode = Parse::Execute);
    
  // get the name of the next node treated as variable
  QString nextVariable();
  // check whether name is a valid variable is global
  bool isVariable(const QString& name) const;
  // check whether variable/array name is global (preceeded with _)
  bool isGlobal(const QString& name) const;
  // check if next item is a function
  bool isFunction() const;
  // check if next item is a widget
  bool isWidget() const;
   
  // reset to default state
  void reset();
  // set error state if no error was set before; err is expected symbol that wasn't found
  void setError(const QString& msg);
  void setError(const QString& msg, int pos);
  // check whether parsing was successful
  bool isError() const;
  
  // parsing data
  ParserData* m_data;
  // current parsing position
  int m_start;
  // current error message
  QString m_error;
  // in case of error, this keeps position of first error
  int m_errorPosition;
  // parsing nodes
  QVector<ParseNode> m_parts;
  // variables
  QMap<QString, ParseNode> m_variables;
  // arrays
  QMap<QString, QMap<QString, ParseNode> > m_arrays;
  // Kommander 
  KommanderWidget* m_widget;
  // global variables
  static QMap<QString, ParseNode> m_globalVariables;
  // global arrays
  static QMap<QString, QMap<QString, ParseNode> > m_globalArrays;
};

#endif

