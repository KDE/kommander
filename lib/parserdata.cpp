/***************************************************************************
                    parserdata.cpp - Parser data: keywords, functions etc.
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

#include "parserdata.h"
using namespace Parse;

KeywordGroup ParserData::keywordGroup(Keyword k) const
{
  if (m_groups.contains(k))
    return m_groups[k];
  else
    return GroupMisc;
}

Keyword ParserData::stringToKeyword(const QString& s) const
{
  QString k = s.toLower();
  if (m_keywords.contains(k))
    return m_keywords[k];
  else
    return Variable;
}

QString ParserData::keywordToString(Parse::Keyword k) const
{
  for (QMap<QString, Keyword>::const_iterator it = m_keywords.constBegin(); it != m_keywords.constEnd(); ++it)
    if (it.value() == k)
      return it.key();
  return QString();
}

bool ParserData::registerFunction(const QString& name, Function f)
{
  m_functions[name.toLower()] = f;
  return true;
}

ParserData::ParserData()
{
  m_keywords["for"] = For;
  m_keywords["foreach"] = Foreach;
  m_keywords["in"] = In;
  m_keywords["end"] = End;
  m_keywords["if"] = If;
  m_keywords["then"] = Then;
  m_keywords["else"] =  Else;
  m_keywords["elseif"] =  Elseif;
  m_keywords["endif"] =  Endif;
  m_keywords["{"] = LeftCurlyBrace;
  m_keywords["}"] =  RightCurlyBrace;
  m_keywords["switch"] =  Switch;
  m_keywords["case"] =  Case;
  m_keywords["while"] =  While;
  m_keywords["to"] = To;
  m_keywords["step"] = Step;
  m_keywords["do"] = Do;
  m_keywords["break"] = Break;
  m_keywords["continue"] = Continue;
  m_keywords["exit"] = Exit;
  m_keywords["."] = Dot;
  m_keywords[";"] = Semicolon;
  m_keywords[","] = Comma;
  m_keywords["="] = Assign;
  m_keywords["<"] = Less;
  m_keywords["<="] =  LessEqual;
  m_keywords[">"] =  Greater;
  m_keywords[">="] =  GreaterEqual;
  m_keywords["=="] =  Equal;
  m_keywords["!="] =  NotEqual;
  m_keywords["<>"] =  NotEqual;
  m_keywords["not"] =  Not;
  m_keywords["!"] =  Not;
  m_keywords["and"] =  And;
  m_keywords["&&"] =  And;
  m_keywords["or"] =  Or;
  m_keywords["||"] =  Or;
  m_keywords["false"] = False;
  m_keywords["true"] = True;
  m_keywords["("] =  LeftParenthesis;
  m_keywords[")"] =  RightParenthesis;
  m_keywords["["] =  LeftBracket;
  m_keywords["]"] =  RightBracket;
  m_keywords["+"] = Plus;
  m_keywords["-"] = Minus;
  m_keywords["*"] = Multiply;
  m_keywords["/"] = Divide;
  m_keywords["%"] = Mod;
  m_keywords["+="] = PlusEqual;
  m_keywords["mod"] = Mod;

  m_groups[Less] = GroupComparison;
  m_groups[LessEqual] = GroupComparison;
  m_groups[Equal] = GroupComparison;
  m_groups[NotEqual] = GroupComparison;
  m_groups[Greater] = GroupComparison;
  m_groups[GreaterEqual] = GroupComparison;

  m_groups[Plus] = GroupAdd;
  m_groups[Minus] = GroupAdd;
  m_groups[Multiply] = GroupMultiply;
  m_groups[Divide] = GroupMultiply;
  m_groups[Mod] = GroupMultiply;

  registerStandardFunctions();
}

bool ParserData::isFunction(const QString& name) const
{
  return m_functions.contains(name.toLower());
}

const Function& ParserData::function(const QString& name)
{
  return m_functions[name.toLower()];
}

