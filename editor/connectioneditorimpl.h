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

#ifndef CONNECTIONEDITORIMPL_H
#define  CONNECTIONEDITORIMPL_H

#include "connectioneditor.h"
#include "metadatabase.h"

#include <qmap.h>

class QListViewItem;
class FormWindow;

class ConnectionEditor : public ConnectionEditorBase
{
Q_OBJECT public:
  ConnectionEditor(QWidget* parent, QObject* sender, QObject* receiver, FormWindow* fw);
  ~ConnectionEditor();

protected slots:
  // Add current pair <signal, slot> to the list
  void connectClicked();
  // Remove current pair <signal, slot> to the list
  void disconnectClicked();
  // Lists all connections defined in current dialog
  void fillConnectionsList();
  // Lists all widgets in sender or receiver combo
  void fillWidgetList(KComboBox* a_combo, const QStringList& items, const QString& defaultWidget);
  // Sender widget was changed - update signal list
  void senderChanged(const QString & s);
  // Receiver widget was changed - update slot list
  void receiverChanged(const QString & s);
  // Enable/disable 'Connect' button when pair <signal,slot> changes
  void updateConnectButton();
  // Enable/disable 'Disconnect' button when selection changes
  void updateDisconnectButton();
  // Accept and close button
  void okClicked();
  // Cancel and close button
  void cancelClicked();
  
private:
  // Whether given signal should appear in the signal list
  bool isSignalIgnored(const char*) const;
  // Whether given slot should appear in the slot list
  bool isSlotIgnored(const QMetaData* md) const;
  // Finds widget by name
  QObject* objectByName(const QString& s);
  bool hasConnection(const QString & snder, const QString & signal,
      const QString & rcvr, const QString & slot);
  QMap <QListViewItem*, MetaDataBase::Connection> m_connections;
  QValueList <MetaDataBase::Connection> m_oldConnections;
  QObject* m_sender;
  QObject* m_receiver;
  FormWindow* m_formWindow;
};

#endif
