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

#ifndef COMMAND_H
#define COMMAND_H

#include "metadatabase.h"
#include "layout.h"

#include <qstring.h>
#include <qrect.h>
#include <qvaluelist.h>
#include <qvariant.h>
#include <qobject.h>
#include <qlistview.h>
#include <qptrlist.h>
#include <qmap.h>

class QWizard;
class QTabWidget;
class Command;
class QWidget;
class FormWindow;
class PropertyEditor;
class QListBox;
class QIconView;
class QMultiLineEdit;
class QTable;
class QAction;
class QDesignerToolBar;
class QMainWindow;
class QDesignerPopupMenu;
class QDesignerMenuBar;

class Command : public Qt
{
public:
    Command( const QString &n, FormWindow *fw );
    virtual ~Command();

    enum Type {
	Resize,
	Insert,
	Move,
	Delete,
	SetProperty,
	LayoutHorizontal,
	LayoutVertical,
	LayoutHorizontalSplitter,
	LayoutVerticalSplitter,
	LayoutGrid,
	BreakLayout,
	Macro,
	AddTabPage,
	DeleteTabPage,
	MoveTabPage,
	AddWizardPage,
	DeleteWizardPage,
	SwapWizardPages,
	RenameWizardPage,
	AddConnection,
	RemoveConnection,
	AddSlot,
	RemoveSlot,
	Lower,
	Raise,
	Paste,
	TabOrder,
	PopulateListBox,
	PopulateIconView,
	PopulateListView,
	PopulateMultiLineEdit,
	PopulateTable,
	AddActionToToolBar,
	RemoveActionFromToolBar,
	AddActionToPopup,
	RemoveActionFromPopup,
	AddMenu,
	RemoveMenu,
	RenameMenu,
	MoveMenu,
	AddToolBar,
	RemoveToolBar
    };

    QString name() const;

    virtual void execute() = 0;
    virtual void unexecute() = 0;
    virtual Type type() const = 0;
    virtual void merge( Command *c );
    virtual bool canMerge( Command *c );

    FormWindow *formWindow() const;

private:
    QString cmdName;
    FormWindow *formWin;

};

class CommandHistory : public QObject
{
    Q_OBJECT

public:
    CommandHistory( int s );

    void addCommand( Command *cmd, bool tryCompress = FALSE );
    void undo();
    void redo();

    void emitUndoRedo();

    void setModified( bool m );
    bool isModified() const;

public slots:
    void checkCompressedCommand();

signals:
    void undoRedoChanged( bool undoAvailable, bool redoAvailable,
			  const QString &undoCmd, const QString &redoCmd );
    void modificationChanged( bool m );

private:
    QPtrList<Command> history;
    int current, steps;
    bool modified;
    int savedAt;
    Command *compressedCommand;

};

class ResizeCommand : public Command
{
public:
    ResizeCommand( const QString &n, FormWindow *fw,
		   QWidget *w, const QRect &oldr, const QRect &nr );

    void execute();
    void unexecute();
    Type type() const { return Resize; }

private:
    QWidget *widget;
    QRect oldRect, newRect;

};

class InsertCommand : public Command
{
public:
    InsertCommand( const QString &n, FormWindow *fw, QWidget *w, const QRect &g );

    void execute();
    void unexecute();
    Type type() const { return Insert; }

private:
    QWidget *widget;
    QRect geometry;

};

class MoveCommand : public Command
{
public:
    MoveCommand( const QString &n, FormWindow *fw,
		 const QWidgetList &w,
		 const QValueList<QPoint> op,
		 const QValueList<QPoint> np,
		 QWidget *opr, QWidget *npr );
    void execute();
    void unexecute();
    Type type() const { return Move; }
    void merge( Command *c );
    bool canMerge( Command *c );

private:
    QWidgetList widgets;
    QValueList<QPoint> oldPos, newPos;
    QWidget *oldParent, *newParent;

};

class DeleteCommand : public Command
{
public:
    DeleteCommand( const QString &n, FormWindow *fw,
		   const QWidgetList &w );
    void execute();
    void unexecute();
    Type type() const { return Delete; }

private:
    QWidgetList widgets;
    QMap< QWidget*, QValueList<MetaDataBase::Connection> > connections;

};

class SetPropertyCommand : public Command
{
public:
    SetPropertyCommand( const QString &n, FormWindow *fw,
			QObject *w, PropertyEditor *e,
			const QString &pn, const QVariant &ov,
			const QVariant &nv, const QString &ncut,
			const QString &ocut,
			bool reset = FALSE );

    void execute();
    void unexecute();
    Type type() const { return SetProperty; }
    void merge( Command *c );
    bool canMerge( Command *c );
    bool checkProperty();

private:
    void setProperty( const QVariant &v, const QString &currentItemText, bool select = TRUE );

    QObject *widget;
    PropertyEditor *editor;
    QString propName;
    QVariant oldValue, newValue;
    QString oldCurrentItemText, newCurrentItemText;
    bool wasChanged;
    bool isResetCommand;

};

class LayoutHorizontalCommand : public Command
{
public:
    LayoutHorizontalCommand( const QString &n, FormWindow *fw,
			     QWidget *parent, QWidget *layoutBase,
			     const QWidgetList &wl );

    void execute();
    void unexecute();
    Type type() const { return LayoutHorizontal; }

private:
    HorizontalLayout layout;

};

class LayoutHorizontalSplitCommand : public Command
{
public:
    LayoutHorizontalSplitCommand( const QString &n, FormWindow *fw,
				  QWidget *parent, QWidget *layoutBase,
				  const QWidgetList &wl );

    void execute();
    void unexecute();
    Type type() const { return LayoutHorizontalSplitter; }

private:
    HorizontalLayout layout;

};

class LayoutVerticalCommand : public Command
{
public:
    LayoutVerticalCommand( const QString &n, FormWindow *fw,
			   QWidget *parent, QWidget *layoutBase,
			   const QWidgetList &wl );

    void execute();
    void unexecute();
    Type type() const { return LayoutVertical; }

private:
    VerticalLayout layout;

};

class LayoutVerticalSplitCommand : public Command
{
public:
    LayoutVerticalSplitCommand( const QString &n, FormWindow *fw,
				QWidget *parent, QWidget *layoutBase,
				const QWidgetList &wl );

    void execute();
    void unexecute();
    Type type() const { return LayoutVerticalSplitter; }

private:
    VerticalLayout layout;

};

class LayoutGridCommand : public Command
{
public:
    LayoutGridCommand( const QString &n, FormWindow *fw,
		       QWidget *parent, QWidget *layoutBase,
		       const QWidgetList &wl, int xres, int yres );

    void execute();
    void unexecute();
    Type type() const { return LayoutGrid; }

private:
    GridLayout layout;

};

class BreakLayoutCommand : public Command
{
public:
    BreakLayoutCommand( const QString &n, FormWindow *fw,
			QWidget *layoutBase, const QWidgetList &wl );

    void execute();
    void unexecute();
    Type type() const { return BreakLayout; }

private:
    Layout *layout;
    int spacing;
    int margin;
    QWidget *lb;
    QWidgetList widgets;

};

class MacroCommand : public Command
{
public:
    MacroCommand( const QString &n, FormWindow *fw,
		  const QPtrList<Command> &cmds );

    void execute();
    void unexecute();
    Type type() const { return Macro; }

private:
    QPtrList<Command> commands;

};

class AddTabPageCommand : public Command
{
public:
    AddTabPageCommand( const QString &n, FormWindow *fw,
		       QTabWidget *tw, const QString &label );

    void execute();
    void unexecute();
    Type type() const { return AddTabPage; }

private:
    QTabWidget *tabWidget;
    int index;
    QWidget *tabPage;
    QString tabLabel;

};

class DeleteTabPageCommand : public Command
{
public:
    DeleteTabPageCommand( const QString &n, FormWindow *fw,
			  QTabWidget *tw, QWidget *page );

    void execute();
    void unexecute();
    Type type() const { return DeleteTabPage; }

private:
    QTabWidget *tabWidget;
    int index;
    QWidget *tabPage;
    QString tabLabel;

};

class MoveTabPageCommand : public Command
{
public:
    MoveTabPageCommand(const QString &n, FormWindow *fw,
		       QTabWidget *tw, QWidget *page,  const QString& label, int nIndex, int oIndex );

    void execute();
    void unexecute();
    Type type() const { return MoveTabPage; }

private:
    QTabWidget *tabWidget;
    int newIndex, oldIndex;
    QWidget *tabPage;
    QString tabLabel;

};

class AddWizardPageCommand : public Command
{
public:
    AddWizardPageCommand( const QString &n, FormWindow *fw,
			  QWizard *w, const QString &label, int index = -1, bool show = TRUE );

    void execute();
    void unexecute();
    Type type() const { return AddWizardPage; }

private:
    QWizard *wizard;
    int index;
    bool show;
    QWidget *page;
    QString pageLabel;

};

class DeleteWizardPageCommand : public Command
{
public:
    DeleteWizardPageCommand( const QString &n, FormWindow *fw,
			     QWizard *w, int index, bool show = TRUE );

    void execute();
    void unexecute();
    Type type() const { return DeleteWizardPage; }

private:
    QWizard *wizard;
    int index;
    bool show;
    QWidget *page;
    QString pageLabel;

};

class RenameWizardPageCommand : public Command
{
public:
    RenameWizardPageCommand( const QString &n, FormWindow *fw,
			     QWizard *w, int index, const QString& name );

    void execute();
    void unexecute();
    Type type() const { return RenameWizardPage; }

private:
    QWizard *wizard;
    int index;
    QWidget *page;
    QString label;

};

class SwapWizardPagesCommand : public Command
{
public:
    SwapWizardPagesCommand( const QString &n, FormWindow *fw,
			      QWizard *w, int index1, int index2 );

    void execute();
    void unexecute();
    Type type() const { return SwapWizardPages; }

private:
    QWizard *wizard;
    int index1, index2;

};

class AddConnectionCommand : public Command
{
public:
    AddConnectionCommand( const QString &name, FormWindow *fw,
			  MetaDataBase::Connection c );

    void execute();
    void unexecute();
    Type type() const { return AddConnection; }

private:
    MetaDataBase::Connection connection;

};

class RemoveConnectionCommand : public Command
{
public:
    RemoveConnectionCommand( const QString &name, FormWindow *fw,
			     MetaDataBase::Connection c );

    void execute();
    void unexecute();
    Type type() const { return RemoveConnection; }

private:
    MetaDataBase::Connection connection;

};

class AddSlotCommand : public Command
{
public:
    AddSlotCommand( const QString &name, FormWindow *fw, const QCString &s, const QString& spec, 
		    const QString &a, const QString &l, const QString &rt );

    void execute();
    void unexecute();
    Type type() const { return AddSlot; }

private:
    QCString slot;
    QString specifier;
    QString access;
    QString language;
    QString returnType;

};

class RemoveSlotCommand : public Command
{
public:
    RemoveSlotCommand( const QString &name, FormWindow *fw, const QCString &s, const QString& spec,
		       const QString &a, const QString &l, const QString &rt );

    void execute();
    void unexecute();
    Type type() const { return RemoveSlot; }

private:
    QCString slot;
    QString specifier;
    QString access;
    QString language;
    QString returnType;

};

class LowerCommand : public Command
{
public:
    LowerCommand( const QString &name, FormWindow *fw, const QWidgetList &w );

    void execute();
    void unexecute();
    Type type() const { return Lower; }

private:
    QWidgetList widgets;

};

class RaiseCommand : public Command
{
public:
    RaiseCommand( const QString &name, FormWindow *fw, const QWidgetList &w );

    void execute();
    void unexecute();
    Type type() const { return Raise; }

private:
    QWidgetList widgets;

};

class PasteCommand : public Command
{
public:
    PasteCommand( const QString &n, FormWindow *fw, const QWidgetList &w );

    void execute();
    void unexecute();
    Type type() const { return Paste; }

private:
    QWidgetList widgets;

};

class TabOrderCommand : public Command
{
public:
    TabOrderCommand( const QString &n, FormWindow *fw, const QWidgetList &ol, const QWidgetList &nl );

    void execute();
    void unexecute();
    Type type() const { return TabOrder; }
    void merge( Command *c );
    bool canMerge( Command *c );

private:
    QWidgetList oldOrder, newOrder;

};

class PopulateListBoxCommand : public Command
{
public:
    struct Item
    {
	QString text;
	QPixmap pix;
#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
	bool operator==( const Item & ) const { return FALSE; }
#endif
    };

    PopulateListBoxCommand( const QString &n, FormWindow *fw,
			    QListBox *lb, const QValueList<Item> &items );
    void execute();
    void unexecute();
    Type type() const { return PopulateListBox; }

    bool operator==( const PopulateListBoxCommand & ) const;

private:
    QValueList<Item> oldItems, newItems;
    QListBox *listbox;

};

class PopulateIconViewCommand : public Command
{
public:
    struct Item
    {
	QString text;
	QPixmap pix;
#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
	bool operator==( const Item & ) const { return FALSE; }
#endif
    };

    PopulateIconViewCommand( const QString &n, FormWindow *fw,
			    QIconView *iv, const QValueList<Item> &items );
    void execute();
    void unexecute();
    Type type() const { return PopulateIconView; }

    bool operator==( const PopulateIconViewCommand & ) const;

private:
    QValueList<Item> oldItems, newItems;
    QIconView *iconview;

};

class PopulateListViewCommand : public Command
{
public:
    PopulateListViewCommand( const QString &n, FormWindow *fw,
			     QListView *lv, QListView *from );
    void execute();
    void unexecute();
    Type type() const { return PopulateListView; }
    static void transferItems( QListView *from, QListView *to );

    bool operator==( const PopulateListViewCommand & ) const;

private:
    QListView *oldItems, *newItems;
    QListView *listview;

};

class PopulateMultiLineEditCommand : public Command
{
public:
    PopulateMultiLineEditCommand( const QString &n, FormWindow *fw,
				  QMultiLineEdit *mle, const QString &txt );
    void execute();
    void unexecute();
    Type type() const { return PopulateMultiLineEdit; }

private:
    QString newText, oldText;
    QMultiLineEdit *mlined;
    bool wasChanged;

};

class PopulateTableCommand : public Command
{
public:
    struct Row
    {
	QString text;
	QPixmap pix;
#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
	bool operator==( const Row& ) const { return FALSE; }
#endif
    };

    struct Column
    {
	QString text;
	QPixmap pix;
	QString field;
#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
	bool operator==( const Column& ) const { return FALSE; }
#endif
    };

    PopulateTableCommand( const QString &n, FormWindow *fw, QTable *t,
			  const QValueList<Row> &rows,
			  const QValueList<Column> &columns );

    void execute();
    void unexecute();
    Type type() const { return PopulateTable; }

private:
    QValueList<Row> oldRows, newRows;
    QValueList<Column> oldColumns, newColumns;
    QTable *table;

};

class AddActionToToolBarCommand : public Command
{
public:
    AddActionToToolBarCommand( const QString &n, FormWindow *fw,
			       QAction *a, QDesignerToolBar *tb, int idx );

    void execute();
    void unexecute();
    Type type() const { return AddActionToToolBar; }

private:
    QAction *action;
    QDesignerToolBar *toolBar;
    int index;

};

class RemoveActionFromToolBarCommand : public AddActionToToolBarCommand
{
public:
    RemoveActionFromToolBarCommand( const QString &n, FormWindow *fw,
				    QAction *a, QDesignerToolBar *tb, int idx )
	: AddActionToToolBarCommand( n, fw, a, tb, idx ) {}

    void execute() { AddActionToToolBarCommand::unexecute(); }
    void unexecute() { AddActionToToolBarCommand::execute(); }
    Type type() const { return RemoveActionFromToolBar; }

};

class AddActionToPopupCommand : public Command
{
public:
    AddActionToPopupCommand( const QString &n, FormWindow *fw,
			       QAction *a, QDesignerPopupMenu *p, int idx );

    void execute();
    void unexecute();
    Type type() const { return AddActionToPopup; }

private:
    QAction *action;
    QDesignerPopupMenu *popup;
    int index;

};

class RemoveActionFromPopupCommand : public AddActionToPopupCommand
{
public:
    RemoveActionFromPopupCommand( const QString &n, FormWindow *fw,
				    QAction *a, QDesignerPopupMenu *p, int idx )
	: AddActionToPopupCommand( n, fw, a, p, idx ) {}

    void execute() { AddActionToPopupCommand::unexecute(); }
    void unexecute() { AddActionToPopupCommand::execute(); }
    Type type() const { return RemoveActionFromPopup; }

};

class AddMenuCommand : public Command
{
public:
    AddMenuCommand( const QString &n, FormWindow *fw, QMainWindow *mw );

    void execute();
    void unexecute();
    Type type() const { return AddMenu; }

protected:
    QDesignerMenuBar *menuBar;
    QDesignerPopupMenu *popup;
    QMainWindow *mainWindow;
    int id;
    int index;
    QString name;

};

class RemoveMenuCommand : public AddMenuCommand
{
public:
    RemoveMenuCommand( const QString &n, FormWindow *fw, QMainWindow *mw,
		       QDesignerMenuBar *mb, QDesignerPopupMenu *p, int i, int idx, const QString &mn )
	: AddMenuCommand( n, fw, mw ) { menuBar = mb; popup = p; id = i; index = idx, name = mn; }

    void execute() { AddMenuCommand::unexecute(); }
    void unexecute() { AddMenuCommand::execute(); }
    Type type() const { return RemoveMenu; }

};

class RenameMenuCommand : public Command
{
public:
    RenameMenuCommand( const QString &n, FormWindow *fw, QDesignerMenuBar *mb,
		       int i, const QString &on, const QString &nn );

    void execute();
    void unexecute();
    Type type() const { return RenameMenu; }

private:
    QDesignerMenuBar *menuBar;
    int id;
    QString oldName, newName;

};

class MoveMenuCommand : public Command
{
public:
    MoveMenuCommand( const QString &n, FormWindow *fw, QDesignerMenuBar *mb,
		     QDesignerPopupMenu *p, int fidx, int tidx, const QString &txt );

    void execute();
    void unexecute();
    Type type() const { return MoveMenu; }

private:
    QDesignerMenuBar *menuBar;
    QDesignerPopupMenu *popup;
    int fromIdx, toIdx;
    QString text;

};

class AddToolBarCommand : public Command
{
public:
    AddToolBarCommand( const QString &n, FormWindow *fw, QMainWindow *mw );

    void execute();
    void unexecute();
    Type type() const { return AddToolBar; }

protected:
    QDesignerToolBar *toolBar;
    QMainWindow *mainWindow;

};

class RemoveToolBarCommand : public AddToolBarCommand
{
public:
    RemoveToolBarCommand( const QString &n, FormWindow *fw, QMainWindow *mw, QDesignerToolBar *tb )
	: AddToolBarCommand( n, fw, mw ) { toolBar = tb; }

    void execute() { AddToolBarCommand::unexecute(); }
    void unexecute() { AddToolBarCommand::execute(); }
    Type type() const { return RemoveToolBar; }

};

#endif
