/****************************************************************************
  ** kommanderplugin.h - KommanderPlugin class definition created from QWidgetPlugin
  ** Copyright (C) 2001 Trolltech AS.  All rights reserved.
  ** Copyright (C) 2003 Marc Britton
  **
  ** This file is part of the widgets module of the Qt GUI Toolkit.
  **
  ** This file may be distributed under the terms of the Q Public License
  ** as defined by Trolltech AS of Norway and appearing in the file
  ** LICENSE.QPL included in the packaging of this file.
  **
  ** This file may be distributed and/or modified under the terms of the
  ** GNU General Public License version 2 as published by the Free Software
  ** Foundation and appearing in the file LICENSE.GPL included in the
  ** packaging of this file.
  **
  ** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
  ** licenses may use this file in accordance with the Qt Commercial License
  ** Agreement provided with the Software.
  **
  ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
  ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
  **
  ** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
  **   information about Qt Commercial License Agreements.
  ** See http://www.trolltech.com/qpl/ for QPL licensing information.
  ** See http://www.trolltech.com/gpl/ for GPL licensing information.
  **
  ** Contact info@trolltech.com if any conditions of this licensing are
  ** not clear to you.
  **
  **********************************************************************/

#ifndef _HAVE_KOMMANDERPLUGIN_H_
#define _HAVE_KOMMANDERPLUGIN_H_

#include <qobject.h>
#include <qmap.h>
#include <qstringlist.h>


class QWidget;

class KommanderPlugin : public QObject 
{
    Q_OBJECT
    public:
    KommanderPlugin();
    ~KommanderPlugin();

    virtual void addWidget( const QString &name, const QString &group, const QString &toolTip, const QString &whatsThis = QString::null, bool isContainer = FALSE );
    virtual void removeWidget( const QString &name );
    virtual QStringList widgets() const;

    virtual QWidget *create( const QString &key, QWidget *parent = 0, const char *name = 0 ) = 0;
    virtual QString group( const QString &key ) const;
    virtual QString toolTip( const QString &key ) const;
    virtual QString whatsThis( const QString &key ) const;
    virtual bool isContainer( const QString &key ) const;
private:
    struct WidgetInfo
    {
	WidgetInfo() { }
	WidgetInfo( const QString &g, const QString &t, const QString &w = QString::null, bool c = FALSE )
	    : group( g ), toolTip( t ), whatsThis( w ), isContainer( c )
	{
	}
	QString group;
	QString toolTip;
	QString whatsThis;
	bool isContainer;
    };
    typedef QMap<QString, WidgetInfo> WidgetInfos;
    WidgetInfos m_widgets;
};

#define KOMMANDER_EXPORT_PLUGIN(plugin) extern "C" { void *kommander_plugin() { return new plugin; } }

#endif // _HAVE_KOMMANDERPLUGIN_H_
