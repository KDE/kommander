/***************************************************************************
                          textedit.cpp - Rich text viewing widget with links
                             -------------------
    copyright            : (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
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
#include <qevent.h>

/* OTHER INCLUDES */
#include <specials.h>
#include "textbrowser.h"

TextBrowser::TextBrowser(QWidget * a_parent, const char *a_name)
  : KTextBrowser(a_parent, a_name), KommanderWidget((QObject *) this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
}

QString TextBrowser::currentState() const
{
  return QString("default");
}

TextBrowser::~TextBrowser()
{
}

bool TextBrowser::isKommanderWidget() const
{
  return TRUE;
}

QStringList TextBrowser::associatedText() const
{
  return KommanderWidget::associatedText();
}

void TextBrowser::setAssociatedText(const QStringList & a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void TextBrowser::setPopulationText(const QString & a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString TextBrowser::populationText() const
{
  return KommanderWidget::populationText();
}

void TextBrowser::populate()
{
  QString txt = KommanderWidget::evalAssociatedText(populationText());
  setWidgetText(txt);
}

void TextBrowser::setWidgetText(const QString & a_text)
{
  setText(a_text);
}

void TextBrowser::showEvent(QShowEvent * e)
{
  QTextBrowser::showEvent(e);
  emit widgetOpened();
}

bool TextBrowser::isFunctionSupported(int f)
{
  return f == DCOP::text or f == DCOP::setText or f == DCOP::selection or f == DCOP::clear;
}

QString TextBrowser::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::text:
      return text();
    case DCOP::setText:
      setText(args[0]);
      break;
    case DCOP::selection:
      return selectedText();
    case DCOP::clear:
      clear();
      break;
    default:
      break;
  }
  return QString::null;
}


#include "textbrowser.moc"
