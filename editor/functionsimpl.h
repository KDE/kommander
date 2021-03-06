/***************************************************************************
                          functionsimpl.h - Function browser implementation 
                             -------------------
    copyright            : (C) 2004    Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _HAVE_FUNCTIONSIMPL_H_
#define _HAVE_FUNCTIONSIMPL_H_

#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>
#include <q3dict.h>

#include "functions.h"
#include "specials.h"

class FunctionsDialog : public FunctionsDialogBase
{
  Q_OBJECT
public:
  FunctionsDialog(QWidget*, const Q3Dict<QWidget>&, char* = 0, bool = true);
  ~FunctionsDialog();
  // Return current content of text box
  QString functionText() const;
  // Return current function and parameters
  QString currentFunctionText();
public slots:
  // Update list of function availabe for given group
  void groupChanged(int);
  // Update syntax of chosen function
  void functionChanged(int);
  // Copy current function and parameters into text box
  void copyText();
  // Show widgets for current function's parameters
  void showParameters();
private:
  // Calculate current parameters
  QString params();
  SpecialFunction m_function;
  int m_DCOP;
  const Q3Dict<QWidget> m_widgetList;
  const QStringList m_widgetNames;
};  


#endif

