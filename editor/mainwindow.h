/**********************************************************************
 This file is based on Qt Designer, Copyright (C) 2000 Trolltech AS.  All rights reserved.

 This file may be distributed and/or modified under the terms of the
 GNU General Public License version 2 as published by the Free Software
 Foundation and appearing in the file LICENSE.GPL included in the
 packaging of this file.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

 See http://www.trolltech.com/gpl/ for GPL licensing information.

 Modified for Kommander:
  (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
  (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>

**********************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "metadatabase.h"

#include <kmainwindow.h>
#include <kdockwidget.h>

#include <qmap.h>
#include <qguardedptr.h>
#include <qobjectlist.h>

class ActionEditor;
class AssistProc;
class FindDialog;
class FormFile;
class FormWindow;
class GotoLineDialog;
class HierarchyView;
class KAction;
class KActionCollection;
class KProcess;
class KRecentFilesAction;
class KToggleAction;
class KURL;
class PropertyEditor;
class QCloseEvent;
class QMenuBar;
class QTimer;
class QWidget;
class QWorkspace;
class MessageLog;
class ReplaceDialog;
class Workspace;

#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
#include <qtoolbar.h>
#else
class QToolBar;
#endif
class Preferences;


class MainWindow : public KDockMainWindow
{
    Q_OBJECT

public:
    MainWindow( bool asClient );
    ~MainWindow();

    HierarchyView *objectHierarchy() const;
    Workspace *workspace() const;
    PropertyEditor *propertyeditor() const;
    ActionEditor *actioneditor() const;

    void resetTool();
    int currentTool() const;

    FormWindow *formWindow();

    bool unregisterClient( FormWindow *w );
    QWidget *isAFormWindowChild( QObject *o ) const;
    QWidget *isAToolBarChild( QObject *o ) const;

    void insertFormWindow( FormWindow *fw );
    QWorkspace *qWorkspace() const;

    void popupFormWindowMenu( const QPoint &gp, FormWindow *fw );
    void popupWidgetMenu( const QPoint &gp, FormWindow *fw, QWidget *w );

    QPopupMenu *setupNormalHierarchyMenu( QWidget *parent );
    QPopupMenu *setupTabWidgetHierarchyMenu( QWidget *parent, const char *addSlot, const char *removeSlot );

    FormWindow *openFormWindow( const QString &fn, bool validFileName = TRUE, FormFile *ff = 0 );
    
    void setGrid( const QPoint &p );
    void setShowGrid( bool b );
    void setSnapGrid( bool b );
    QPoint grid() const { return grd; }
    bool showGrid() const { return sGrid; }
    bool snapGrid() const { return snGrid && sGrid; }

    QString documentationPath() const;

    static MainWindow *self;
    void saveAllTemp();

    QString templatePath() const { return templPath; }

    bool isPreviewing() const { return previewing; }

    FormWindow *activeForm() const { return lastActiveFormWindow; }

    void setModified( bool b, QWidget *window );
    void updateWorkspace();

    void formNameChanged( FormWindow *fw );

    int currentLayoutDefaultSpacing() const;
    int currentLayoutDefaultMargin() const;


public slots:
    void showProperties( QObject *w );
    void updateProperties( QObject *w );
    void showDialogHelp();

signals:
    void addedFormFile(FormFile *);
    void removedFormFile(FormFile *);
    void currentToolChanged();
    void hasActiveForm( bool );
    void hasActiveWindow( bool );

    void formModified( bool );
    void formWindowsChanged();
    void formWindowChanged();

protected:
    bool eventFilter( QObject *o, QEvent *e );
    void closeEvent( QCloseEvent *e );

public slots:
    void fileNew();
    void fileClose();
    void fileOpen();
    void fileOpen(const QString &filename);
    void fileOpenRecent(const KURL& name);
    bool fileSave();
    bool fileSaveForm(); // not visible in menu, called from fileSave
    bool fileSaveAs();
    void fileSaveAll();
    void fileCreateTemplate();

    void editUndo();
    void editRedo();
    void editCut();
    void editCopy();
    void editPaste();
    void editDelete();
    void editSelectAll();
    void editLower();
    void editRaise();
    void editAdjustSize();
    void editLayoutHorizontal();
    void editLayoutVertical();
    void editLayoutHorizontalSplit();
    void editLayoutVerticalSplit();
    void editLayoutGrid();
    void editLayoutContainerHorizontal();
    void editLayoutContainerVertical();
    void editLayoutContainerGrid();
    void editBreakLayout();
    void editAccels();
    void editConnections();
    
    void editFormSettings();
    void editPreferences();
    void editShortcuts();

    void runForm();
    
private slots:
    void activeWindowChanged( QWidget *w );
    void updateUndoRedo( bool, bool, const QString &, const QString & );

    void toolSelected();

    void clipboardChanged();
    void selectionChanged();

    void chooseDocPath();
    void windowsMenuActivated( int id );
    void setupWindowActions();
    void createNewTemplate();
    
    void closeRunningForm(KProcess* process);
    
private:
    void setupMDI();
    void setupEditActions();
    void setupToolActions();
    void setupLayoutActions();
    void setupFileActions();
    void setupSettingsActions();
    void setupRunActions();
    void setupHelpActions();
    void setupRMBMenus();

    void setupPropertyEditor();
    void setupHierarchyView();
    void setupWorkspace();
    void setupActionEditor();
    void setupMessageLog();
    void setupPlugins();

    QWidget* previewFormInternal( QStyle* style = 0, QPalette* pal = 0 );

    void writeConfig();
    void readConfig();
    
    void setupRMBProperties( QValueList<int> &ids, QMap<QString, int> &props, QWidget *w );
    void handleRMBProperties( int id, QMap<QString, int> &props, QWidget *w );
    void setupRMBSpecialCommands( QValueList<int> &ids, QMap<QString, int> &commands, QWidget *w );
    void handleRMBSpecialCommands( int id, QMap<QString, int> &commands, QWidget *w );
    void setupRMBSpecialCommands( QValueList<int> &ids, QMap<QString, int> &commands, FormWindow *w );
    void handleRMBSpecialCommands( int id, QMap<QString, int> &commands, FormWindow *w );
    bool openEditor( QWidget *w, FormWindow *fw );
    void checkTempFiles();

    void addRecentlyOpened(const QString &fn);
    QWidget *findRealForm( QWidget *w );

    QString whatsThisFrom( const QString &key );

private slots:

private:
    PropertyEditor *propertyEditor;
    HierarchyView *hierarchyView;
    Workspace *wspace;
    QWidget *lastPressWidget;
    QWorkspace *qworkspace;
    KMenuBar *menubar;
    FormWindow *lastActiveFormWindow;
    bool breakLayout, layoutChilds, layoutSelected;
    QPoint grd;
    bool sGrid, snGrid;
    bool restoreConfig;
    bool backPix;
    bool splashScreen;
    QString docPath;
    
    KAction *actionEditUndo, *actionEditRedo, *actionEditCut, *actionEditCopy,
      *actionEditPaste, *actionEditDelete, *actionEditAdjustSize,
      *actionEditHLayout, *actionEditVLayout, *actionEditGridLayout,
      *actionEditSplitHorizontal, *actionEditSplitVertical,
      *actionEditSelectAll, *actionEditBreakLayout, *actionEditConnections,
      *actionEditLower, *actionEditRaise;
    
    KToggleAction* actionPointerTool, *actionConnectTool, *actionOrderTool;
    KAction* actionCurrentTool;
    KAction *actionEditFormSettings, *actionEditAccels;
    KRecentFilesAction* actionRecent;
    
    QPopupMenu *rmbWidgets;
    QPopupMenu *rmbFormWindow;
    QPopupMenu *windowMenu, *fileMenu;

    QToolBar *layoutToolBar;

    Preferences *prefDia;
    QMap<QString,QString> propertyDocumentation;
    bool client;
    QString templPath;
    ActionEditor *actionEditor;

    bool databaseAutoEdit;
    QTimer *updateSlotsTimer;
    bool inDebugMode;
    QObjectList debuggingForms;
    QString lastOpenFilter;
    QString menuHelpFile;
    AssistProc *assistant;
    
    bool previewing;
    MessageLog* messageLog;

public:
    QString lastSaveFilter;

};

#endif
