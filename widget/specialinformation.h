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
   SpecialFunction(const QString& function, int minArgs = 0, const QString& description
     = QString::null);
   SpecialFunction()   {m_minArgs = 0;}
   int minArg() const    {return m_minArgs;}
   int maxArg() const    {return m_types.count();}
   bool isValidArg(int args) const    {return args >= minArg() && args <= maxArg();}
   QString description() const {return m_description;}
   QString name() const {return m_function;}
   QString prototype() const;
   QString argumentName(uint i) const;
   QString argumentType(uint i) const;
protected:
   QString m_function;
   QString m_description;
   int m_minArgs;
   QStringList m_args;
   QStringList m_types;
};




class SpecialInformation
{
public:
  SpecialInformation()  {m_defaultGroup = -1;}
  static int function(int group, const QString& fname);
  static int group(const QString& gname);
  static bool isValid(int gname, int fname);
  static bool isValid(const QString& gname, const QString& fname);
  static int minArg(int gname, int fname);
  static int maxArg(int gname, int fname);
  static bool isValidArg(int gname, int fname, int args);
  static QString description(int gname, int fname);
  static QString prototype(int gname, int fname);
  static bool insert(int id, const QString& function, int minArgs = 0, 
    const QString description = QString::null);
  static bool insertAlias(int id, const QString& alias);
  static void insertGroup(int id, const QString& name);
  static void setDefaultGroup(int gname);
  static void registerSpecials();
protected:
  static QMap<int, QMap<int, SpecialFunction> > m_specials;
  static QMap<QString, int> m_groups;
  static QMap<int, QMap<QString, int> > m_functions;
  static int m_defaultGroup;
};


#endif

