/***************************************************************************
                    parserdata.h - Parser data: keywords, functions etc.
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

#ifndef _HAVE_PARSERDATA_H_
#define _HAVE_PARSERDATA_H_

#include "parsenode.h"
#include "function.h"
#include <qmap.h>

class ParserData {
public:
  /* initialize keywords */
  ParserData();
  /* Return group of given keyword */
  Parse::KeywordGroup keywordGroup(Parse::Keyword k) const;
  /* Convert string to keyword */
  Parse::Keyword stringToKeyword(const QString& s) const;
  /* Convert keyword to string */
  QString keywordToString(Parse::Keyword k) const;
  /* register a function */
  bool registerFunction(const QString& name, Function f);
  /* check if this is a name of standard function */
  bool isFunction(const QString& name) const;
  /* Return function with given name. Warning: this function has undefined behavior when there is
    no such function. */
  const Function& function(const QString& name) const;
private:
  // register standard function
  void registerStandardFunctions();
  QMap<QString, Parse::Keyword> m_keywords;
  QMap<Parse::Keyword, Parse::KeywordGroup> m_groups;
  QMap<QString, Function> m_functions;
}; 


#endif
