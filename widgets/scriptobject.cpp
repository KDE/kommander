/***************************************************************************
                          scriptobject.cpp - Widget for holding scripts 
                             -------------------
    copyright            : (C) 2002 Marc Britton <consume@optusnet.com.au>
		           (C) 2003-2004 Michal Rudolf <mrudolf@kdewebdev.org>
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


/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "scriptobject.h"
#include "myprocess.h"
#include "specials.h"

ScriptObject::ScriptObject(QWidget *a_parent, const char *a_name)
  : QLabel(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  if (KommanderWidget::inEditor)
  {
    setPixmap(KGlobal::iconLoader()->loadIcon("exec", KIcon::NoGroup, KIcon::SizeMedium));
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

QString ScriptObject::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::setText:
      setAssociatedText(args[0]);
      break;
    case DCOP::clear:
      setAssociatedText(QString::null);
      break;
    default:
      break;
  }
  return QString::null;
}

#include "scriptobject.moc"
