/**********************************************************************
 This file is based on Qt Designer, Copyright (C) 2000 Trolltech AS. All rights reserved.

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

#include "mainwindow.h"

#include <qapplication.h>
#include <qclipboard.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmap.h>
#include <qmessagebox.h>
#include <qsignalmapper.h>
#include <qspinbox.h>
#include <qstatusbar.h>
#include <qstylefactory.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qworkspace.h>
#include <stdlib.h>

#include "defs.h"
#include "widgetdatabase.h"
#include "widgetfactory.h"
#include "preferences.h"
#include "formwindow.h"
#include "newformimpl.h"
#include "resource.h"
#include "workspace.h"
#include "createtemplate.h"
#include "hierarchyview.h"
#include "formsettingsimpl.h"
#include "styledbutton.h"
#include "customwidgeteditorimpl.h"
#include "connectioneditorimpl.h"
#include "actioneditorimpl.h"
#include "formfile.h"
#ifndef QT_NO_SQL
#include "dbconnectionsimpl.h"
#include "dbconnectionimpl.h"
#endif
#include "command.h"

#include <kaction.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kkeydialog.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kpopupmenu.h>
#include <kstatusbar.h>
#include <kstdguiitem.h>

const QString toolbarHelp = "<p>Toolbars contain a number of buttons to "
"provide quick access to often used functions.%1"
"<br>Click on the toolbar handle to hide the toolbar, "
"or drag and place the toolbar to a different location.</p>";

static QIconSet createIconSet( const QString &name )
{
  QIconSet ic( PixmapChooser::loadPixmap( name, PixmapChooser::Small ) );
  ic.setPixmap( PixmapChooser::loadPixmap( name, PixmapChooser::Disabled ), QIconSet::Small, QIconSet::Disabled );
  return ic;
}

int forms = 0;

void MainWindow::setupEditActions()
{
  qDebug("Setup Edit Actions");
  actionEditUndo = KStdAction::undo(this, SLOT(editUndo()), actionCollection());
  actionEditUndo->setText(i18n("&Undo: Not Available"));
  actionEditUndo->setToolTip(i18n("Undoes the last action"));
  actionEditUndo->setWhatsThis(whatsThisFrom("Edit|Undo"));
  actionEditUndo->setEnabled(false);

  actionEditRedo = KStdAction::redo(this, SLOT(editRedo()), actionCollection());
  actionEditRedo->setText(i18n("&Redo: Not Available"));
  actionEditRedo->setToolTip(i18n("Redoes the last undone operation"));
  actionEditRedo->setWhatsThis(whatsThisFrom("Edit|Redo"));
  actionEditRedo->setEnabled(false);

  actionEditCut = KStdAction::cut(this, SLOT(editCut()), actionCollection());
  actionEditCut->setToolTip(i18n("Cuts the selected widgets and puts them on the clipboard"));
  actionEditCut->setWhatsThis(whatsThisFrom("Edit|Cut"));
  actionEditCut->setEnabled(false);

  actionEditCopy = KStdAction::copy(this, SLOT(editCopy()), actionCollection());
  actionEditCopy->setToolTip(i18n("Copies the selected widgets to the clipboard"));
  actionEditCopy->setWhatsThis(whatsThisFrom("Edit|Copy"));
  actionEditCopy->setEnabled(false);

  actionEditPaste = KStdAction::paste(this, SLOT(editPaste()), actionCollection());
  actionEditPaste->setToolTip(i18n("Pastes the clipboard's contents"));
  actionEditPaste->setWhatsThis(whatsThisFrom("Edit|Paste"));
  actionEditPaste->setEnabled(false);

  actionEditDelete = new KAction(i18n("Delete"), Key_Delete, this, SLOT(editDelete()), 
                                 actionCollection(), "edit_delete");
  actionEditDelete->setToolTip(i18n("Deletes the selected widgets"));
  actionEditDelete->setWhatsThis(whatsThisFrom("Edit|Delete"));
  actionEditDelete->setEnabled(false);

  actionEditSelectAll = KStdAction::selectAll(this, SLOT(editSelectAll()), actionCollection());
  actionEditSelectAll->setToolTip(i18n("Selects all widgets"));
  actionEditSelectAll->setWhatsThis(whatsThisFrom("Edit|Select All"));
  
  actionEditRaise = new KAction(i18n("Bring to Front"), createIconSet("editraise.xpm"), 
                                KShortcut::null(), this, SLOT(editRaise()), actionCollection(), "edit_raise");
  actionEditRaise->setToolTip(i18n("Raises the selected widgets"));
  actionEditRaise->setEnabled(false);
  
  actionEditLower = new KAction(i18n("Send to Back"), createIconSet("editlower.xpm"), 
                                KShortcut::null(), this, SLOT(editLower()), actionCollection(), "edit_lower");
  actionEditLower->setToolTip(i18n("Lowers the selected widgets"));
  actionEditLower->setWhatsThis(i18n("Lowers the selected widgets"));
  actionEditLower->setEnabled(false);

  actionEditAccels = new KAction(i18n("Check Accelerators"), ALT + Key_R, this, SLOT(editAccels()), 
                                 actionCollection(), "edit_check_accel");
  actionEditAccels->setToolTip(i18n("Checks if the accelerators used in the form are unique"));
  actionEditAccels->setWhatsThis(whatsThisFrom("Edit|Check Accelerator"));
  connect(this, SIGNAL(hasActiveForm(bool)), actionEditAccels, SLOT(setEnabled(bool)));

  actionEditConnections = new KAction(i18n("Connections"), createIconSet("connecttool.xpm"), 
                                      KShortcut::null(), this, SLOT(editConnections()), actionCollection(),
                                      "edit_connections");
  actionEditConnections->setToolTip(i18n("Opens a dialog for editing connections"));
  actionEditConnections->setWhatsThis(whatsThisFrom("Edit|Connections"));
  connect(this, SIGNAL(hasActiveForm(bool)), actionEditConnections, SLOT(setEnabled(bool)));

  actionEditFormSettings = new KAction(i18n("Form Settings"), KShortcut::null(),
                                       this, SLOT(editFormSettings()), actionCollection(), "edit_form");
  actionEditFormSettings->setToolTip(i18n("Opens a dialog to change the form's settings"));
  actionEditFormSettings->setWhatsThis(whatsThisFrom("Edit|Form Settings"));
  connect(this, SIGNAL(hasActiveForm(bool)), actionEditFormSettings, SLOT(setEnabled(bool)));

  KToolBar *tb = new KToolBar(this, "Edit");
  tb->setFullSize(false);
  QWhatsThis::add(tb, i18n("<b>The Edit toolbar</b>%1").arg(toolbarHelp));
  addToolBar(tb, i18n("Edit"));
  actionEditUndo->plug(tb);
  actionEditRedo->plug(tb);
  tb->addSeparator();
  actionEditCut->plug(tb);
  actionEditCopy->plug(tb);
  actionEditPaste->plug(tb);

  QPopupMenu *menu = new QPopupMenu(this, "Edit");
  menuBar()->insertItem(i18n("&Edit"), menu);
  actionEditUndo->plug(menu);
  actionEditRedo->plug(menu);
  menu->insertSeparator();
  actionEditCut->plug(menu);
  actionEditCopy->plug(menu);
  actionEditPaste->plug(menu);
  actionEditDelete->plug(menu);
  actionEditSelectAll->plug(menu);
  actionEditAccels->plug(menu);
  menu->insertSeparator();
  actionEditConnections->plug(menu);
  actionEditFormSettings->plug(menu);
  menu->insertSeparator();
  
}


void MainWindow::setupLayoutActions()
{
  qDebug("Setup Layout Actions");
  actionEditAdjustSize = new KAction(i18n("Adjust Size"), createIconSet("adjustsize.xpm"),
                                     CTRL + Key_J, this, SLOT(editAdjustSize()), actionCollection(), "edit_adjust_size");
  actionEditAdjustSize->setToolTip(i18n("Adjusts the size of the selected widget"));
  actionEditAdjustSize->setWhatsThis(whatsThisFrom("Layout|Adjust Size"));
  actionEditAdjustSize->setEnabled(false);

  actionEditHLayout = new KAction(i18n("Lay Out Horizontally"), createIconSet("edithlayout.xpm"),
                                  CTRL + Key_H, this, SLOT(editLayoutHorizontal()), actionCollection(), "edit_layout_h");
  actionEditHLayout->setToolTip(i18n("Lays out the selected widgets horizontally"));
  actionEditHLayout->setWhatsThis(whatsThisFrom("Layout|Lay Out Horizontally"));
  actionEditHLayout->setEnabled(false);

  actionEditVLayout = new KAction(i18n("Lay Out Vertically"), createIconSet("editvlayout.xpm"),
                                  CTRL + Key_L, this, SLOT(editLayoutVertical()), actionCollection(), "edit_layout_v");
  actionEditVLayout->setToolTip(i18n("Lays out the selected widgets vertically"));
  actionEditVLayout->setWhatsThis(whatsThisFrom("Layout|Lay Out Vertically"));
  actionEditVLayout->setEnabled(false);

  actionEditGridLayout = new KAction(i18n("Lay Out in a Grid"), createIconSet("editgrid.xpm"),
                                     CTRL + Key_G, this, SLOT(editLayoutGrid()), actionCollection(), "edit_layout_grid");
  actionEditGridLayout->setToolTip(i18n("Lays out the selected widgets in a grid"));
  actionEditGridLayout->setWhatsThis(whatsThisFrom("Layout|Lay Out in a Grid"));
  actionEditGridLayout->setEnabled(false);

  actionEditSplitHorizontal = new KAction(i18n("Lay Out Horizontally in Splitter"), createIconSet("editvlayoutsplit.xpm"),
                                          KShortcut::null(), this, SLOT(editLayoutHorizontalSplit()), actionCollection(),
                                          "edit_split_h");
  actionEditSplitHorizontal->setToolTip(i18n("Lays out the selected widgets horizontally in a splitter"));
  actionEditSplitHorizontal->setWhatsThis(whatsThisFrom("Layout|Lay Out Horizontally in Splitter"));
  actionEditSplitHorizontal->setEnabled(false);

  actionEditSplitVertical = new KAction(i18n("Lay Out Vertically in Splitter"), createIconSet("edithlayoutsplit.xpm"),
                                        KShortcut::null(), this, SLOT(editLayoutVerticalSplit()), actionCollection(),
                                        "edit_split_v");
  actionEditSplitVertical->setToolTip(i18n("Lays out the selected widgets vertically in a splitter"));
  actionEditSplitVertical->setWhatsThis(whatsThisFrom("Layout|Lay Out Vertically (in Splitter)"));
  actionEditSplitVertical->setEnabled(false);

  actionEditBreakLayout = new KAction(i18n("Break Layout"), createIconSet("editbreaklayout.xpm"),
                                      CTRL + Key_B, this, SLOT(editBreakLayout()), actionCollection(), "edit_break_layout");
  actionEditBreakLayout->setToolTip(i18n("Breaks the selected layout"));
  actionEditBreakLayout->setWhatsThis(whatsThisFrom("Layout|Break Layout"));
  
  int id = WidgetDatabase::idFromClassName("Spacer");
  KToggleAction *a = new KToggleAction(QString::number(id).latin1(), WidgetDatabase::iconSet(id), 
                                       KShortcut::null(), this, SLOT(toolSelected()), actionCollection(), "edit_spacer");
  a->setExclusiveGroup("tool");
  a->setText(i18n("Add ") + WidgetDatabase::className(id));
  a->setToolTip(i18n("Insert a %1").arg(WidgetDatabase::toolTip(id)));
  a->setWhatsThis(i18n("<b>A %1</b><p>%2</p>"
      "<p>Click to insert a single %3,"
      "or double click to keep the tool selected.").arg(WidgetDatabase::toolTip(id)).
      arg(WidgetDatabase::whatsThis(id)).arg(WidgetDatabase::toolTip(id)));

  QWhatsThis::add(layoutToolBar, i18n("<b>The Layout toolbar</b>%1").arg(toolbarHelp));
  actionEditAdjustSize->plug(layoutToolBar);
  layoutToolBar->addSeparator();
  actionEditHLayout->plug(layoutToolBar);
  actionEditVLayout->plug(layoutToolBar);
  actionEditGridLayout->plug(layoutToolBar);
  actionEditSplitHorizontal->plug(layoutToolBar);
  actionEditSplitVertical->plug(layoutToolBar);
  actionEditBreakLayout->plug(layoutToolBar);
  layoutToolBar->addSeparator();
  a->plug(layoutToolBar);

  QPopupMenu *menu = new QPopupMenu(this, "Layout");
  menuBar()->insertItem(i18n("&Layout"), menu);
  actionEditAdjustSize->plug(menu);
  menu->insertSeparator();
  actionEditHLayout->plug(menu);
  actionEditVLayout->plug(menu);
  actionEditGridLayout->plug(menu);
  actionEditSplitHorizontal->plug(menu);
  actionEditSplitVertical->plug(menu);
  actionEditBreakLayout->plug(menu);
  menu->insertSeparator();
  a->plug(menu);
}

void MainWindow::setupToolActions()
{
  qDebug("Setup Tool Actions");
  actionPointerTool = new KToggleAction(i18n("Pointer"), "arrow", Key_F2,
                                        this, SLOT(toolSelected()), actionCollection(), 
                                        QString::number(POINTER_TOOL).latin1());
  actionPointerTool->setToolTip(i18n("Selects the pointer tool"));
  actionPointerTool->setWhatsThis(whatsThisFrom("Tools|Pointer"));
  actionPointerTool->setExclusiveGroup("tool");

  actionConnectTool = new KToggleAction(i18n("Connect Signal/Slots"), createIconSet("connecttool.xpm"),
                                        Key_F3, this, SLOT(toolSelected()), actionCollection(), 
                                        QString::number(CONNECT_TOOL).latin1());
  actionConnectTool->setToolTip(i18n("Selects the connection tool"));
  actionConnectTool->setWhatsThis(whatsThisFrom("Tools|Connect Signals and Slots"));
  actionConnectTool->setExclusiveGroup("tool");

  actionOrderTool = new KToggleAction(i18n("Tab Order"), createIconSet("ordertool.xpm"),
                                      Key_F4, this, SLOT(toolSelected()), actionCollection(),
                                      QString::number(ORDER_TOOL).latin1());
  actionOrderTool->setToolTip(i18n("Selects the tab order tool"));
  actionOrderTool->setWhatsThis(whatsThisFrom("Tools|Tab Order"));
  actionOrderTool->setExclusiveGroup("tool");

  KToolBar *tb = new KToolBar(this, "Tools");
  tb->setFullSize(false);
  QWhatsThis::add(tb, i18n("<b>The Tools toolbar</b>%1").arg(toolbarHelp));

  addToolBar(tb, i18n("Tools"), QMainWindow::DockTop, true);
  actionPointerTool->plug(tb);
  actionConnectTool->plug(tb);
  actionOrderTool->plug(tb);

  QPopupMenu *mmenu = new QPopupMenu(this, "Tools");
  menuBar()->insertItem(i18n("&Tools"), mmenu);
  actionPointerTool->plug(mmenu);
  actionConnectTool->plug(mmenu);
  actionOrderTool->plug(mmenu);
  mmenu->insertSeparator();

  customWidgetToolBar = 0;
  customWidgetMenu = 0;

  actionToolsCustomWidget = new KAction(i18n("Custom Widgets"), createIconSet("customwidget.xpm"), 
                                        KShortcut::null(), this, SLOT(toolsCustomWidget()), 0);
  actionToolsCustomWidget->setToolTip(i18n("Opens a dialog to add and change custom widgets"));
  actionToolsCustomWidget->setWhatsThis(whatsThisFrom("Tools|Custom|Edit Custom Widgets"));

  for (int j = 0; j < WidgetDatabase::numWidgetGroups(); ++j)
  {
    QString grp = WidgetDatabase::widgetGroup(j);
    if (!WidgetDatabase::isGroupVisible(grp) || WidgetDatabase::isGroupEmpty(grp))
      continue;
    KToolBar *tb = new KToolBar(this, grp.latin1());
    tb->setFullSize(false);
    bool plural = grp[(int) grp.length() - 1] == 's';
    if (plural)
    {
      QWhatsThis::add(tb, i18n("<b>The %1</b>%2").arg(grp).arg(toolbarHelp).
          arg(i18n(" Click on a button to insert a single widget, "
              "or double click to insert multiple %1.")).arg(grp));
    } else
    {
      QWhatsThis::add(tb, i18n("<b>The %1 Widgets</b>%2").arg(grp).arg(toolbarHelp).
          arg(i18n(" Click on a button to insert a single %1 widget, "
              "or double click to insert multiple widgets.")).arg(grp));
    }
    addToolBar(tb, grp);
    QPopupMenu *menu = new QPopupMenu(this, grp.latin1());
    mmenu->insertItem(grp, menu);

    if (grp == "Custom")
    {
      if (!customWidgetMenu)
        actionToolsCustomWidget->plug(menu);
      else
        menu->insertSeparator();
      customWidgetMenu = menu;
      customWidgetToolBar = tb;
    }

    for (int i = 0; i < WidgetDatabase::count(); ++i)
    {
      if (WidgetDatabase::group(i) != grp)
        continue;               // only widgets, i.e. not forms and temp stuff
      KToggleAction *a = new KToggleAction(WidgetDatabase::className(i), KShortcut::null(),
                                           this, SLOT(toolSelected()), actionCollection(), QString::number(i).latin1());
      a->setExclusiveGroup("tool");
      QString atext = WidgetDatabase::className(i);
      if (atext[0] == 'Q')
        atext = atext.mid(1);
      while (atext.length() && atext[0] >= 'a' && atext[0] <= 'z')
        atext = atext.mid(1);
      if (atext.isEmpty())
        atext = WidgetDatabase::className(i);
      a->setText(atext);
      QString ttip = WidgetDatabase::toolTip(i);
      a->setIconSet(WidgetDatabase::iconSet(i));
      a->setToolTip(ttip);
      if (!WidgetDatabase::isWhatsThisLoaded())
        WidgetDatabase::loadWhatsThis(documentationPath());
      a->setToolTip(i18n("Insert a %1").arg(WidgetDatabase::className(i)));

      QString whats = i18n("<b>A %1</b>").arg(WidgetDatabase::className(i));
      if (!WidgetDatabase::whatsThis(i).isEmpty())
        whats += QString("<p>%1</p>").arg(WidgetDatabase::whatsThis(i));
      a->setWhatsThis(whats + i18n("<p>Double click on this tool to keep it selected.</p>"));

      if (grp != "KDE")
        a->plug(tb);
      a->plug(menu);
    }
  }

  if (!customWidgetToolBar)
  {
    KToolBar *tb = new KToolBar(this, "Custom Widgets");
    tb->setFullSize(false);
    QWhatsThis::add(tb, i18n("<b>The Custom Widgets toolbar</b>%1"
        "<p>Click <b>Edit Custom Widgets...</b> in the <b>Tools|Custom</b> menu to "
            "add and change custom widgets</p>").arg(toolbarHelp).
            arg(i18n(" Click on the buttons to insert a single widget, "
            "or double click to insert multiple widgets.")));
    addToolBar(tb, i18n("Custom"));
    customWidgetToolBar = tb;
    QPopupMenu *menu = new QPopupMenu(this, "Custom Widgets");
    mmenu->insertItem(i18n("Custom"), menu);
    customWidgetMenu = menu;
    customWidgetToolBar->hide();
    actionToolsCustomWidget->plug(customWidgetMenu);
    customWidgetMenu->insertSeparator();
  }

  resetTool();
}

void MainWindow::setupFileActions()
{
  qDebug("Setup File Actions");
  KToolBar *tb = new KToolBar(this, "File");
  tb->setFullSize(false);

  QWhatsThis::add(tb, i18n("<b>The File toolbar</b>%1").arg(toolbarHelp));
  addToolBar(tb, i18n("File"));
  fileMenu = new QPopupMenu(this, "File");
  menuBar()->insertItem(i18n("&File"), fileMenu);

  KAction *a = KStdAction::openNew(this, SLOT(fileNew()), actionCollection());
  a->setToolTip(i18n("Creates a new dialog"));
  a->setWhatsThis(whatsThisFrom("File|New"));
  a->plug(tb);
  a->plug(fileMenu);

  a = KStdAction::open(this, SLOT(fileOpen()), actionCollection());
  a->setToolTip(i18n("Opens an existing dialog "));
  a->setWhatsThis(whatsThisFrom("File|Open"));
  a->plug(tb);
  a->plug(fileMenu);

  fileMenu->insertSeparator();
  a = KStdAction::close(this, SLOT(fileClose()), actionCollection());
  a->setToolTip(i18n("Closes the current dialog"));
  a->setWhatsThis(whatsThisFrom("File|Close"));
  connect(this, SIGNAL(hasActiveWindow(bool)), a, SLOT(setEnabled(bool)));
  a->plug(fileMenu);

  fileMenu->insertSeparator();

  a = KStdAction::save(this, SLOT(fileSave()), actionCollection());
  a->setToolTip(i18n("Saves the current dialog"));
  a->setWhatsThis(whatsThisFrom("File|Save"));
  connect(this, SIGNAL(hasActiveWindow(bool)), a, SLOT(setEnabled(bool)));
  a->plug(tb);
  a->plug(fileMenu);

  a = KStdAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
  a->setToolTip(i18n("Saves the current dialog with a new filename"));
  a->setWhatsThis(whatsThisFrom("File|Save As"));
  connect(this, SIGNAL(hasActiveWindow(bool)), a, SLOT(setEnabled(bool)));
  a->plug(fileMenu);

  a = new KAction(i18n("Save All"), "save_all", KShortcut::null(), this, SLOT(fileSaveAll()), 
                  actionCollection(), "file_close_all");
  a->setToolTip(i18n("Saves all open dialogs"));
  a->setWhatsThis(whatsThisFrom("File|Save All"));
  connect(this, SIGNAL(hasActiveWindow(bool)), a, SLOT(setEnabled(bool)));
  a->plug(fileMenu);
  
  fileMenu->insertSeparator();
  
  recentlyFilesMenu = new QPopupMenu(this);
  fileMenu->insertItem(i18n("Recently Opened Files "), recentlyFilesMenu);
  connect(recentlyFilesMenu, SIGNAL(aboutToShow()), this, SLOT(setupRecentlyFilesMenu()));
  connect(recentlyFilesMenu, SIGNAL(activated(int)), this, SLOT(recentlyFilesMenuActivated(int)));
  fileMenu->insertSeparator();

  a = KStdAction::quit(kapp, SLOT(closeAllWindows()), actionCollection());
  a->setToolTip(i18n("Quits the application and prompts to save any changed dialogs"));
  a->setWhatsThis(whatsThisFrom("File|Exit"));
  a->plug(fileMenu);
}


void MainWindow::setupRunActions()
{
  qDebug("Setup Run Actions");
  QPopupMenu *menu = new QPopupMenu(this, "Run");
  menuBar()->insertItem(i18n("&Run"), menu);

  KAction* a = new KAction(i18n("Run Dialog"), "run", CTRL + Key_R,
                           this, SLOT(runForm()), actionCollection(), "run");
  a->setToolTip(i18n("Executes dialog"));
  a->setWhatsThis(whatsThisFrom("Run|Run dialog"));
  connect(this, SIGNAL(hasActiveForm(bool)), a, SLOT(setEnabled(bool)));
  a->plug(menu);
}

void MainWindow::setupWindowActions()
{
  qDebug("Setup Window Actions");
  static bool windowActionsSetup = false;
  if (!windowActionsSetup)
  {
    windowActionsSetup = true;

    actionWindowTile = new KAction(i18n("Tile"), KShortcut::null(), qworkspace, SLOT(tile()), 
                                   actionCollection(), "window_tile");
    actionWindowTile->setToolTip(i18n("Tiles the windows so that they are all visible"));
    actionWindowTile->setWhatsThis(whatsThisFrom("Window|Tile"));
    
    actionWindowCascade = new KAction(i18n("Cascade"), KShortcut::null(), qworkspace, SLOT(cascade()), 
                                      actionCollection(), "window_cascade");
    actionWindowCascade->setToolTip(i18n("Cascades the windows so that all their title bars are visible"));
    actionWindowCascade->setWhatsThis(whatsThisFrom("Window|Cascade"));
    
    actionWindowClose = new KAction(i18n("Cascade"), KShortcut::null(), qworkspace, SLOT(closeActiveWindow()),
                                    actionCollection(), "window_close");
    actionWindowClose->setToolTip(i18n("Closes the active window"));
    actionWindowClose->setWhatsThis(whatsThisFrom("Window|Close"));
    
    actionWindowCloseAll = new KAction(i18n("Close All"), KShortcut::null(), qworkspace, 
                                       SLOT(closeAllWindows()), actionCollection(), "window_close_all");
    actionWindowCloseAll->setToolTip(i18n("Closes all form windows"));
    actionWindowCloseAll->setWhatsThis(whatsThisFrom("Window|Close All"));
    
    actionWindowNext = new KAction(i18n("Next"), Key_F6, qworkspace, 
                                   SLOT(activateNextWindow()), actionCollection(), "window_next");
    actionWindowNext->setToolTip(i18n("Activates the next window"));
    actionWindowNext->setWhatsThis(whatsThisFrom("Window|Next"));
    
    actionWindowPrevious = new KAction(i18n("Previous"), CTRL + SHIFT + Key_F6, qworkspace, 
                                       SLOT(activatePreviousWindow()), actionCollection(), "window_prev");
    actionWindowPrevious->setToolTip(i18n("Activates the previous window"));
    actionWindowPrevious->setWhatsThis(whatsThisFrom("Window|Previous"));
  }

  if (!windowMenu)
  {
    windowMenu = new KPopupMenu(this, "Window");
    menuBar()->insertItem(i18n("&Window"), windowMenu);
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(setupWindowActions()));
  } 
  else
    windowMenu->clear();
  
  actionWindowClose->plug(windowMenu);
  actionWindowCloseAll->plug(windowMenu);
  windowMenu->insertSeparator();
  actionWindowNext->plug(windowMenu);
  actionWindowPrevious->plug(windowMenu);
  windowMenu->insertSeparator();
  actionWindowTile->plug(windowMenu);
  actionWindowCascade->plug(windowMenu);
  windowMenu->insertSeparator();
  windowMenu->insertItem(i18n("Vie&ws"), createDockWindowMenu(NoToolBars));
  windowMenu->insertItem(i18n("Tool&bars"), createDockWindowMenu(OnlyToolBars));
  
  QWidgetList windows = qworkspace->windowList();
  if (windows.count() && formWindow())
    windowMenu->insertSeparator();
  int j = 0;
  for (int i = 0; i < int (windows.count()); ++i)
  {
    QWidget *w = windows.at(i);
    if (!w->inherits("FormWindow") && !w->inherits("SourceEditor"))
      continue;
    j++;
    QString itemText;
    if (j < 10)
      itemText = QString("&%1 ").arg(j);
    if (w->inherits("FormWindow"))  
      itemText += w->name();
    else
      itemText += w->caption();

    int id = windowMenu->insertItem(itemText, this, SLOT(windowsMenuActivated(int)));
    windowMenu->setItemParameter(id, i);
    windowMenu->setItemChecked(id, qworkspace->activeWindow() == windows.at(i));
  }
}


void MainWindow::setupSettingsActions()
{    
  KPopupMenu *settings = new KPopupMenu(this, "Settings");
  KAction* a = KStdAction::preferences(this, SLOT(editPreferences()), actionCollection());
  a->setToolTip(i18n("Opens a dialog to change preferences"));
  a->setWhatsThis(whatsThisFrom("Edit|Preferences"));
  a->plug(settings);
  
  a = KStdAction::keyBindings(this, SLOT(editShortcuts()), actionCollection());
  a->setToolTip(i18n("Opens a dialog to change shortcuts"));
  a->plug(settings);
  
  menuBar()->insertItem( i18n("&Settings"), settings);
} 

void MainWindow::setupHelpActions()
{
  KPopupMenu *help = helpMenu();
  menuBar()->insertItem( i18n("&Help"), help);
}


void MainWindow::fileNew()
{
  statusBar()->message(i18n("Create a new dialog..."));
  NewForm dlg(this, QString::null);
  dlg.exec();
  statusBar()->clear();
}

void MainWindow::fileClose()
{
  QWidget *w = qworkspace->activeWindow();
  if (w->inherits("FormWindow"))
  {
    emit removedFormFile(((FormWindow *) w)->formFile());
    ((FormWindow *) w)->formFile()->close();
  }
}

void MainWindow::fileOpen()     // as called by the menu
{
  fileOpen("", "");
}

void MainWindow::fileOpen(const QString & filter, const QString & fn)
{
  statusBar()->message(i18n("Open a file..."));
  QStringList additionalSources;

  QStringList filterlist;
  if (filter.isEmpty())
    filterlist << i18n("*.kmdr|Kommander Files");
  else
    filterlist << filter;

  QString filters = filterlist.join(" ");
  QStringList filenames;
  if (fn.isEmpty())
    filenames = KFileDialog::getOpenFileNames(QString::null, filters, this, i18n("Open Files"));
  else
    filenames << fn;
  for (QStringList::Iterator fit = filenames.begin(); fit != filenames.end(); ++fit)
  {
    QString filename = *fit;
    if (!filename.isEmpty())
    {
      QFileInfo fi(filename);

      openFormWindow(filename);

      addRecentlyOpened(filename, recentlyFiles);
    }
  }
}



FormWindow *MainWindow::openFormWindow( const QString &filename, bool validFileName, FormFile *ff )
{
  if (filename.isEmpty())
    return 0;

  bool makeNew = false;

  if (!QFile::exists(filename))
  {
    makeNew = true;
  } else
  {
    QFile f(filename);
    f.open(IO_ReadOnly);
    QTextStream ts(&f);
    makeNew = ts.read().length() < 2;
  }
  if (!makeNew)
  {
    statusBar()->message(i18n("Reading file '%1'...").arg(filename));
    if (QFile::exists(filename))
    {
      QApplication::setOverrideCursor(WaitCursor);
      Resource resource(this);
      if (!ff)
        ff = new FormFile(filename, false);
      bool b = resource.load(ff) && (FormWindow *) resource.widget();
      if (!validFileName && resource.widget())
        ((FormWindow *) resource.widget())->setFileName(QString::null);
      QApplication::restoreOverrideCursor();
      if (b)
      {
        rebuildCustomWidgetGUI();
        statusBar()->message(i18n("Loaded file '%1'").arg(filename), 3000);
      } else
      {
        statusBar()->message(i18n("Failed to load file '%1'").arg(filename), 5000);
        QMessageBox::information(this, i18n("Load File"),
            i18n("Could not load file '%1'").arg(filename));
        delete ff;
      }
      return (FormWindow *) resource.widget();
    } else
    {
      statusBar()->clear();
    }
  } else
  {
    fileNew();
    if (formWindow())
      formWindow()->setFileName(filename);
    return formWindow();
  }
  return 0;
}

bool MainWindow::fileSave()
{
  return fileSaveForm();
}

bool MainWindow::fileSaveForm()
{
  FormWindow* fw = 0;
  if (!fw)
    fw = formWindow();
  if (!fw || !fw->formFile()->save())
    return false;
  QApplication::restoreOverrideCursor();
  return true;
}

bool MainWindow::fileSaveAs()
{
  statusBar()->message(i18n("Enter a filename..."));

  QWidget *w = qworkspace->activeWindow();
  if (!w)
    return true;
  if (w->inherits("FormWindow"))
    return ((FormWindow *) w)->formFile()->saveAs();
  return false;
}

void MainWindow::fileSaveAll()
{
  fprintf(stderr, "MainWindow::fileSaveAll");
// FIXME: does nothing now
}

static bool inSaveAllTemp = false;

void MainWindow::saveAllTemp()
{
  if (inSaveAllTemp)
    return;
  inSaveAllTemp = true;
  statusBar()->message(i18n("Qt Designer is crashing. Attempting to save files..."));
  QWidgetList windows = qWorkspace()->windowList();
  QString baseName = QDir::homeDirPath() + "/.designer/saved-form-";
  int i = 1;
  for (QWidget * w = windows.first(); w; w = windows.next())
  {
    if (!w->inherits("FormWindow"))
      continue;

    QString fn = baseName + QString::number(i++) + ".kmdr";
    ((FormWindow *) w)->setFileName(fn);
    ((FormWindow *) w)->formFile()->save();
  }
  inSaveAllTemp = false;
}

void MainWindow::fileCreateTemplate()
{
  CreateTemplate dia(this, 0, true);

  int i = 0;
  for (i = 0; i < WidgetDatabase::count(); ++i)
  {
    if (WidgetDatabase::isForm(i) && WidgetDatabase::widgetGroup(i) != "Temp")
    {
      dia.listClass->insertItem(WidgetDatabase::className(i));
    }
  }
  for (i = 0; i < WidgetDatabase::count(); ++i)
  {
    if (WidgetDatabase::isContainer(i) && !WidgetDatabase::isForm(i) &&
        WidgetDatabase::className(i) != "QTabWidget" && WidgetDatabase::widgetGroup(i) != "Temp")
    {
      dia.listClass->insertItem(WidgetDatabase::className(i));
    }
  }

  QPtrList < MetaDataBase::CustomWidget > *lst = MetaDataBase::customWidgets();
  for (MetaDataBase::CustomWidget * w = lst->first(); w; w = lst->next())
  {
    if (w->isContainer)
      dia.listClass->insertItem(w->className);
  }

  dia.editName->setText(i18n("NewTemplate"));
  connect(dia.buttonCreate, SIGNAL(clicked()), this, SLOT(createNewTemplate()));
  dia.exec();
}

void MainWindow::createNewTemplate()
{
  CreateTemplate *dia = (CreateTemplate *) sender()->parent();
  QString fn = dia->editName->text();
  QString cn = dia->listClass->currentText();
  if (fn.isEmpty() || cn.isEmpty())
  {
    QMessageBox::information(this, i18n("Create Template"), i18n("Could not create the template"));
    return;
  }

  QStringList templRoots;
  const char *qtdir = getenv("QTDIR");
  if (qtdir)
    templRoots << qtdir;
#ifdef QT_INSTALL_PREFIX
  templRoots << QT_INSTALL_PREFIX;
#endif
#ifdef QT_INSTALL_DATA
  templRoots << QT_INSTALL_DATA;
#endif
  if (qtdir)                    //try the tools/designer directory last!
    templRoots << QString(qtdir) + "/tools/designer";
  QFile f;
  for (QStringList::Iterator it = templRoots.begin(); it != templRoots.end(); ++it)
  {
    if (QFile::exists((*it) + "/templates/"))
    {
      QString tmpfn = (*it) + "/templates/" + fn + ".kmdr";
      f.setName(tmpfn);
      if (f.open(IO_WriteOnly))
        break;
    }
  }
  if (!f.isOpen())
  {
    QMessageBox::information(this, i18n("Create Template"), i18n("Could not create the template"));
    return;
  }
  QTextStream ts(&f);

  ts << "<!DOCTYPE UI><UI>" << endl;
  ts << "<widget>" << endl;
  ts << "<class>" << cn << "</class>" << endl;
  ts << "<property stdset=\"1\">" << endl;
  ts << "    <name>name</name>" << endl;
  ts << "    <cstring>" << cn << "Form</cstring>" << endl;
  ts << "</property>" << endl;
  ts << "<property stdset=\"1\">" << endl;
  ts << "    <name>geometry</name>" << endl;
  ts << "    <rect>" << endl;
  ts << "        <width>300</width>" << endl;
  ts << "        <height>400</height>" << endl;
  ts << "    </rect>" << endl;
  ts << "</property>" << endl;
  ts << "</widget>" << endl;
  ts << "</UI>" << endl;

  dia->editName->setText(i18n("NewTemplate"));

  f.close();
}

void MainWindow::editUndo()
{
  if (formWindow())
    formWindow()->undo();
}

void MainWindow::editRedo()
{
  if (formWindow())
    formWindow()->redo();
}

void MainWindow::editCut()
{
  editCopy();
  editDelete();
}

void MainWindow::editCopy()
{
  if (formWindow())
    qApp->clipboard()->setText(formWindow()->copy());
}

void MainWindow::editPaste()
{
  if (!formWindow())
    return;

  QWidget *w = formWindow()->mainContainer();
  QWidgetList l(formWindow()->selectedWidgets());
  if (l.count() == 1)
  {
    w = l.first();
    if (WidgetFactory::layoutType(w) != WidgetFactory::NoLayout ||
        (!WidgetDatabase::isContainer(WidgetDatabase::idFromClassName(WidgetFactory::
                    classNameOf(w))) && w != formWindow()->mainContainer()))
      w = formWindow()->mainContainer();
  }

  if (w && WidgetFactory::layoutType(w) == WidgetFactory::NoLayout)
  {
    formWindow()->paste(qApp->clipboard()->text(), WidgetFactory::containerOfWidget(w));
    hierarchyView->widgetInserted(0);
    formWindow()->commandHistory()->setModified(true);
  } else
  {
    QMessageBox::information(this, i18n("Paste Error"),
        i18n("Cannot paste widgets. Designer could not find a container\n"
            "to paste into which does not contain a layout. Break the layout\n"
            "of the container you want to paste into and select this container\n"
            "and then paste again."));
  }
}

void MainWindow::editDelete()
{
  if (formWindow())
    formWindow()->deleteWidgets();
}

void MainWindow::editSelectAll()
{
  if (formWindow())
    formWindow()->selectAll();
}

void MainWindow::editLower()
{
  if (formWindow())
    formWindow()->lowerWidgets();
}

void MainWindow::editRaise()
{
  if (formWindow())
    formWindow()->raiseWidgets();
}

void MainWindow::editAdjustSize()
{
  if (formWindow())
    formWindow()->editAdjustSize();
}

void MainWindow::editLayoutHorizontal()
{
  if (layoutChilds)
    editLayoutContainerHorizontal();
  else if (layoutSelected && formWindow())
    formWindow()->layoutHorizontal();
}

void MainWindow::editLayoutVertical()
{
  if (layoutChilds)
    editLayoutContainerVertical();
  else if (layoutSelected && formWindow())
    formWindow()->layoutVertical();
}

void MainWindow::editLayoutHorizontalSplit()
{
  if (layoutChilds)
    ;                           // no way to do that
  else if (layoutSelected && formWindow())
    formWindow()->layoutHorizontalSplit();
}

void MainWindow::editLayoutVerticalSplit()
{
  if (layoutChilds)
    ;                           // no way to do that
  else if (layoutSelected && formWindow())
    formWindow()->layoutVerticalSplit();
}

void MainWindow::editLayoutGrid()
{
  if (layoutChilds)
    editLayoutContainerGrid();
  else if (layoutSelected && formWindow())
    formWindow()->layoutGrid();
}

void MainWindow::editLayoutContainerVertical()
{
  if (!formWindow())
    return;
  QWidget *w = formWindow()->mainContainer();
  QWidgetList l(formWindow()->selectedWidgets());
  if (l.count() == 1)
    w = l.first();
  if (w)
    formWindow()->layoutVerticalContainer(w);
}

void MainWindow::editLayoutContainerHorizontal()
{
  if (!formWindow())
    return;
  QWidget *w = formWindow()->mainContainer();
  QWidgetList l(formWindow()->selectedWidgets());
  if (l.count() == 1)
    w = l.first();
  if (w)
    formWindow()->layoutHorizontalContainer(w);
}

void MainWindow::editLayoutContainerGrid()
{
  if (!formWindow())
    return;
  QWidget *w = formWindow()->mainContainer();
  QWidgetList l(formWindow()->selectedWidgets());
  if (l.count() == 1)
    w = l.first();
  if (w)
    formWindow()->layoutGridContainer(w);
}


void MainWindow::editBreakLayout()
{
  if (!formWindow() || !breakLayout)
  {
    return;
  }
  QWidget *w = formWindow()->mainContainer();
  if (formWindow()->currentWidget())
    w = formWindow()->currentWidget();
  if (WidgetFactory::layoutType(w) != WidgetFactory::NoLayout ||
      w->parentWidget() && WidgetFactory::layoutType(w->parentWidget()) != WidgetFactory::NoLayout)
  {
    formWindow()->breakLayout(w);
    return;
  } 
  else
  {
    QWidgetList widgets = formWindow()->selectedWidgets();
    for (w = widgets.first(); w; w = widgets.next())
    {
      if (WidgetFactory::layoutType(w) != WidgetFactory::NoLayout ||
          w->parentWidget()
          && WidgetFactory::layoutType(w->parentWidget()) != WidgetFactory::NoLayout)
        break;
    }
    if (w)
    {
      formWindow()->breakLayout(w);
      return;
    }
  }

  w = formWindow()->mainContainer();
  if (WidgetFactory::layoutType(w) != WidgetFactory::NoLayout ||
      w->parentWidget() && WidgetFactory::layoutType(w->parentWidget()) != WidgetFactory::NoLayout)
  {
    formWindow()->breakLayout(w);
  }
}

void MainWindow::editAccels()
{
  if (formWindow())
    formWindow()->checkAccels();
}


void MainWindow::editConnections()
{
  if (!formWindow())
    return;

  statusBar()->message(i18n("Edit connections..."));
  ConnectionEditor editor(this, 0, 0, formWindow());
  editor.exec();
  statusBar()->clear();
}

void MainWindow::editFormSettings()
{
  if (!formWindow())
    return;

  statusBar()->message(i18n("Edit the current form's settings..."));
  FormSettings dlg(this, formWindow());
  dlg.exec();
  statusBar()->clear();
}

void MainWindow::editPreferences()
{
  statusBar()->message(i18n("Edit preferences..."));
  Preferences *dia = new Preferences(this, 0, true);
  prefDia = dia;
  connect(dia->helpButton, SIGNAL(clicked()), MainWindow::self, SLOT(showDialogHelp()));
  dia->buttonColor->setEditor(StyledButton::ColorEditor);
  dia->buttonPixmap->setEditor(StyledButton::PixmapEditor);
  dia->checkBoxShowGrid->setChecked(sGrid);
  dia->checkBoxGrid->setChecked(snGrid);
  dia->spinGridX->setValue(grid().x());
  dia->spinGridY->setValue(grid().y());
  dia->checkBoxWorkspace->setChecked(restoreConfig);
  dia->checkBoxBigIcons->setChecked(usesBigPixmaps());
  dia->checkBoxBigIcons->hide();  // ##### disabled for now
  dia->checkBoxTextLabels->setChecked(usesTextLabel());
  dia->buttonColor->setColor(qworkspace->backgroundColor());
  if (qworkspace->backgroundPixmap())
    dia->buttonPixmap->setPixmap(*qworkspace->backgroundPixmap());
  if (backPix)
    dia->radioPixmap->setChecked(true);
  else
    dia->radioColor->setChecked(true);
  dia->checkBoxSplash->setChecked(splashScreen);
  dia->editDocPath->setText(docPath);
  dia->checkAutoEdit->setChecked(!databaseAutoEdit);
  connect(dia->buttonDocPath, SIGNAL(clicked()), this, SLOT(chooseDocPath()));

  if (dia->exec() == QDialog::Accepted)
  {
    setSnapGrid(dia->checkBoxGrid->isChecked());
    setShowGrid(dia->checkBoxShowGrid->isChecked());
    setGrid(QPoint(dia->spinGridX->value(), dia->spinGridY->value()));
    restoreConfig = dia->checkBoxWorkspace->isChecked();
    setUsesBigPixmaps(false /*dia->checkBoxBigIcons->isChecked() */ );  // ### disable for now
    setUsesTextLabel(dia->checkBoxTextLabels->isChecked());
    if (dia->radioPixmap->isChecked() && dia->buttonPixmap->pixmap())
    {
      qworkspace->setBackgroundPixmap(*dia->buttonPixmap->pixmap());
      backPix = true;
    } else
    {
      qworkspace->setBackgroundColor(dia->buttonColor->color());
      backPix = false;
    }
    splashScreen = dia->checkBoxSplash->isChecked();
    docPath = dia->editDocPath->text();
    databaseAutoEdit = !dia->checkAutoEdit->isChecked();
  }
  delete dia;
  prefDia = 0;
  statusBar()->clear();
}

void MainWindow::editShortcuts()
{
  KKeyDialog K(false);
  K.configure(actionCollection());
}
    

void MainWindow::chooseDocPath()
{
  if (!prefDia)
    return;
  QString fn = KFileDialog::getExistingDirectory(QString::null, this);
  if (!fn.isEmpty())
    prefDia->editDocPath->setText(fn);
}


void MainWindow::toolsCustomWidget()
{
  statusBar()->message(i18n("Edit custom widgets..."));
  CustomWidgetEditor edit(this, this);
  edit.exec();
  rebuildCustomWidgetGUI();
  statusBar()->clear();
}
