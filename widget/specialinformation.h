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
#include <qmap.h>
#include <qpair.h>

class SpecialFunction
{
public:
   SpecialFunction(const QString& function, int minArgs = 0, int maxArgs = 0, const QString& description
     = QString::null);
   SpecialFunction()   {m_args = QPair<int, int>(0,0);}
   int minArg() const    {return m_args.first;}
   int maxArg() const    {return m_args.second;}
   bool validArg(int args) const    {return args >= m_args.first && args <= m_args.second;}
   QString description() const {return m_description;}
   QString function() const {return m_function;}
protected:
   QString m_function;
   QString m_description;
   QPair<int, int> m_args;
};




class SpecialInformation
{
public:
  static bool isValid(const QString& function, const QString& objectName = QString::null);
  static bool isObject(const QString& objectName);
  static int minArg(const QString& function, const QString& objectName = QString::null);
  static int maxArg(const QString& function, const QString& objectName = QString::null);
  static bool validArg(const QString& name, int arg, const QString& objectName = QString::null);
  static QString description(const QString& name, const QString& objectName = QString::null);
  static void insert(const SpecialFunction& function);
  static void insert(const QString& function, int minArgs = 0, int maxArgs = 0,
    const QString description = QString::null);
  static void setCurrentObject(const QString& s)  {m_currentObject = s;}
protected:
  static QMap<QString, QMap<QString, SpecialFunction> > m_specials;
  static QString m_currentObject;
};


#endif

