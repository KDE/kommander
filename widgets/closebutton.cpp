/***************************************************************************
                          closebutton.cpp - Button that when clicked, closes the dialog it's in
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
#include <kmessagebox.h>
#include <klocale.h>
#include <kprocess.h>

/* QT INCLUDES */
#include <qdialog.h>
#include <qevent.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <specials.h>
#include "closebutton.h"

CloseButton::CloseButton(QWidget* a_parent, const char* a_name)
  : KPushButton(a_parent), KommanderWidget(this)
{
  this->setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  setWriteStdout(true);
  m_process = 0;
  connect(this, SIGNAL(clicked()), this, SLOT(startProcess()));

  QObject *parent = this;
  while (parent->parent() != 0)
  {
    parent = parent->parent();
    if (parent->inherits("QDialog"))
      break;
  }
  connect(this, SIGNAL(clicked()), parent, SLOT(reject()));
}

CloseButton::~CloseButton()
{
}

QString CloseButton::currentState() const
{
  return QString("default");
}

bool CloseButton::isKommanderWidget() const
{
  return true;
}

QStringList CloseButton::associatedText() const
{
  return KommanderWidget::associatedText();
}

void CloseButton::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void CloseButton::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString CloseButton::populationText() const
{
  return KommanderWidget::populationText();
}

void CloseButton::populate()
{
  setWidgetText(KommanderWidget::evalAssociatedText(populationText()));
}

void CloseButton::setWidgetText(const QString& a_text)
{
  setText(a_text);
  emit widgetTextChanged(a_text);
}

void CloseButton::startProcess()
{
  QString at = evalAssociatedText();

  if (!at.isEmpty())
  {
    m_process = new KProcess();
    m_process->setShellCommand("/bin/sh");
    m_process->setProgram(at);
    m_process->setOutputChannelMode(KProcess::SeparateChannels);
    //should emit output and block
    connect(m_process, SIGNAL(finished(int,  QProcess::ExitStatus)), SLOT(endProcess(int,  QProcess::ExitStatus)));
    connect(m_process, SIGNAL(readyReadStandardOutput()), SLOT(appendOutput()));
    connect(m_process, SIGNAL(readyReadStandardError()), SLOT(appendError()));

    m_process->start();
    if (!m_process->waitForStarted())
    {
      KMessageBox::error(this, i18n("Failed to start shell process."));
      endProcess(m_process->exitCode(), m_process->exitStatus());
      return;
    }
  } else
    if (m_process && m_process->waitForFinished())
    { 
      KMessageBox::error(this, i18n("Shell process exited with an error."));
      endProcess(m_process->exitCode(), m_process->exitStatus());
      return;
    }
}

void CloseButton::appendOutput()
{
  QByteArray a = m_process->readAllStandardOutput();
  m_output += a;
  if (writeStdout())
  {
    // FIXME stdout << a <<flush;
  }
}

void CloseButton::appendError()
{
  QByteArray a = m_process->readAllStandardError();
  m_output += a;
  if (writeStdout())
  {
    // FIXME stderr << a <<flush;
  }
  
}


void CloseButton::endProcess(int exitCode, QProcess::ExitStatus exitStatus)
{
  Q_UNUSED(exitCode)
  Q_UNUSED(exitStatus)
  emit widgetTextChanged(m_output);
  m_output = "";
  delete m_process;
  m_process = 0;
}

bool CloseButton::writeStdout() const
{
  return m_writeStdout;
}

void CloseButton::setWriteStdout(bool a_enable)
{
  m_writeStdout = a_enable;
}

void CloseButton::showEvent(QShowEvent *e)
{
  QPushButton::showEvent(e);
  emit widgetOpened();
}

bool CloseButton::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::setText;
}

QString CloseButton::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
      return m_output;
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}

#include "closebutton.moc"
