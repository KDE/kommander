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

#ifndef EDITSLOTSIMPL_H
#define EDITSLOTSIMPL_H

#include "editslots.h"
#include <qmap.h>

class FormWindow;
class QListViewItem;

class EditSlots : public EditSlotsBase
{
    Q_OBJECT

public:
    EditSlots( QWidget *parent, FormWindow *fw );

    void setCurrentSlot( const QString &slot );
#ifndef KOMMANDER
    static void removeSlotFromCode( const QString &slot, FormWindow *formWindow );
#endif
    void slotAdd( const QString& access = QString::null );
    void slotAdd() { slotAdd( "public" ); }

protected slots:
    void okClicked();
    void slotRemove();
    void currentItemChanged( QListViewItem * );
    void currentTextChanged( const QString &txt );
    void currentSpecifierChanged( const QString &s);
    void currentAccessChanged( const QString &a );
    void currentTypeChanged( const QString &type );

private:

private:
    FormWindow *formWindow;
    QMap<QListViewItem*, QString> oldSlotNames;
    QStringList removedSlots;

};

#endif
