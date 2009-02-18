/***************************************************************************
                          scriptobject.cpp - Widget for holding scripts 
                             -------------------
    copyright            : (C) 2002 by Marc Britton
    email                : consume@optusnet.com.au
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* QT INCLUDES */
#include <qstringlist.h>
#include <qwidget.h>

/* KDE INCLUDES */
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>


/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "scriptobject.h"
#include <myprocess.h>
#include <specials.h>

ScriptObject::ScriptObject(QWidget *a_parent, const char *a_name)
  : QLabel(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  if (KommanderWidget::inEditor)
  {
    setPixmap(KGlobal::iconLoader()->loadIcon("shellscript", KIcon::NoGroup, KIcon::SizeMedium));
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(1);
    setFixedSize(pixmap()->size());
  }
  else
    setHidden(true);
}

ScriptObject::~ScriptObject()
{
}

QString ScriptObject::currentState() const
{
  return QString("default");
}

bool ScriptObject::isKommanderWidget() const
{
  return true;
}

QStringList ScriptObject::associatedText() const
{
  return KommanderWidget::associatedText();
}

void ScriptObject::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void ScriptObject::setWidgetText(const QString& a_text)
{
  KommanderWidget::setAssociatedText(a_text);
}

void ScriptObject::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString ScriptObject::populationText() const
{
  return KommanderWidget::populationText();
}

void ScriptObject::populate()
{
  setAssociatedText(KommanderWidget::evalAssociatedText(populationText()));
}

QString ScriptObject::executeProcess(bool blocking)
{
  int index = ( states().findIndex( currentState()) );
  if (index == -1)
  {
    printError(i18n("Invalid state for associated text."));
    return QString();
  }
  QString evalText = m_associatedText[index];

  if ((KommanderWidget::useInternalParser && !evalText.startsWith("#!")) || evalText.startsWith("#!kommander"))
  {
    evalAssociatedText(evalText);
    return global(widgetName() + "_RESULT");
  } else
  {
    MyProcess process(this);
    process.setBlocking(blocking);
    return process.run(evalAssociatedText(evalText));
  }
}

void ScriptObject::execute()
{
  m_params.clear();
  executeProcess(true);
}

void ScriptObject::execute(const QString& s)
{
  m_params.clear();
  m_params.append(s);
  executeProcess(true);
}

void ScriptObject::execute(const QString& s1, const QString& s2)
{
  m_params.clear();
  m_params.append(s1);
  m_params.append(s2);
  executeProcess(true);
}

void ScriptObject::execute(int i)
{
  m_params.clear();
  m_params.append(QString::number(i));
  executeProcess(true);
}

void ScriptObject::execute(int i, int j)
{
  m_params.clear();
  m_params.append(QString::number(i));
  m_params.append(QString::number(j));
  executeProcess(true);
}

void ScriptObject::execute(bool i)
{
  m_params.clear();
  m_params.append(QString::number(i));
  executeProcess(true);
}

bool ScriptObject::isFunctionSupported(int f)
{
  return f == DCOP::setText || f == DCOP::clear || f == DCOP::execute || f == DCOP::item 
      || f == DCOP::count;
}

QString ScriptObject::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::setText:
      setAssociatedText(args[0]);
      break;
    case DCOP::clear:
      setAssociatedText(QString::null);
      break;
    case DCOP::execute:
      m_params = args;
      return executeProcess(true);
      break;
    case DCOP::item:
    {
      uint index = args[0].toInt();
      return index < m_params.count() ? m_params[index] : QString::null;
    }
    case DCOP::count:
      return QString::number(m_params.count());
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}

#include "scriptobject.moc"
