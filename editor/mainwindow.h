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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "metadatabase.h"
#ifndef KOMMANDER
//#include "actioninterface.h" // for GCC 2.7.* compatibility
//#include "editorinterface.h"
//#include "templatewizardiface.h"
//#include "eventinterface.h"
//#include "languageinterface.h"
//#include "filterinterface.h"
//#include "programinterface.h"
//#include "interpreterinterface.h"
//#include "preferenceinterface.h"
//#include "sourcetemplateiface.h"
//#include "sourceeditor.h"
#endif

#if defined(HAVE_KDE)
#include <kmainwindow.h>
#else
#include <qmainwindow.h>
#endif

#include <qmap.h>
#include <qguardedptr.h>
#ifndef KOMMANDER
//#include <private/qpluginmanager_p.h>
#endif
#include <qobjectlist.h>

class PropertyEditor;
class QWorkspace;
class QMenuBar;
class FormWindow;
class QAction;
class QActionGroup;
class QPopupMenu;
class HierarchyView;
class QCloseEvent;
class Workspace;
class ActionEditor;
#ifndef KOMMANDER
//class OutputWindow;
#endif
class QTimer;
class FindDialog;
class ReplaceDialog;
class GotoLineDialog;
class SourceFile;
class FormFile;
class QWidget;
#ifndef Q_OS_WIN32
class AssistProc;
#endif

#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
#include <qtoolbar.h>
#else
class QToolBar;
#endif
class Preferences;

#if defined(HAVE_KDE)
#define QMainWindow KMainWindow
#endif

class MainWindow : public QMainWindow
{
    Q_OBJECT

#undef QMainWindow

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
#ifndef KOMMANDER 
//    void editorClosed( SourceEditor *e );
#endif
    QWidget *isAFormWindowChild( QObject *o ) const;
    QWidget *isAToolBarChild( QObject *o ) const;

    void insertFormWindow( FormWindow *fw );
    QWorkspace *qWorkspace() const;

    void popupFormWindowMenu( const QPoint &gp, FormWindow *fw );
    void popupWidgetMenu( const QPoint &gp, FormWindow *fw, QWidget *w );

    QPopupMenu *setupNormalHierarchyMenu( QWidget *parent );
    QPopupMenu *setupTabWidgetHierarchyMenu( QWidget *parent, const char *addSlot, const char *removeSlot );

    FormWindow *openFormWindow( const QString &fn, bool validFileName = TRUE, FormFile *ff = 0 );
    bool isCustomWidgetUsed( MetaDataBase::CustomWidget *w );

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

#ifndef KOMMANDER
//    void editFunction( const QString &func, const QString &l = QString::null, bool rereadSource = FALSE );
#endif

    bool isPreviewing() const { return previewing; }

    FormWindow *activeForm() const { return lastActiveFormWindow; }

#ifndef KOMMANDER
//    TemplateWizardInterface* templateWizardInterface( const QString& className );
//    QStringList sourceTemplates() const;
//    SourceTemplateInterface* sourceTemplateInterface( const QString& templ );
//    QUnknownInterface* designerInterface() const { return desInterface; }
//    OutputWindow *outputWindow() const { return oWindow; }
#endif
    #ifndef KOMMANDER
//    void addPreferencesTab( QWidget *tab, const QString &title, QObject *receiver, const char *init_slot, const char *accept_slot );
   #endif
    void setModified( bool b, QWidget *window );
#ifndef KOMMANDER
//    void slotsChanged();
//    void updateFunctionList();
#endif
    void updateWorkspace();

    void formNameChanged( FormWindow *fw );

    int currentLayoutDefaultSpacing() const;
    int currentLayoutDefaultMargin() const;

#ifndef KOMMANDER
//    void saveAllBreakPoints();
//    void resetBreakPoints();

//   SourceFile *sourceFile();
#endif

public slots:
    void showProperties( QObject *w );
    void updateProperties( QObject *w );
    void showDialogHelp();
#ifndef KOMMANDER
//    void showDebugStep( QObject *o, int line );
//    void showStackFrame( QObject *o, int line );
//    void showErrorMessage( QObject *o, int line, const QString &errorMessage );
//    void finishedRun();
//    void breakPointsChanged();
#endif

signals:
	void addedFormFile(FormFile *);
	void removedFormFile(FormFile *);
    void currentToolChanged();
    void hasActiveForm( bool );
    void hasActiveWindow( bool );

    void formModified( bool );
    void formWindowsChanged();
    void formWindowChanged();
#ifndef KOMMANDER
//    void editorChanged();
#endif

protected:
    bool eventFilter( QObject *o, QEvent *e );
    void closeEvent( QCloseEvent *e );

public slots:
    void fileNew();
    void fileClose();
    void fileOpen();
    void fileOpen( const QString &filter, const QString &filename = "" );
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
    void editSlots();
    void editConnections();
    void editScriptObjects();

#ifndef KOMMANDER
//    SourceEditor *editSource();
//    SourceEditor *editSource( SourceFile *f );
//    SourceEditor *openSourceEdior();
//    void editPixmapCollection();
//    void editDatabaseConnections();
#endif
    void editFormSettings();
    void editPreferences();

#ifndef KOMMANDER
//    void searchFind();
//    void searchIncremetalFindMenu();
//    void searchIncremetalFind();
//    void searchIncremetalFindNext();
//    void searchReplace();
//    void searchGotoLine();
#endif

    void previewForm();
    void previewForm( const QString& );

    void toolsCustomWidget();

    void helpContents();
    void helpManual();
    void helpAbout();
    void helpAboutQt();
    void helpRegister();

private slots:
    void activeWindowChanged( QWidget *w );
    void updateUndoRedo( bool, bool, const QString &, const QString & );

    void toolSelected( QAction* );

    void clipboardChanged();
    void selectionChanged();

    void chooseDocPath();
    void windowsMenuActivated( int id );
    void setupWindowActions();

    void createNewTemplate();

    void setupRecentlyFilesMenu();
    void recentlyFilesMenuActivated( int id );

private:
    void setupMDI();
    void setupMenuBar();
    void setupEditActions();
#ifndef KOMMANDER
//    void setupSearchActions();
#endif
    void setupToolActions();
    void setupLayoutActions();
    void setupFileActions();
    void setupPreviewActions();
    void setupHelpActions();
    void setupRMBMenus();

    void setupPropertyEditor();
    void setupHierarchyView();
    void setupWorkspace();
    void setupActionEditor();
#ifndef KOMMANDER
//    void setupOutputWindow();
#endif

#ifndef KOMMANDER
//    void setupActionManager();
#endif
    void setupPlugins();

    void enableAll( bool enable );

    QWidget* previewFormInternal( QStyle* style = 0, QPalette* pal = 0 );

    void writeConfig();
    void readConfig();
    void readOldConfig();

    void setupRMBProperties( QValueList<int> &ids, QMap<QString, int> &props, QWidget *w );
    void handleRMBProperties( int id, QMap<QString, int> &props, QWidget *w );
    void setupRMBSpecialCommands( QValueList<int> &ids, QMap<QString, int> &commands, QWidget *w );
    void handleRMBSpecialCommands( int id, QMap<QString, int> &commands, QWidget *w );
    void setupRMBSpecialCommands( QValueList<int> &ids, QMap<QString, int> &commands, FormWindow *w );
    void handleRMBSpecialCommands( int id, QMap<QString, int> &commands, FormWindow *w );
    bool openEditor( QWidget *w, FormWindow *fw );
    void rebuildCustomWidgetGUI();
    void checkTempFiles();

    void addRecentlyOpened( const QString &fn, QStringList &lst );
    #ifndef KOMMANDER
//    enum LineMode { Error, Step, StackFrame };
//    void showSourceLine( QObject *o, int line, LineMode lm );
#endif
    QWidget *findRealForm( QWidget *w );

    QString whatsThisFrom( const QString &key );

private slots:
#ifndef KOMMANDER
//    void doSlotsChanged();
#endif

#ifndef KOMMANDER
private:
//    struct Tab
//    {
//	QWidget *w;
//	QString title;
//	QObject *receiver;
//	const char *init_slot, *accept_slot;
//#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
//	bool operator==( const Tab& ) const { return FALSE; }
//#endif
//    };
#endif

private:
    PropertyEditor *propertyEditor;
    HierarchyView *hierarchyView;
    Workspace *wspace;
    QWidget *lastPressWidget;
    QWorkspace *qworkspace;
#if defined(HAVE_KDE)
    KMenuBar *menubar;
#else
    QMenuBar *menubar;
#endif
    FormWindow *lastActiveFormWindow;
    bool breakLayout, layoutChilds, layoutSelected;
    QPoint grd;
    bool sGrid, snGrid;
    bool restoreConfig;
    bool backPix;
    bool splashScreen;
    QString docPath;
    QString fileFilter;

    QAction *actionEditUndo, *actionEditRedo, *actionEditCut, *actionEditCopy,
    *actionEditPaste, *actionEditDelete,
    *actionEditAdjustSize,
    *actionEditHLayout, *actionEditVLayout, *actionEditGridLayout,
    *actionEditSplitHorizontal, *actionEditSplitVertical,
    *actionEditSelectAll, *actionEditBreakLayout, *actionEditSlots, *actionEditConnections,
    *actionEditLower, *actionEditRaise;
    QActionGroup *actionGroupTools;
    QAction* actionPointerTool, *actionConnectTool, *actionOrderTool;
    QAction* actionCurrentTool;
    QAction *actionHelpContents, *actionHelpAbout, *actionHelpAboutQt, *actionHelpWhatsThis;
    QAction *actionHelpManual;
#if defined(QT_NON_COMMERCIAL)
    QAction *actionHelpRegister;
#endif
    QAction *actionToolsCustomWidget, *actionEditPreferences;
    QAction *actionWindowTile, *actionWindowCascade, *actionWindowClose, *actionWindowCloseAll;
    QAction *actionWindowNext, *actionWindowPrevious;
    QAction *actionEditFormSettings, *actionEditAccels;
#ifndef KOMMANDER
//    QAction *actionSearchFind, *actionSearchIncremetal, *actionSearchReplace, *actionSearchGotoLine;
#endif

    QAction *actionEditScriptObjects;

    QPopupMenu *rmbWidgets;
    QPopupMenu *rmbFormWindow;
    QPopupMenu *customWidgetMenu, *windowMenu, *fileMenu, *recentlyFilesMenu;

    QToolBar *customWidgetToolBar, *layoutToolBar;


    Preferences *prefDia;
    QMap<QString,QString> propertyDocumentation;
    bool client;
    QString templPath;
    ActionEditor *actionEditor;
#ifndef KOMMANDER
	/* We don't need QCom/Interface functionality for Kommander. Scrap these interfaces */ 
//    QPluginManager<ActionInterface> *actionPluginManager;
//    QPluginManager<EditorInterface> *editorPluginManager;
//    QPluginManager<TemplateWizardInterface> *templateWizardPluginManager;
//    QPluginManager<ProgramInterface> *programPluginManager;
//    QPluginManager<InterpreterInterface> *interpreterPluginManager;
//    QPluginManager<PreferenceInterface> *preferencePluginManager;
//    QPluginManager<SourceTemplateInterface> *sourceTemplatePluginManager;
//    QPtrList<SourceEditor> sourceEditors;
#endif
    bool previewing;
#ifndef KOMMANDER
//    QUnknownInterface *desInterface; // scrap all interfaces:
//    OutputWindow *oWindow;
#endif

    QStringList recentlyFiles;

#ifndef KOMMANDER
	// preference tabs now static
//    QValueList<Tab> preferenceTabs;
#endif
    bool databaseAutoEdit;
    QTimer *updateSlotsTimer;
#ifndef KOMMANDER
//    QLineEdit *incrementalSearch;
//    QGuardedPtr<FindDialog> findDialog;
//    QGuardedPtr<ReplaceDialog> replaceDialog;
//    QGuardedPtr<GotoLineDialog> gotoLineDialog;
#endif
    bool inDebugMode;
    QObjectList debuggingForms;
    QString lastOpenFilter;
    QGuardedPtr<QWidget> previewedForm;
    QString menuHelpFile;
#ifndef Q_OS_WIN32
    AssistProc *assistant;
#endif

public:
    QString lastSaveFilter;

};

#ifndef KOMMANDER
//class SenderObject : public QObject
//{
//    Q_OBJECT
//
//public:
//    SenderObject( QUnknownInterface *i ) : iface( i ) { iface->addRef(); }
//    ~SenderObject() { iface->release(); }
//
//public slots:
//    void emitInitSignal() { emit initSignal( iface ); }
//    void emitAcceptSignal() { emit acceptSignal( iface ); }
//
//signals:
//    void initSignal( QUnknownInterface * );
//    void acceptSignal( QUnknownInterface * );
//
//private:
//    QUnknownInterface *iface;
//
//};
#endif

#endif
