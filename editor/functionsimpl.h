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
#include <qwidget.h>

#include "functions.h"


class FunctionsDialog : public FunctionsDialogBase
{
  Q_OBJECT
public:
  FunctionsDialog(QWidget*, char* = 0, bool = true);
  ~FunctionsDialog();
  QString functionText() const;
  QString currentFunctionText();
public slots:
  void groupChanged(int);
  void functionChanged(int);
  void copyText();
};  


#endif

