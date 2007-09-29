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

/* QT INCLUDES */
#include <qdialog.h>
#include <qevent.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>

/* OTHER INCLUDES */
#include <specials.h>
#include "closebutton.h"

CloseButton::CloseButton(QWidget* a_parent, const char* a_name)
  : KPushButton(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  setWriteStdout(true);

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
    KShellProcess *process = new KShellProcess("/bin/sh");
    *process << at;

    connect(process, SIGNAL(processExited(KProcess *)), SLOT(endProcess(KProcess *)));
    connect(process, SIGNAL(receivedStdout(KProcess *, char *, int)), SLOT(appendOutput(KProcess *,
                char *, int)));
    connect(process, SIGNAL(receivedStderr(KProcess *, char *, int)), SLOT(appendOutput(KProcess *,
                char *, int)));

    if (!process->start(KProcess::Block, KProcess::Stdout))
    {
      KMessageBox::error(this, i18n("Failed to start shell process."));
      endProcess(process);
      return;
    }
  } else
    endProcess(0);
}

void CloseButton::appendOutput(KProcess *, char *a_buffer, int a_len)
{
  char *buffer = new char[a_len + 1];
  buffer[a_len] = 0;
  for (int i = 0; i < a_len; ++i)
    buffer[i] = a_buffer[i];

  QString bufferString(buffer);
  m_output += bufferString;
  if (writeStdout())
  {
    fputs(buffer, stdout);
    fflush(stdout);
  }
  delete[] buffer;
}

void CloseButton::endProcess(KProcess *a_process)
{
  emit widgetTextChanged(m_output);
  m_output = "";
  delete a_process;
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
  return f == DCOP::text || f == DCOP::setText;
}

QString CloseButton::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::text:
      return m_output;
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}

#include "closebutton.moc"
