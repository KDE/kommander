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

#ifndef FORMFILE_H
#define FORMFILE_H

#include <qobject.h>
#include "timestamp.h"
#include "metadatabase.h"

#ifndef KOMMANDER
class Project;
#endif
class FormWindow;
#ifndef KOMMANDER
class SourceEditor;
#endif

class FormFile : public QObject
{
    Q_OBJECT
#ifndef KOMMANDER
    friend class SourceEditor;
#endif

public:
#ifndef KOMMANDER
    enum Who {
	WFormWindow = 1,
	WFormCode = 2,
	WAnyOrAll = WFormWindow | WFormCode
    };
#endif
#ifndef KOMMANDER
    FormFile( const QString &fn, bool temp, Project *p );
#else
    FormFile(const QString &fn, bool temp);
#endif
    ~FormFile();

    void setFormWindow( FormWindow *f );
#ifndef KOMMANDER
    void setEditor( SourceEditor *e );
#endif
    void setFileName( const QString &fn );
#ifndef KOMMANDER
    void setCode( const QString &c );
    void setCodeEdited( bool b );
    void setModified( bool m, int who = WAnyOrAll );
#else
    void setModified(bool m);
#endif

    FormWindow *formWindow() const;
#ifndef KOMMANDER
    SourceEditor *editor() const;
#endif
    QString fileName() const;
    QString absFileName() const;

#ifndef KOMMANDER
    bool supportsCodeFile() const { return !codeExtension().isEmpty(); }
    QString codeFile() const;
    QString code();
    bool isCodeEdited() const;

    bool loadCode();
#endif
    bool save( bool withMsgBox = TRUE );
    bool saveAs();
    bool close();
    bool closeEvent();
#ifndef KOMMANDER
    bool isModified( int who = WAnyOrAll );
#else
    bool isModified();
#endif
#ifndef KOMMANDER
    bool hasFormCode() const;
    void createFormCode();
    void syncCode();
    void addSlotCode( MetaDataBase::Slot slot );
    void functionNameChanged( const QString &oldName, const QString &newName );
    SourceEditor *showEditor();
    void checkTimeStamp();
#endif

    void showFormWindow();

    static QString createUnnamedFileName();
    QString formName() const;

    bool hasTempFileName() const { return fileNameTemp; }

signals:
	void addedFormFile(FormFile *);
	void removedFormFile(FormFile *);
    void somethingChanged( FormFile* );

private slots:
    void formWindowChangedSomehow();

private:
    bool isFormWindowModified() const;
#ifndef KOMMANDER
    bool isCodeModified() const;
#endif
    void setFormWindowModified( bool m );
#ifndef KOMMANDER
    void setCodeModified( bool m );
    QString codeExtension() const;
    void parseCode( const QString &txt, bool allowModify );
#endif
#ifndef KOMMANDER
    bool checkFileName( bool allowBreak );
#endif

private:
    QString filename;
    bool fileNameTemp;
#ifndef KOMMANDER
    Project *pro;
    SourceEditor *ed;
    bool codeEdited;
    QString cod;
    bool seperateSource;
    TimeStamp timeStamp;
    bool cm;
#endif
    FormWindow *fw;
    QString cachedFormName;

};

#endif
