/***************************************************************************
                          lineedit.cpp - Lineedit widget 
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

/* QT INCLUDES */
#include <qlayout.h>
#include <qevent.h>

/* OTHER INCLUDES */
#include <specials.h>
#include "lineedit.h"

LineEdit::LineEdit(QWidget *a_parent, const char *a_name)
	: KLineEdit(a_parent, a_name), KommanderWidget((QObject *)this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);

  connect(this, SIGNAL(textChanged(const QString &)), this,
      SIGNAL(widgetTextChanged(const QString &)));
}

void LineEdit::showEvent(QShowEvent *e)
{
  QLineEdit::showEvent(e);
  emit widgetOpened();
}

QString LineEdit::currentState() const
{
  return QString("default");
}

LineEdit::~LineEdit()
{
}

bool LineEdit::isKommanderWidget() const
{
  return true;
}

QStringList LineEdit::associatedText() const
{
  return KommanderWidget::associatedText();
}

void LineEdit::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void LineEdit::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString LineEdit::populationText() const
{
  return KommanderWidget::populationText();
}

void LineEdit::populate()
{
  QString txt = KommanderWidget::evalAssociatedText(populationText());
  setWidgetText(txt);
}

void LineEdit::setSelectedWidgetText(const QString& a_text)
{
  insert(a_text);
}

void LineEdit::setWidgetText(const QString& a_text)
{
  setText(a_text);
  emit widgetTextChanged(a_text);
}

bool LineEdit::isFunctionSupported(int f)
{
  return f == DCOP::text || f == DCOP::setText || f == DCOP::selection || f == DCOP::setSelection ||
    f == DCOP::clear;
}

QString LineEdit::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::text:
      return text();
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    case DCOP::selection:
      return selectedText();
    case DCOP::setSelection:
      setSelectedWidgetText(args[0]);
      break;
    case DCOP::clear:
      setWidgetText("");
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString::null;
}



#include "lineedit.moc"
