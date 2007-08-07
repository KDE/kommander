/***************************************************************************
  mainwindow.cpp - Kommander plugin manager mainwindow class implementation
                             -------------------
    begin                : Tue Aug 13 09:31:50 EST 2002
    copyright            : (C) 2004 by Marc Britton
    email                : consume@optushome.com.au
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "mainwindow.h"
#include "pluginmanager.h"

#include <ktoolbar.h>
//#include <k3listbox.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kstandarddirs.h>

MainWindow::MainWindow( QWidget* parent, const char *name, Qt::WFlags f )
    : KXmlGuiWindow( parent, f )
{
  setObjectName(name);
  KToolBar *toolBar = new KToolBar( this );
  //toolBar->insertButton("document-open", Add, true, i18n("Add") );
  //toolBar->insertButton("no", Remove, true, i18n("Remove") );
  //toolBar->insertButton("view-refresh", Refresh, true, i18n("Refresh") );
  connect( toolBar, SIGNAL(clicked(int)), this, SLOT(toolButton(int)) );

  //m_list = new K3ListBox( this );
  //setCentralWidget(m_list);
  
  m_pluginManager = new PluginManager;
  //m_list->insertStringList(m_pluginManager->items());
}

MainWindow::~MainWindow()
{
  delete m_pluginManager;
}

void MainWindow::toolButton( int id )
{
  switch (id)
  {
    case Add: 
      add();
      break;
    case Remove:
      remove();
      break;
    case Refresh:
      verify();
      break;
  }
}

void MainWindow::add()
{
  
}

void MainWindow::add(const QString &plugin)
{
  if (!m_pluginManager->add(plugin))
  {
    QString errMsg = i18n("<qt>Unable to load Kommander plugin<br><b>%1</b></qt>", plugin);
    KMessageBox::error(this, errMsg, i18n("Cannot add plugin"));
  }
  else
    refresh();
}

void MainWindow::remove()
{
  /*QString plugin = m_list->currentText();
  if (m_pluginManager->remove(plugin))
    refresh();
  */
}

void MainWindow::refresh()
{
  //m_list->clear();
  //m_list->insertStringList(m_pluginManager->items()); 
}

void MainWindow::verify()
{
  m_pluginManager->verify();
  refresh();
}

#include "mainwindow.moc"
