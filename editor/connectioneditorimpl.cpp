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

#include "connectioneditorimpl.h"
#include "metadatabase.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "command.h"
#include "widgetfactory.h"
#include "editslotsimpl.h"
#ifndef KOMMANDER
#include "project.h"
#endif
#include "pixmapchooser.h"

#include <qmetaobject.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qcombobox.h>

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

ConnectionEditor::ConnectionEditor(QWidget * parent, QObject * sndr, QObject * rcvr, 
    FormWindow * fw):ConnectionEditorBase(parent, 0, TRUE), formWindow
    (fw)
{
  connect(helpButton, SIGNAL(clicked()), MainWindow::self, SLOT(showDialogHelp()));
  if (rcvr == formWindow)
    rcvr = formWindow->mainContainer();
  if (sndr == formWindow)
    sndr = formWindow->mainContainer();
  sender = sndr;
  receiver = rcvr;

  QStrList sigs = sender->metaObject()->signalNames(TRUE);
  sigs.remove("destroyed()");
  sigs.remove("destroyed(QObject*)");
  sigs.remove("accessibilityChanged(int)");
  sigs.remove("accessibilityChanged(int,int)");
  signalBox->insertStrList(sigs);

  if (sender->inherits("CustomWidget"))
  {
    MetaDataBase::CustomWidget * w = ((CustomWidget *) sender)->customWidget();
    for (QValueList < QCString >::Iterator it = w->lstSignals.begin(); it != w->lstSignals.end();
        ++it)
      signalBox->insertItem(QString(*it));
  }

  if (sender == fw->mainContainer())
  {
    QStringList extra = MetaDataBase::signalList(fw);
    if (!extra.isEmpty())
      signalBox->insertStringList(extra);
  }

  labelSignal->setText(i18n("Signals (%1):").arg(sender->name()));

  QPtrDictIterator < QWidget > it(*formWindow->widgets());
  QStringList lst;
  bool includeMainContainer = TRUE;
#ifndef KOMMANDER
  if (formWindow->project())
  {
    LanguageInterface *iface = MetaDataBase::languageInterface(formWindow->project()->language());
    if (iface && !iface->supports(LanguageInterface::ConnectionsToCustomSlots))
      includeMainContainer = FALSE;
  }
#endif

  while (it.current())
  {
    if (lst.find(it.current()->name()) != lst.end())
    {
      ++it;
      continue;
    }
    lst << it.current()->name();
    if (it.current()->isVisibleTo(this) &&
        !it.current()->inherits("QLayoutWidget") &&
        !it.current()->inherits("Spacer") &&
        qstrcmp(it.current()->name(), "central widget") != 0 &&
        (includeMainContainer || !formWindow->isMainContainer(it.current())))
    {
      if (!includeMainContainer && formWindow->isMainContainer(receiver))
        receiver = it.current();
      comboReceiver->insertItem(it.current()->name());
    }
    ++it;
  }
  comboReceiver->listBox()->sort();
  for (int i = 0; i < comboReceiver->count(); ++i)
  {
    if (comboReceiver->text(i) == QString(receiver->name()))
    {
      comboReceiver->setCurrentItem(i);
      break;
    }
  }

  signalBox->setCurrentItem(signalBox->firstItem());

  fillConnectionsList();
  connectButton->setEnabled(FALSE);
  buttonAddSlot->setEnabled(receiver == fw->mainContainer());
}

ConnectionEditor::~ConnectionEditor()
{
}

static void insertUnique(const QString & item, QStringList & lst)
{
  if (lst.find(item) == lst.end())
    lst << item;
}

void ConnectionEditor::signalChanged()
{
  updateConnectButtonState();
  QCString signal = signalBox->currentText().latin1();
  if (!signal.data())
    return;
  signal = MetaDataBase::normalizeSlot(signal);
  slotBox->clear();
  if (signalBox->currentText().isEmpty())
    return;
  int n = receiver->metaObject()->numSlots(TRUE);
  QStringList slts;
  for (int i = 0; i < n; ++i)
  {
    // accept only public slots. For the form window, also accept protected slots
    const QMetaData *md = receiver->metaObject()->slot(i, TRUE);
    if (((receiver->metaObject()->slot(i, TRUE)->access == QMetaData::Public) ||
            (formWindow->isMainContainer((QWidget *) receiver) &&
                receiver->metaObject()->slot(i, TRUE)->access == QMetaData::Protected)) &&
        !ignoreSlot(md->name) && checkConnectArgs(signal.data(), receiver, md->name))
      insertUnique(md->name, slts);
  }

#ifndef KOMMANDER
#ifndef KOMMANDER
  LanguageInterface *iface = MetaDataBase::languageInterface(formWindow->project()->language());
#else
  LanguageInterface *iface = MetaDataBase::languageInterface("C++");
#endif
  if (!iface || iface->supports(LanguageInterface::ConnectionsToCustomSlots))
  {
    if (formWindow->isMainContainer((QWidget *) receiver))
    {
      QValueList < MetaDataBase::Slot > moreSlots = MetaDataBase::slotList(formWindow);
      if (!moreSlots.isEmpty())
      {
        for (QValueList < MetaDataBase::Slot >::Iterator it = moreSlots.begin();
            it != moreSlots.end(); ++it)
        {
          QCString s = (*it).slot;
          if (!s.data())
            continue;
          s = MetaDataBase::normalizeSlot(s);
          if (checkConnectArgs(signal.data(), receiver, s))
            insertUnique((*it).slot, slts);
        }
      }
    }
  }
#endif

  if (receiver->inherits("CustomWidget"))
  {
    MetaDataBase::CustomWidget * w = ((CustomWidget *) receiver)->customWidget();
    for (QValueList < MetaDataBase::Slot >::Iterator it = w->lstSlots.begin();
        it != w->lstSlots.end(); ++it)
    {
      QCString s = (*it).slot;
      if (!s.data())
        continue;
      s = MetaDataBase::normalizeSlot(s);
      if (checkConnectArgs(signal.data(), receiver, s))
        insertUnique((*it).slot, slts);
    }
  }

  slotBox->insertStringList(slts);

  slotsChanged();
}

bool ConnectionEditor::ignoreSlot(const char *slot) const
{
  if (qstrcmp(slot, "update()") == 0 && receiver->inherits("QDataBrowser"))
    return FALSE;

  for (int i = 0; ignore_slots[i]; i++)
  {
    if (qstrcmp(slot, ignore_slots[i]) == 0)
      return TRUE;
  }

  if (!formWindow->isMainContainer((QWidget *) receiver))
  {
    if (qstrcmp(slot, "close()") == 0)
      return TRUE;
  }

  if (qstrcmp(slot, "setFocus()") == 0)
    if (receiver->isWidgetType() && ((QWidget *) receiver)->focusPolicy() == QWidget::NoFocus)
      return TRUE;

  return FALSE;
}

void ConnectionEditor::connectClicked()
{
  if (signalBox->currentItem() == -1 || slotBox->currentItem() == -1)
    return;
  if (hasConnection(sender->name(), signalBox->currentText(), receiver->name(),
          slotBox->currentText()))
    return;
  MyConnection conn;
  conn.signal = signalBox->currentText();
  conn.slot = slotBox->currentText();
  conn.receiver = receiver;
  QListViewItem *i = new QListViewItem(connectionView);
  i->setPixmap(0, PixmapChooser::loadPixmap("connecttool.xpm"));
  i->setText(0, sender->name());
  i->setText(1, conn.signal);
  i->setText(2, receiver->name());
  i->setText(3, conn.slot);
  connectionView->setCurrentItem(i);
  connectionView->setSelected(i, TRUE);
  connections.insert(i, conn);
  connectButton->setEnabled(FALSE);
}

void ConnectionEditor::disconnectClicked()
{
  QListViewItem *i = connectionView->currentItem();
  if (!i)
    return;

  QMap < QListViewItem *, MyConnection >::Iterator it = connections.find(i);
  if (it != connections.end())
    connections.remove(it);
  delete i;
  if (connectionView->currentItem())
    connectionView->setSelected(connectionView->currentItem(), TRUE);
  connectionsChanged();
}

void ConnectionEditor::okClicked()
{
  MacroCommand *rmConn = 0, *addConn = 0;
  QString n =
      i18n("Connect/Disconnect the signals and slots of '%1' and '%2'").arg(sender->name()).
      arg(receiver->name());
  QValueList < MetaDataBase::Connection >::Iterator cit;
  if (!oldConnections.isEmpty())
  {
    QPtrList < Command > commands;
    for (cit = oldConnections.begin(); cit != oldConnections.end(); ++cit)
    {
      commands.append(new RemoveConnectionCommand(i18n("Remove Connection"), formWindow, *cit));
    }
    rmConn = new MacroCommand(i18n("Remove Connections"), formWindow, commands);
  }

  if (!connections.isEmpty())
  {
    QMap < QListViewItem *, MyConnection >::Iterator it = connections.begin();
    QPtrList < Command > commands;
    for (; it != connections.end(); ++it)
    {
      MyConnection c = *it;
      MetaDataBase::Connection conn;
      conn.sender = sender;
      conn.signal = c.signal;
      conn.receiver = c.receiver;
      conn.slot = c.slot;
      commands.append(new AddConnectionCommand(i18n("Add Connection"), formWindow, conn));
    }
    addConn = new MacroCommand(i18n("Add Connections"), formWindow, commands);
  }

  if (rmConn || addConn)
  {
    QPtrList < Command > commands;
    if (rmConn)
      commands.append(rmConn);
    if (addConn)
      commands.append(addConn);
    MacroCommand *cmd = new MacroCommand(n, formWindow, commands);
    formWindow->commandHistory()->addCommand(cmd);
    cmd->execute();
  }

  accept();
}

void ConnectionEditor::cancelClicked()
{
  reject();
}

void ConnectionEditor::slotsChanged()
{
  connectButton->setEnabled(slotBox->currentItem() != -1);
  if (connectButton->isEnabled())
    updateConnectButtonState();
}

void ConnectionEditor::connectionsChanged()
{
  disconnectButton->setEnabled((connectionView->currentItem() != 0));
}

void ConnectionEditor::addSlotClicked()
{
  EditSlots dlg(this, formWindow);
  dlg.exec();
  signalChanged();
  QListViewItemIterator it(connectionView);
  QListViewItem *i = 0;
  while ((i = it.current()))
  {
    ++it;
    if (!MetaDataBase::hasSlot(formWindow, MetaDataBase::normalizeSlot(i->text(3)).latin1()))
      delete i;
  }
}

void ConnectionEditor::receiverChanged(const QString & s)
{
  QPtrDictIterator < QWidget > it(*formWindow->widgets());
  while (it.current())
  {
    if (QString(it.current()->name()) == s)
    {
      receiver = it.current();
      break;
    }
    ++it;
  }
  signalChanged();
  buttonAddSlot->setEnabled(receiver == formWindow->mainContainer());
  connectButton->setEnabled(slotBox->currentItem() != -1);
  if (connectButton->isEnabled())
    updateConnectButtonState();
  fillConnectionsList();
}

void ConnectionEditor::updateConnectButtonState()
{
  QString snder = sender->name();
  QString rcvr = receiver->name();
  QString signal = signalBox->currentText();
  QString slot = slotBox->currentText();

  connectButton->setEnabled(!hasConnection(snder, signal, rcvr, slot));
}

bool ConnectionEditor::hasConnection(const QString & snder, const QString & signal,
    const QString & rcvr, const QString & slot)
{
  QListViewItemIterator it(connectionView);
  while (it.current())
  {
    if (it.current()->text(0) == snder &&
        it.current()->text(1) == signal &&
        it.current()->text(2) == rcvr && it.current()->text(3) == slot)
    {
      return TRUE;
    }
    ++it;
  }
  return FALSE;
}

void ConnectionEditor::fillConnectionsList()
{
  connectionView->clear();
  connections.clear();
  oldConnections = MetaDataBase::connections(formWindow);
  if (!oldConnections.isEmpty())
  {
    QValueList < MetaDataBase::Connection >::Iterator it = oldConnections.begin();
    for (; it != oldConnections.end(); ++it)
    {
      if (formWindow->isMainContainer((QWidget *) (*it).receiver) &&
          !MetaDataBase::hasSlot(formWindow, MetaDataBase::normalizeSlot((*it).slot).latin1()))
        continue;
      MetaDataBase::Connection conn = *it;
      QListViewItem *i = new QListViewItem(connectionView);
      i->setPixmap(0, PixmapChooser::loadPixmap("connecttool.xpm"));
      i->setText(0, conn.sender->name());
      i->setText(1, conn.signal);
      i->setText(2, conn.receiver->name());
      i->setText(3, conn.slot);
      MyConnection c;
      c.signal = conn.signal;
      c.slot = conn.slot;
      c.receiver = conn.receiver;
      connections.insert(i, c);
    }
  }

  connectionsChanged();
}
#include "connectioneditorimpl.moc"
