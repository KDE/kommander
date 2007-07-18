/**********************************************************************
 This file is based on Qt Designer, Copyright (C) 2000 Trolltech AS.  All rights reserved.

 This file may be distributed and/or modified under the terms of the
 GNU General Public License version 2 as published by the Free Software
 Foundation and appearing in the file LICENSE.GPL included in the
 packaging of this file.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

 See http://www.trolltech.com/gpl/ for GPL licensing information.

 Modified for Kommander:
  (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>

**********************************************************************/

#include "connectioneditorimpl.h"
#include "metadatabase.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "command.h"
#include "widgetfactory.h"
#include "pixmapchooser.h"
#include <qmetaobject.h>
#include <qlabel.h>
#include <qregexp.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3StrList>
#include <Q3PtrList>

#include <k3listbox.h>
#include <k3listview.h>
#include <kpushbutton.h>
#include <kcombobox.h>
#include <klocale.h>

static const char *const ignore_slots[] = {
  "destroyed()",
  "setCaption(const QString&)",
  "setIcon(const QPixmap&)",
  "setIconText(const QString&)",
  "setMouseTracking(bool)",
  "clearFocus()",
  "setUpdatesEnabled(bool)",
  "update()",
  "update(int,int,int,int)",
  "update(const QRect&)",
  "repaint()",
  "repaint(bool)",
  "repaint(int,int,int,int,bool)",
  "repaint(const QRect&,bool)",
  "repaint(const QRegion&,bool)",
  "repaint(int,int,int,int)",
  "repaint(const QRect&)",
  "repaint(const QRegion&)",
  "show()",
  "hide()",
  "iconify()",
  "showMinimized()",
  "showMaximized()",
  "showFullScreen()",
  "showNormal()",
  "polish()",
  "constPolish()",
  "raise()",
  "lower()",
  "stackUnder(QWidget*)",
  "move(int,int)",
  "move(const QPoint&)",
  "resize(int,int)",
  "resize(const QSize&)",
  "setGeometry(int,int,int,int)",
  "setGeometry(const QRect&)",
  "focusProxyDestroyed()",
  "showExtension(bool)",
  "setUpLayout()",
  "showDockMenu(const QPoint&)",
  "init()",
  "destroy()",
  "deleteLater()",
  0
};

static const char *const ignore_signals[] = {
  "destroyed()",
  "destroyed(QObject*)",
  "accessibilityChanged(int)",
  "accessibilityChanged(int,int)",
  0
};


ConnectionEditor::ConnectionEditor(QWidget* parent, QObject* sndr, QObject* rcvr, FormWindow* fw)
  : ConnectionEditorBase(parent, 0, true), m_formWindow(fw)
{
  if (!rcvr || rcvr == m_formWindow)
    rcvr = m_formWindow->mainContainer();
  if (!sndr || sndr == m_formWindow)
    sndr = m_formWindow->mainContainer();
  m_sender = sndr;
  m_receiver = rcvr;

  /* Create widget list */
  QStringList lst;
  lst << m_formWindow->name();
  for (Q3PtrDictIterator<QWidget> it(*m_formWindow->widgets()); it.current(); ++it)
  {
    if (it.current()->isVisibleTo(this) &&
        !it.current()->inherits("QLayoutWidget") &&
        !it.current()->inherits("Spacer") &&
        qstrcmp(it.current()->name(), "central widget") &&
        !m_formWindow->isMainContainer(it.current()) &&
        !lst.contains(it.current()->name()))
      lst << it.current()->name();
  }
  lst.sort();
  
  // Fill receiver combos with widget list    
  fillWidgetList(comboReceiver, lst, m_receiver->name());
  
  // Fill receiver combos with widget and action list    
  for (Q3PtrListIterator<QAction> it(m_formWindow->actionList()); it.current(); ++it)
    lst << it.current()->name();
  fillWidgetList(comboSender, lst, m_sender->name());
  senderChanged(m_sender->name());
  fillConnectionsList();
  updateConnectButton();
  updateDisconnectButton();
  
  // Connections
  connect(comboSender, SIGNAL(activated(const QString&)), SLOT(senderChanged(const QString&)));
  connect(comboReceiver, SIGNAL(activated(const QString&)), SLOT(receiverChanged(const QString&)));
  connect(signalBox, SIGNAL(selectionChanged()), SLOT(updateConnectButton()));
  connect(slotBox, SIGNAL(selectionChanged()), SLOT(updateConnectButton()));
  connect(connectButton, SIGNAL(clicked()), SLOT(connectClicked()));
  connect(disconnectButton, SIGNAL(clicked()), SLOT(disconnectClicked()));
  connect(okButton, SIGNAL(clicked()), SLOT(okClicked()));
  connect(cancelButton, SIGNAL(clicked()), SLOT(cancelClicked()));
  connect(signalBox, SIGNAL(doubleClicked(Q3ListBoxItem*)), SLOT(connectClicked()));
  connect(slotBox, SIGNAL(doubleClicked(Q3ListBoxItem*)), SLOT(connectClicked()));
}

ConnectionEditor::~ConnectionEditor()
{
}

bool ConnectionEditor::isSignalIgnored(const char *signal) const
{
  for (int i = 0; ignore_signals[i]; i++)
    if (!qstrcmp(signal, ignore_signals[i]))
      return true;
  return false;
}

bool ConnectionEditor::isSlotIgnored(const QMetaData* md) 
{
  if (md->access != QMetaData::Public && (md->access != QMetaData::Protected ||
      !m_formWindow->isMainContainer((QWidget*)m_receiver)))
    return true;
  for (int i = 0; ignore_slots[i]; i++)
    if (!qstrcmp(md->name, ignore_slots[i]))
      return true;
  if (!m_formWindow->isMainContainer((QWidget *)m_receiver) && !qstrcmp(md->name, "close()"))
    return true;
  if (!qstrcmp(md->name, "setFocus()") && m_receiver->isWidgetType() && 
     ((QWidget*)m_receiver)->focusPolicy() == QWidget::NoFocus)
    return true;
  for (int i = 0; i<comboSender->count(); i++)
    if (checkConnectArgs(MetaDataBase::normalizeSlot(signalBox->text(i)).toLatin1(), m_receiver, md->name))
      return false;
  return true;
}

QObject* ConnectionEditor::objectByName(const QString& s) const
{
  for (Q3PtrDictIterator <QWidget> it(*m_formWindow->widgets()); it.current(); ++it)
    if (QString(it.current()->name()) == s)
      return it.current();
  
  for (Q3PtrListIterator<QAction> it(m_formWindow->actionList()); it.current(); ++it)
    if (QString(it.current()->name()) == s)
      return it.current();
  
  return 0;
}


void ConnectionEditor::connectClicked()
{
  if (signalBox->currentItem() == -1 || slotBox->currentItem() == -1)
    return;
  if (hasConnection(m_sender->name(), signalBox->currentText(), m_receiver->name(),
          slotBox->currentText()))
    return;
  MetaDataBase::Connection conn;
  conn.sender = m_sender;
  conn.signal = signalBox->currentText();
  conn.slot = slotBox->currentText();
  conn.receiver = m_receiver;
  K3ListViewItem *i = new K3ListViewItem(connectionView, m_sender->name(), conn.signal, m_receiver->name(),
                                       conn.slot);                       
  i->setPixmap(0, PixmapChooser::loadPixmap("connecttool.xpm"));
  connectionView->setCurrentItem(i);
  connectionView->setSelected(i, true);
  m_connections.insert(i, conn);
  connectButton->setEnabled(false);
  updateDisconnectButton();
}

void ConnectionEditor::disconnectClicked()
{
  Q3ListViewItem *p_item = connectionView->currentItem();
  if (!p_item)
    return;

  QMap <Q3ListViewItem*, MetaDataBase::Connection>::Iterator it = m_connections.find(p_item);
  if (it != m_connections.end())
    m_connections.remove(it);
  delete p_item;
  if (connectionView->currentItem())
    connectionView->setSelected(connectionView->currentItem(), true);
  updateConnectButton();
  updateDisconnectButton();
}

void ConnectionEditor::okClicked()
{
  MacroCommand* rmConn = 0, *addConn = 0;
  QString n = i18n("Connect/Disconnect the signals and slots of '%1' and '%2'", m_sender->name(), 
      m_receiver->name());
  Q3ValueList <MetaDataBase::Connection>::Iterator cit;
  if (!m_oldConnections.isEmpty())
  {
    Q3PtrList <Command> commands;
    for (cit = m_oldConnections.begin(); cit != m_oldConnections.end(); ++cit)
      commands.append(new RemoveConnectionCommand(i18n("Remove Connection"), m_formWindow, *cit));
    rmConn = new MacroCommand(i18n("Remove Connections"), m_formWindow, commands);
  }
  if (!m_connections.isEmpty())
  {
    QMap<Q3ListViewItem*, MetaDataBase::Connection>::Iterator it = m_connections.begin();
    Q3PtrList<Command> commands;
    for (; it != m_connections.end(); ++it)
    {
      MetaDataBase::Connection conn = *it;
      commands.append(new AddConnectionCommand(i18n("Add Connection"), m_formWindow, conn));
    }
    addConn = new MacroCommand(i18n("Add Connections"), m_formWindow, commands);
  }

  if (rmConn || addConn)
  {
    Q3PtrList < Command > commands;
    if (rmConn)
      commands.append(rmConn);
    if (addConn)
      commands.append(addConn);
    MacroCommand *cmd = new MacroCommand(n, m_formWindow, commands);
    m_formWindow->commandHistory()->addCommand(cmd);
    cmd->execute();
  }

  accept();
}

void ConnectionEditor::cancelClicked()
{
  reject();
}

void ConnectionEditor::senderChanged(const QString& s)
{
  QObject* p_object = objectByName(s);
  if (!p_object)
    return;
  m_sender = p_object;
  Q3StrList p_sigs = m_sender->metaObject()->signalNames(true);
  signalBox->clear();
  for (QStrListIterator it(p_sigs); it.current(); ++it)
    if (!isSignalIgnored(it.current()) && !signalBox->findItem(it.current(), Qt::ExactMatch))
      signalBox->insertItem(it.current());
  if (m_sender == m_formWindow->mainContainer())
    signalBox->insertStringList(MetaDataBase::signalList(m_formWindow));
  signalBox->sort();
  signalBox->setCurrentItem(signalBox->firstItem());
  // Update slots - some may (not) have their signal equivalents now.
  receiverChanged(m_receiver->name());
}

void ConnectionEditor::receiverChanged(const QString& s)
{
  QObject* p_object = objectByName(s);
  if (!p_object)
    return;
  m_receiver = p_object;
  int n = m_receiver->metaObject()->numSlots(true);
  slotBox->clear();
  for (int i = 0; i < n; ++i)
  {
    const QMetaData* md = m_receiver->metaObject()->slot(i, true);
    if (!isSlotIgnored(md) && !slotBox->findItem(md->name, Qt::ExactMatch))
      slotBox->insertItem(md->name);
  }
  slotBox->sort();
  slotBox->setCurrentItem(slotBox->firstItem());
  updateConnectButton();
}

void ConnectionEditor::updateConnectButton()
{
  bool itemsSelected = signalBox->currentItem() != -1 && slotBox->currentItem() != -1;
  bool notConnected = !itemsSelected || !hasConnection(m_sender->name(), signalBox->currentText(),
        m_receiver->name(), slotBox->currentText());
  bool connectionAllowed = notConnected && checkConnectArgs(MetaDataBase::normalizeSlot(signalBox->currentText()).toLatin1(),
      m_receiver, MetaDataBase::normalizeSlot(slotBox->currentText()).toLatin1());
  connectButton->setEnabled(itemsSelected && notConnected && connectionAllowed);
}
  
void ConnectionEditor::updateDisconnectButton()  
{
  disconnectButton->setEnabled((connectionView->currentItem()));
}

bool ConnectionEditor::hasConnection(const QString& snder, const QString& signal,
    const QString& rcvr, const QString& slot) const
{
  for (Q3ListViewItemIterator it(connectionView); it.current(); ++it)
    if (it.current()->text(0) == snder &&
        it.current()->text(1) == signal &&
        it.current()->text(2) == rcvr && it.current()->text(3) == slot)
      return true;
  return false;
}

void ConnectionEditor::fillConnectionsList()
{
  connectionView->clear();
  m_connections.clear();
  m_oldConnections = MetaDataBase::connections(m_formWindow);
  if (!m_oldConnections.isEmpty())
  {
    Q3ValueList <MetaDataBase::Connection>::Iterator it = m_oldConnections.begin();
    for (; it != m_oldConnections.end(); ++it)
    {
      if (m_formWindow->isMainContainer((QWidget*)(*it).receiver) &&
          !MetaDataBase::hasSlot(m_formWindow, MetaDataBase::normalizeSlot((*it).slot).toLatin1()))
        continue;
      MetaDataBase::Connection conn = *it;
      K3ListViewItem *i = new K3ListViewItem(connectionView, conn.sender->name(), conn.signal, 
                                           conn.receiver->name(), conn.slot);
      i->setPixmap(0, PixmapChooser::loadPixmap("connecttool.xpm"));
      m_connections.insert(i, conn);
    }
  }
  connectionView->setCurrentItem(connectionView->firstChild());
  if (connectionView->currentItem())
    connectionView->setSelected(connectionView->currentItem(), true);
}

void ConnectionEditor::fillWidgetList(KComboBox* a_combo, const QStringList& items, const QString& defaultWidget)
{
  a_combo->insertStringList(items);
  for (int i = 0; i < a_combo->count(); ++i)
    if (a_combo->text(i) == defaultWidget)
    {
      a_combo->setCurrentItem(i);
      return;
    }
  if (a_combo->count())
    a_combo->setCurrentItem(0);
}
    
#include "connectioneditorimpl.moc"
