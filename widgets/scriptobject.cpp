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
//Added by qt3to4:
#include <QLabel>

/* KDE INCLUDES */
#include <kglobal.h>
#include <kiconloader.h>
#include <kicon.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "scriptobject.h"
#include "myprocess.h"
#include "specials.h"

ScriptObject::ScriptObject(QWidget *a_parent, const char *a_name)
  : QLabel(a_parent), KommanderWidget(this)
{
  QStringList states;
  this->setObjectName(a_name);
  states << "default";
  setStates(states);
  setDisplayStates(states);
  if (KommanderWidget::inEditor)
  {
   /* FIXME_ICON setPixmap(KIconLoader::global()->loadIcon("exec")); //FIXME  KIcon::NoGroup, KIcon::SizeMedium));
    setFrameStyle(Q3Frame::Box | Q3Frame::Plain);
    setLineWidth(1);
    setFixedSize(pixmap()->size());
    */
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
  KommanderWidget::setAssociatedText(a_text.split("\n"));
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
  setAssociatedText(KommanderWidget::evalAssociatedText(populationText()).split("\n"));
}

void ScriptObject::executeProcess(bool blocking)
{
  MyProcess process(this);
  process.setBlocking(blocking);
  process.run(evalAssociatedText());
}

void ScriptObject::execute()
{
  executeProcess(true);
}

bool ScriptObject::isFunctionSupported(int f)
{
  return f == DBUS::setText || f == DBUS::clear || f == DBUS::execute;
}

QString ScriptObject::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::setText:
      setAssociatedText(args[0].split("\n"));
      break;
    case DBUS::clear:
      setAssociatedText(QStringList());
      break;
    case DBUS::execute:
      executeProcess(true);
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString::null;
}

#include "scriptobject.moc"
