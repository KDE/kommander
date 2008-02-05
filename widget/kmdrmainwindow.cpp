//
// C++ Implementation: KmdrMainWindow
//
// Description: 
//
//
// Author: Andras Mantia <amantia@kdewebdev.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "kmdrmainwindow.h"

#include <qtimer.h>

KmdrMainWindow::KmdrMainWindow(QWidget *parent, const char *name, WFlags f)
 : KMainWindow(parent, name, f)
{
  QTimer::singleShot(10, this, SIGNAL(initialize()));
}


KmdrMainWindow::~KmdrMainWindow()
{
}

bool KmdrMainWindow::queryClose()
{
  if (KMainWindow::queryClose())
    emit destroy();
}


#include "kmdrmainwindow.moc"
