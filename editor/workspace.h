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

#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <qlistview.h>

class FormWindow;
class QResizeEvent;
class QCloseEvent;
class QDropEvent;
class QDragMoveEvent;
class QDragEnterEvent;
class MainWindow;
#ifndef KOMMANDER
class Project;
class SourceFile;
#endif
class FormFile;
class QCompletionEdit;
#ifndef KOMMANDER
    class SourceEditor;
#endif

class WorkspaceItem : public QListViewItem
{
public:
    enum Type { ProjectType, FormFileType, FormSourceType, SourceFileType };

#ifndef KOMMANDER
    //WorkspaceItem( QListView *parent, Project* p );
#else
    WorkspaceItem(QListView *parent);
#endif
#ifndef KOMMANDER
//    WorkspaceItem( QListViewItem *parent, SourceFile* sf );
#endif
    WorkspaceItem( QListViewItem *parent, FormFile* ff, Type t = FormFileType );
    WorkspaceItem( QListView *parent, FormFile* ff, Type t = FormFileType );

    void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align );

    Type type() const { return t; }

    bool isModified() const;

    QString text( int ) const;

    void fillCompletionList( QStringList& completion );
    bool checkCompletion( const QString& completion );

    QString key( int, bool ) const; // column sorting key

#ifndef KOMMANDER
    Project* project;
#endif
#ifndef KOMMANDER
    SourceFile* sourceFile;
#endif
    FormFile* formFile;

    void setOpen( bool );

    void setAutoOpen( bool );
    bool isAutoOpen() const { return isOpen() && autoOpen; }

    bool useOddColor;

private:
    void init();
    bool autoOpen;
    QColor backgroundColor();
    Type t;
};

class Workspace : public QListView
{
    Q_OBJECT

public:
    Workspace( QWidget *parent , MainWindow *mw );

#ifndef KOMMANDER
    void setCurrentProject( Project *pro );
#endif
#ifdef KOMMANDER
	void makeConnections(MainWindow *);
#endif
    void contentsDropEvent( QDropEvent *e );
    void contentsDragEnterEvent( QDragEnterEvent *e );
    void contentsDragMoveEvent( QDragMoveEvent *e );

    void setBufferEdit( QCompletionEdit *edit );

public slots:

    void update();
    void update( FormFile* );

    void activeFormChanged( FormWindow *fw );
#ifndef KOMMANDER
//    void activeEditorChanged( SourceEditor *se );
#endif

protected:
    void closeEvent( QCloseEvent *e );
    bool eventFilter( QObject *, QEvent * );


private slots:
    void itemClicked( int, QListViewItem *i, const QPoint& pos  );
    void itemDoubleClicked( QListViewItem *i );
    void rmbClicked( QListViewItem *i, const QPoint& pos  );
    void bufferChosen( const QString &buffer );

#ifndef KOMMANDER
//    void projectDestroyed( QObject* );
#endif

#ifndef KOMMANDER
//    void sourceFileAdded( SourceFile* );
 //   void sourceFileRemoved( SourceFile* );
#endif

    void formFileAdded( FormFile* );
    void formFileRemoved( FormFile* );

private:
    WorkspaceItem *findItem( FormFile *ff );
#ifndef KOMMANDER
    WorkspaceItem *findItem( SourceFile *sf );
#endif

    void closeAutoOpenItems();

private:
    MainWindow *mainWindow;
#ifndef KOMMANDER
    Project *project;
#endif
#ifndef KOMMANDER
    WorkspaceItem *projectItem;
#endif
    QCompletionEdit *bufferEdit;
    bool blockNewForms;
    void updateBufferEdit();
    bool completionDirty;
    void updateColors();

};

#endif
