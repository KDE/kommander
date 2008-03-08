/***************************************************************************
                          textedit.cpp - Rich text editing widget 
                             -------------------
    copyright            : (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
                           (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
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
#include <qlayout.h>
#include <qstringlist.h>
#include <qevent.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <specials.h>
#include "textedit.h"

TextEdit::TextEdit(QWidget * a_parent, const char *a_name):KTextEdit(a_parent),
KommanderWidget((QObject *) this)
{
  setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);

  connect(this, SIGNAL(textChanged()), this, SLOT(setTextChanged()));
}

QString TextEdit::currentState() const
{
  return QString("default");
}

TextEdit::~TextEdit()
{
}

bool TextEdit::isKommanderWidget() const
{
  return true;
}

QStringList TextEdit::associatedText() const
{
  return KommanderWidget::associatedText();
}

void TextEdit::setAssociatedText(const QStringList & a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void TextEdit::setPopulationText(const QString & a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString TextEdit::populationText() const
{
  return KommanderWidget::populationText();
}

void TextEdit::populate()
{
  setWidgetText(KommanderWidget::evalAssociatedText(populationText()));
}

void TextEdit::setWidgetText(const QString & a_text)
{
  setText(a_text);
  emit widgetTextChanged(toPlainText());
}

void TextEdit::setTextChanged()
{
  emit widgetTextChanged(toPlainText());
}

void TextEdit::showEvent(QShowEvent * e)
{
  KTextEdit::showEvent(e);
  emit widgetOpened();
}

bool TextEdit::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::setText || f == DBUS::selection || f == DBUS::setSelection || f == DBUS::clear;
}

QString TextEdit::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
      return toPlainText();
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    case DBUS::selection:
      return textCursor().selectedText();
    case DBUS::setSelection:
      insertPlainText(args[0]);
      break;
    case DBUS::clear:
      setWidgetText("");
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}

#include "textedit.moc"
