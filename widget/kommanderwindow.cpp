/***************************************************************************
                      kommanderwindow.cpp  -  Kommander window widgets
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

#include "kommanderwindow.h"
 
KommanderWindow::KommanderWindow(QObject* object) : KommanderWidget(object)
{
}

KommanderWindow::~KommanderWindow()
{
}


void KommanderWindow::setFileName(const QString& s)
{
  m_fileName = s;
}

QString KommanderWindow::fileName()
{
  return m_fileName; 
}