/***************************************************************************
                        specialinformation.h - internal commands information
                             -------------------
    copyright            : (C) 2004 by Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
  
#ifndef _HAVE_SPECIALINFORMATION_H_
#define _HAVE_SPECIALINFORMATION_H_

/* QT INCLUDES */
#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qpair.h>

class SpecialFunction
{
public:
 
   /* flags for getting function prototype:
      SkipFirstArgument - ignore first argument (useful for Kommander functions
         prefixed by widget name 
      ShowArgumentNames - show parameter names, not only types 
   */
   enum PrototypeFlags {SkipFirstArgument = 1, ShowArgumentNames = 2, NoSpaces = 4};
   /* Flags describing which parser supports the function */
   enum ParserType {MacroParser = 1, InternalParser = 2, AllParsers = 3};
   SpecialFunction(const QString& function, const QString& description
     = QString::null, int minArgs = -1, int maxArgs = -1);
   SpecialFunction(ParserType p, const QString& function, const QString& description
     = QString::null, int minArgs = -1, int maxArgs = -1);
   SpecialFunction()   {m_minArgs = m_maxArgs = 0;}
   /* minimum number of arguments */
   int minArg() const    {return m_minArgs;}
   /* maximum number of arguments */
   int maxArg() const    {return m_maxArgs;}
   /* checks number of arguments */
   bool isValidArg(int args) const    {return args >= minArg() && args <= maxArg();}
   /* function description */
   QString description() const {return m_description;}
   /* function name */
   QString name() const {return m_function;}
   /* function prototype: with parameter types and optional names */
   QString prototype(uint prototypeFlags = 0) const;
   /* i-th parameter name */
   QString argumentName(uint i) const;
   /* i-th parameter type */
   QString argumentType(uint i) const;
   /* number of named arguments */
   int argumentCount() const;
   /* check whether given parser supports the function */
   bool isSupported(ParserType p) const;
protected:
   QString m_function;
   QString m_description;
   int m_minArgs, m_maxArgs;
   QStringList m_args;
   QStringList m_types;
   unsigned m_parserTypes;
};




class SpecialInformation
{
public:
  SpecialInformation()  {m_defaultGroup = -1;}
  static int function(int group, const QString& fname);
  static SpecialFunction functionObject(const QString& gname, const QString& fname);
  static int group(const QString& gname);
  static bool isValid(int gname, int fname);
  static bool isValid(const QString& gname, const QString& fname);
  static bool isValid(int gname, int fname, SpecialFunction::ParserType p);
  static bool isValid(const QString& gname, const QString& fname, SpecialFunction::ParserType p);
  static int minArg(int gname, int fname);
  static int maxArg(int gname, int fname);
  static int argCount(int gname, int fname);
  static bool isValidArg(int gname, int fname, int args);
  static QString description(int gname, int fname);
  static QString prototype(int gname, int fname, uint prototypeFlags = 0);
  /* Insert function supported by all parsers */
  static bool insert(int id, const QString& function, const QString description = QString::null,
    int minArgs = -1, int maxArgs = -1, SpecialFunction::ParserType = SpecialFunction::AllParsers);
  /* Insert function supported by (old) macro parser */
  static bool insertMacro(int id, const QString& function, const QString description = QString::null,
    int minArgs = -1, int maxArgs = -1);
  /* Insert function supported by (new) internal parser */
  static bool insertInternal(int id, const QString& function, const QString description = QString::null,
    int minArgs = -1, int maxArgs = -1);
  static bool insertAlias(int id, const QString& alias);
  static void insertGroup(int id, const QString& name, const QString& parserName);
  static void setDefaultGroup(int gname);
  static void registerSpecials();
  static QString parserGroupName(const QString&);
  static QStringList groups();
  static QStringList functions(const QString& group);
protected:
  static QMap<int, QMap<int, SpecialFunction> > m_specials;
  static QMap<QString, int> m_groups;
  static QMap<QString, QString> m_parserGroups;
  static QMap<int, QMap<QString, int> > m_functions;
  static QMap<int, QMap<QString, int> > m_aliases;
  static int m_defaultGroup;
};


#endif

