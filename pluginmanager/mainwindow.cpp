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

#include <ktoolbar.h>
#include <klistbox.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <stdlib.h>

MainWindow::MainWindow( QWidget* parent, const char *name, WFlags f )
    : KMainWindow( parent, name, f )
{
    KToolBar *toolBar = new KToolBar( this );
    toolBar->insertButton( "fileopen", 0, true, i18n("Add") );
    toolBar->insertButton( "no", 1, true, i18n("Remove") );
    connect( toolBar, SIGNAL(clicked(int)), this, SLOT(toolButton(int)) );

    m_list = new KListBox( this );
    setCentralWidget( m_list );
    m_cfg = new KConfig( "kommanderrc" );

    QStringList list = m_cfg->readListEntry( "plugins");
    for( QStringList::Iterator it = list.begin() ; it != list.end() ; ++it )
	add( *it );
}

MainWindow::~MainWindow()
{
    QStringList plugins;
    for( int i = 0 ; i < m_list->count() ; ++i )
    {
	QFileInfo fi( m_list->item( i )->text() );
	plugins += fi.baseName();
    }
    m_cfg->writeEntry( "plugins", plugins );
    delete m_cfg;
}

void MainWindow::toolButton( int id )
{
    if( id == 0 )
	add();
    else if( id == 1 )
	remove();
}

void MainWindow::add()
{
    QString libDir = getenv("KDEDIR");
    if( !libDir.isEmpty() )
	libDir += "/lib/";
    QString plugin = KFileDialog::getOpenFileName( libDir, "lib*", this, 
					    i18n("Add Kommander plugin") );
    add( plugin );
}

void MainWindow::add( const QString &plugin )
{
    if( !plugin.isNull() )
    {
	/* 
	   WHERE
	   if already have the library in the list, don't add it again
	   when loading plugin basenames from config file, expand them
	   to full library paths with kstandarddirs
       */
	QString errMsg;
	bool alreadyHaveIt = FALSE;
	KLibLoader *f = KLibLoader::self();
	KLibrary *l = f->library( plugin.latin1() );
	if( !l )
	    errMsg = i18n("Unable to load Kommander plugin %1").arg(plugin);
	else if( !l->hasSymbol("kommander_plugin") )
	    errMsg = i18n("Library %1 isn't a Kommander plugin").arg(plugin);
	else
	{
	    for( int i = 0 ; i < m_list->count() ; ++i )
		if( m_list->item( i )->text() == l->fileName() )
		    alreadyHaveIt = TRUE;
	}

	if( !errMsg.isNull() )
	    KMessageBox::error( this, errMsg, i18n("Can't add plugin") );
	else if( !alreadyHaveIt )
	    m_list->insertItem( l->fileName() );
    }
}

void MainWindow::remove()
{
    int ci = m_list->currentItem();
    if( ci == -1 )
	return;
    m_list->removeItem( ci );
    if( m_list->count() )
    {
	int ni = (ci > 0 ? ci-1 : 0);
	m_list->setCurrentItem( ni );
    }
}
