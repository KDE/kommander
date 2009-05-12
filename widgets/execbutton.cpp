/***************************************************************************
                          execbutton.cpp - Button that runs its text association 
                             -------------------
    copyright            : (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
                           (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* KDE INCLUDES */
#include <kapplication.h>
#include <klocale.h>
#include <kmessagebox.h>

/* QT INCLUDES */
#include <qcursor.h>
#include <qevent.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>
#include <qpopupmenu.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "execbutton.h"
#include <myprocess.h>
#include <iostream>
#include <kommanderplugin.h>

using namespace std;

enum Functions {
  FirstFunction = 260, //CHANGE THIS NUMBER TO AN UNIQUE ONE!!!
  EB_isOn,
  EB_setPopup,
  LastFunction
};

ExecButton::ExecButton(QWidget* a_parent, const char* a_name)
  : KPushButton(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  setWriteStdout(true);
  setBlockGUI(Button);
  connect(this, SIGNAL(clicked()), this, SLOT(startProcess()));
  
  KommanderPlugin::setDefaultGroup(Group::DCOP);
  KommanderPlugin::registerFunction(EB_isOn, "isOn(QString widget)",  i18n("For use only when button is togle type."), 1);
  //KommanderPlugin::registerFunction(EB_setPopup, "setPopup(QString widget, QString Menu)",  i18n("Associate a Kommander PopupMenu with this ExecButton."), 2);
}

ExecButton::~ExecButton()
{
}

QString ExecButton::currentState() const
{
  return QString("default");
}

bool ExecButton::isKommanderWidget() const
{
  return true;
}

QStringList ExecButton::associatedText() const
{
  return KommanderWidget::associatedText();
}

void ExecButton::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void ExecButton::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString ExecButton::populationText() const
{
  return KommanderWidget::populationText();
}

void ExecButton::populate()
{
  QString txt = KommanderWidget::evalAssociatedText(populationText());
  setWidgetText(txt);
}

void ExecButton::setWidgetText(const QString& a_text)
{
  setText(a_text);
  emit widgetTextChanged(a_text);
}

void ExecButton::startProcess()
{
  QString at = evalAssociatedText().stripWhiteSpace();
  bool enabledStatus = isEnabled();
  if (m_blockGUI != None)
    setEnabled(false);
  if (m_blockGUI == GUI)
    KApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  MyProcess* process = new MyProcess(this);
  process->setBlocking(m_blockGUI == GUI);
  connect(process, SIGNAL(processExited(MyProcess*)), SLOT(processExited(MyProcess*)));
  m_output = process->run(at);
  if (m_blockGUI == GUI)
  {
    KApplication::restoreOverrideCursor();
    if (writeStdout())
      cout << m_output << flush;
  }
  setEnabled(enabledStatus);
}


bool ExecButton::writeStdout() const
{
  return m_writeStdout;
}

void ExecButton::setWriteStdout(bool a_enable)
{
  m_writeStdout = a_enable;
}

void ExecButton::setBlockGUI(Blocking a_enable)
{
  m_blockGUI = a_enable;
}

ExecButton::Blocking ExecButton::blockGUI() const
{
  return m_blockGUI;
}

void ExecButton::processExited(MyProcess* p)
{
  if (blockGUI() != None)
    setEnabled(true);
  if (p)
  {
    m_output = p->output();
    if (writeStdout())
      cout << m_output << flush;
    delete p;
  }
}

void ExecButton::showEvent(QShowEvent* e)
{
  KPushButton::showEvent(e);
  emit widgetOpened();
}

void ExecButton::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

bool ExecButton::isFunctionSupported(int f)
{
  return f == DCOP::text || f == DCOP::setText || f == DCOP::execute || f == DCOP::geometry || f == DCOP::getBackgroundColor || f == DCOP::setBackgroundColor || (f >= FirstFunction && f <= LastFunction);
}

QString ExecButton::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::text:
      return m_output;
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    case DCOP::execute:
      startProcess();
      break;
    case EB_isOn:
      return QString::number(this->isOn() );
      break;
    case EB_setPopup:
    {
      QPopupMenu *popup = dynamic_cast<QPopupMenu*>(widgetByName(args[0]));
      this->setPopup(popup);
      break;
    }
    case DCOP::geometry:
    {
      QString geo = QString::number(this->x())+" "+QString::number(this->y())+" "+QString::number(this->width())+" "+QString::number(this->height());
      return geo;
      break;
    }
    case DCOP::getBackgroundColor:
      return this->paletteBackgroundColor().name();
      break;
    case DCOP::setBackgroundColor:
    {
      QColor color;
      color.setNamedColor(args[0]);
      this->setPaletteBackgroundColor(color);
      break;
    }
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}


#include "execbutton.moc"
