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
#include "kommander_export.h"

class QWidget;
class QIconSet;

class KOMMANDER_EXPORT KommanderPlugin : public QObject 
{
    Q_OBJECT
    public:
    KommanderPlugin();
    ~KommanderPlugin();

    virtual void addWidget( const QString &name, const QString &group, const QString &toolTip, QIconSet *iconSet, const QString &whatsThis = QString::null, bool isContainer = false);
    virtual void removeWidget( const QString &name );
    virtual QStringList widgets() const;

    virtual QWidget *create( const QString &key, QWidget *parent = 0, const char *name = 0 ) = 0;
    virtual QString group( const QString &key ) const;
    virtual QString toolTip( const QString &key ) const;
    virtual QString whatsThis( const QString &key ) const;
    virtual bool isContainer( const QString &key ) const;
    virtual QIconSet *iconSet( const QString &key ) const;

    /**
     * Sets the default group for functions. Must be called before registerFunction.
     * @param group the groups ID
     */
    static void setDefaultGroup(int group);

    /**
     * Register a function of the plugin.
     * @param id Kommander wide unique ID
     * @param function function signature
     * @param description description of what the function does
     * @param minArgs minimum number of accepted arguments
     * @param maxArgs maximum number of accepted arguments
     * @return true if registration was successful
     */
    static bool registerFunction(int id, const QString& function, const QString description = QString::null,
    int minArgs = -1, int maxArgs = -1);

    

private:
    struct WidgetInfo
    {
      WidgetInfo() { }
      WidgetInfo( const QString &g, const QString &t, QIconSet *i, const QString &w = QString::null, bool c = false)
          : group( g ), toolTip( t ), iconSet(i), whatsThis( w ), isContainer( c )
      {}

      QString group;
      QString toolTip;
      QIconSet *iconSet;
      QString whatsThis;
      bool isContainer;
    };
    typedef QMap<QString, WidgetInfo> WidgetInfos;
    WidgetInfos m_widgets;
};

#define KOMMANDER_EXPORT_PLUGIN(plugin) extern "C" { KOMMANDER_EXPORT void *kommander_plugin() { return new plugin; } }

#endif // _HAVE_KOMMANDERPLUGIN_H_
