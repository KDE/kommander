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

#ifndef ACTIONEDITOR_H
#define ACTIONEDITOR_H

#include "actioneditor.h"
//Added by qt3to4:
#include <QCloseEvent>

class QAction;
class FormWindow;
class ActionItem;

class ActionEditor : public ActionEditorBase
{
    Q_OBJECT

public:
    ActionEditor( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
    void setFormWindow( FormWindow *fw );
    void updateActionName( QAction *a );
    void updateActionIcon( QAction *a );
    FormWindow *form() const { return formWindow; }

protected:
    void closeEvent( QCloseEvent *e );

protected slots:
    void currentActionChanged( Q3ListViewItem * );
    void deleteAction();
    void newAction();
    void newActionGroup();
    void newDropDownActionGroup();
    void connectionsClicked();

signals:
    void hidden();

private:
    void insertChildActions( ActionItem *i );

private:
    QAction *currentAction;
    FormWindow *formWindow;

};

#endif // ACTIONEDITOR_H
