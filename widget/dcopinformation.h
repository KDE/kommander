/***************************************************************************
                          dcopinformation.h - internal DCOP functions information
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

 
#ifndef _HAVE_DCOPINFORMATION_H_
#define _HAVE_DCOPINFORMATION_H_

/* QT INCLUDES */
#include <qstring.h>
#include <qmap.h>


class DCOPFunction
{
public:
   DCOPFunction(const QString& prototype, const QString& description = QString::null);
   DCOPFunction()   {}
   int arguments() const       {return m_args;}
   QString function() const    {return m_function;}
   QString prototype() const   {return m_prototype;}
   QString description() const {return m_description;}
protected:
   QString m_function;
   QString m_prototype;
   QString m_description;
   int m_args;
};




class DCOPInformation
{
public:
  static QString prototype(const QString& name);
  static int arguments(const QString& name);
  static QString function(const QString& name);
  static QString description(const QString& name);
  static void insert(const DCOPFunction& function);
  static void insert(const QString& function, const QString description = QString::null);
protected:
  static QMap<QString, DCOPFunction> m_prototypes;
};


#endif

