/***************************************************************************
                      kommanderwindow.h  -  Kommander window widgets
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

#ifndef _HAVE_KOMMANDERWINDOW_H_
#define _HAVE_KOMMANDERWINDOW_H_

#include "kommanderwidget.h"
#include "kommandercore_export.h"
class KOMMANDERCORE_EXPORT KommanderWindow : public KommanderWidget
{
public:
  KommanderWindow(QObject *);
  virtual ~KommanderWindow();
  virtual void setFileName(const QString& s);
  virtual QString fileName();
protected:
  
private:
  QString m_fileName;
};



#endif

