/***************************************************************************
	  kommanderplugin.cpp - An interface to Kommander widget plugins
                             -------------------
    copyright            : (C) 2003 by Marc Britton
    email                : consume@optusnet.com.au
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kommanderplugin.h"

KommanderPlugin::KommanderPlugin()
{
}

KommanderPlugin::~KommanderPlugin()
{
}

void KommanderPlugin::addWidget( const QString &name, const QString &group, const QString &toolTip, const QString &whatsThis, bool isContainer )
{
    m_widgets[name] = WidgetInfo( group, toolTip, whatsThis, isContainer );
}

void KommanderPlugin::removeWidget( const QString &name )
{
    m_widgets.remove( name );
}

QStringList KommanderPlugin::widgets() const
{
    QStringList ws;
    WidgetInfos::ConstIterator it;
    for( it = m_widgets.begin() ; it != m_widgets.end() ; ++ it )
	ws += it.key();
    return ws;
}

QString KommanderPlugin::group( const QString &name ) const
{
    return m_widgets[name].group;
}

QString KommanderPlugin::toolTip( const QString &name ) const
{
    return m_widgets[name].toolTip;
}

QString KommanderPlugin::whatsThis( const QString &name ) const
{
    return m_widgets[name].whatsThis;
}

bool KommanderPlugin::isContainer( const QString &name ) const
{
    return m_widgets[name].isContainer;
}

#include "kommanderplugin.moc"
