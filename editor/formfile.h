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
class SourceEditor;

class FormFile : public QObject
{
    Q_OBJECT
    friend class SourceEditor;

public:
    enum Who {
	WFormWindow = 1,
	WFormCode = 2,
	WAnyOrAll = WFormWindow | WFormCode
    };
#ifndef KOMMANDER
    FormFile( const QString &fn, bool temp, Project *p );
#else
    FormFile(const QString &fn, bool temp);
#endif
    ~FormFile();

    void setFormWindow( FormWindow *f );
    void setEditor( SourceEditor *e );
    void setFileName( const QString &fn );
    void setCode( const QString &c );
    void setModified( bool m, int who = WAnyOrAll );
    void setCodeEdited( bool b );

    FormWindow *formWindow() const;
    SourceEditor *editor() const;
    QString fileName() const;
    QString absFileName() const;

    bool supportsCodeFile() const { return !codeExtension().isEmpty(); }
    QString codeFile() const;
    QString code();
    bool isCodeEdited() const;

    bool loadCode();
    bool save( bool withMsgBox = TRUE );
    bool saveAs();
    bool close();
    bool closeEvent();
    bool isModified( int who = WAnyOrAll );
    bool hasFormCode() const;
    void createFormCode();
    void syncCode();
    void checkTimeStamp();
    void addSlotCode( MetaDataBase::Slot slot );
    void functionNameChanged( const QString &oldName, const QString &newName );

    void showFormWindow();
    SourceEditor *showEditor();

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
    bool isCodeModified() const;
    void setFormWindowModified( bool m );
    void setCodeModified( bool m );
    QString codeExtension() const;
    void parseCode( const QString &txt, bool allowModify );
#ifndef KOMMANDER
    bool checkFileName( bool allowBreak );
#endif

private:
    QString filename;
    bool fileNameTemp;
#ifndef KOMMANDER
    Project *pro;
#endif
    FormWindow *fw;
    SourceEditor *ed;
    QString cod;
    TimeStamp timeStamp;
    bool codeEdited;
    bool seperateSource;
    QString cachedFormName;
    bool cm;

};

#endif
