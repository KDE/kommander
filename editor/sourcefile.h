/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef SOURCEFILE_H
#define SOURCEFILE_H

#include <qobject.h>
#include "timestamp.h"

struct DesignerSourceFile;
class SourceEditor;
#ifndef KOMMANDER
class Project;
#endif

class SourceFile : public QObject
{
    Q_OBJECT

public:
#ifndef KOMMANDER
	    SourceFile( const QString &fn, bool temp, Project *p );
#else
	    SourceFile( const QString &fn, bool temp );
#endif
    ~SourceFile();

    void setText( const QString &s );
    void setModified( bool m );

    bool save();
    bool saveAs();
    bool load();
    bool close();
    bool closeEvent();
#ifndef KOMMANDER
    Project *project() const;
#endif

    QString text() const;
    QString fileName() const { return filename; }
    bool isModified() const;

    void checkTimeStamp();

    DesignerSourceFile *iFace();

    void setEditor( SourceEditor *e );
    SourceEditor *editor() const { return ed; }

    static QString createUnnamedFileName( const QString &extension );

private:
    bool checkFileName( bool allowBreak );

private:
    QString filename;
    QString txt;
    DesignerSourceFile *iface;
    SourceEditor *ed;
    bool fileNameTemp;
    TimeStamp timeStamp;
#ifndef KOMMANDER
    Project *pro;
#endif

};

#endif
