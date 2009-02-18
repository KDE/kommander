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

/* KDE INCLUDES */
#include <klocale.h>

/* QT INCLUDES */
#include <qstringlist.h>
#include <qevent.h>
#include <qstring.h>

/* OTHER INCLUDES */
#include "kommanderplugin.h"
#include <specials.h>
#include "textbrowser.h"
/*
enum Functions {
  FirstFunction = 420,
  TBR_scrollToAnchor,
  LastFunction
};
*/
TextBrowser::TextBrowser(QWidget * a_parent, const char *a_name)
  : KTextBrowser(a_parent, a_name), KommanderWidget((QObject *) this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
//  KommanderPlugin::setDefaultGroup(Group::DCOP);
//  KommanderPlugin::registerFunction(TBR_scrollToAnchor, "scrollToAnchor(QString widget, QString name)",i18n("Scroll to an anchor in the form of <a href name=\"#anchor\">anchor</a>"), 2, 2);
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
  return true;
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

void TextBrowser::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}


bool TextBrowser::isFunctionSupported(int f)
{
  return f == DCOP::text || f == DCOP::setText || f == DCOP::selection || f == DCOP::clear;
// || (f >= FirstFunction && f <= LastFunction);
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
//    case TBR_scrollToAnchor:
//      KTextBrowser::scrollToAnchor(args[0]);
    case DCOP::clear:
      clear();
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}


#include "textbrowser.moc"
