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
#include <qvaluevector.h>
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
  // execute whole block; return true if ok
  bool parse(Parse::Mode mode = Parse::Execute);
  // return line of errorneous node
  int errorLine();
  // return error message
  QString errorMessage();
  
  // set variable value
  void setVariable(const QString& name, int value);
  // set variable value
  void setVariable(const QString& name, double value);
  // set variable value
  void setVariable(const QString& name, const QString& value);
  // get variable value
  QString variable(const QString& name);
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
  ParseNode next();
  // check if next item is keyword k, if so - go further, if no, set error
  bool tryKeyword(Parse::Keyword k, Parse::Mode mode = Parse::Execute);
  // check if next item is a variable, if so, return its name
  bool tryVariable(Parse::Mode mode = Parse::Execute);
    
  // get the name of the next node treated as variable
  QString nextVariable();
  // check if next item is a function
  bool isFunction();
  // check if next item is a widget
  bool isWidget();
   
  // reset to default state
  void reset();
  // set error state if no error was set before; err is expected symbol that wasn't found
  void setError(Parse::Keyword err);
  
  // parsing data
  ParserData* m_data;
  // current parsing position
  uint m_start;
  // current parser state
  Parse::Keyword m_error;
  // in case of error, this keeps position of first error
  uint m_errorPosition;
  // parsing nodes
  QValueVector<ParseNode> m_parts;
  // variables
  QMap<QString, ParseNode> m_variables;
  // arrays
  QMap<QString, QMap<QString, ParseNode> > m_arrays;
  // Kommander 
  KommanderWidget* m_widget;
};

#endif

