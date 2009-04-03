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
#include <qlineedit.h>
#include <qstringlist.h>
#include <qevent.h>

/* OTHER INCLUDES */
#include <specials.h>
#include "textedit.h"
#include <klocale.h>
#include <kommanderplugin.h>

enum Functions {
  FirstFunction = 450, //CHANGE THIS NUMBER TO AN UNIQUE ONE!!!
  TE_isModified,
  TE_selectText,
  TE_paragraphs,
  TE_length,
//  TE_getCursorPosition,
  TE_paragraphLength,
  TE_linesOfParagraph,
  TE_findText,
  TE_VAsuperScript,
  TE_VAnormalScript,
  LastFunction
};

TextEdit::TextEdit(QWidget * a_parent, const char *a_name):KTextEdit(a_parent, a_name),
KommanderWidget((QObject *) this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);

  connect(this, SIGNAL(textChanged()), this, SLOT(setTextChanged()));

  KommanderPlugin::setDefaultGroup(Group::DCOP);
  KommanderPlugin::registerFunction(TE_isModified, "isModified(QString widget)",  i18n("see if widget has been modified."), 1);
  KommanderPlugin::registerFunction(TE_selectText, "selectText(QString widget, int paraFrom, int indexFrom, int paraTo, int indexTo)",  i18n("Select a block of text using the paragraph number and character index of the line. You can use the cursorPositionChanged(int, int) signal to get this data in real time into a script."), 5);
  KommanderPlugin::registerFunction(TE_findText, "findText(QString widget, QString Text, bool Case-Sensitive, bool Forward)",  i18n("Search for text from the cursor or a specified position. You can specifiy case sensitive search and forward or backward."), 5);
//  KommanderPlugin::registerFunction(TE_findText, "findText(QString widget, QString Text, bool Case-Sensitive, bool Forward, int Paragraph, int Index)",  i18n("Search for text from the cursor or a specified position. You can specifiy case sensitive search and forward or backward."), 5, 7);
  KommanderPlugin::registerFunction(TE_paragraphs, "paragraphs(QString widget)",  i18n("Get the number of paragraphs in the widget."), 1);
  KommanderPlugin::registerFunction(TE_length, "length(QString widget)",  i18n("Get the total length of all text."), 1);
//  KommanderPlugin::registerFunction(TE_getCursorPosition, "getCursorPosition(QString widget)",  i18n("Get the cursor postion in the form of paragraph and postion integers."), 1);
  KommanderPlugin::registerFunction(TE_paragraphLength, "paragraphLength(QString widget, int Paragraph)",  i18n("Get the length of the paragraph."), 2);
  KommanderPlugin::registerFunction(TE_linesOfParagraph, "linesOfParagraph(QString widget, int Paragraph)",  i18n("Get the number of lines in the paragraph."), 2);
  KommanderPlugin::registerFunction(TE_VAsuperScript, "setSuperScript(QString widget)",  i18n("Use to set superscript."), 1);
  KommanderPlugin::registerFunction(TE_VAnormalScript, "setNormalScript(QString widget)",  i18n("Use to revert from superscript to normal script."), 1);
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
  emit widgetTextChanged(text());
}

void TextEdit::setTextChanged()
{
  emit widgetTextChanged(text());
}

void TextEdit::focusOutEvent( QFocusEvent * e)
{
  QTextEdit::focusOutEvent(e);
  emit lostFocus();
}

void TextEdit::focusInEvent( QFocusEvent * e)
{
  QTextEdit::focusInEvent(e);
  emit gotFocus();
}

void TextEdit::showEvent(QShowEvent * e)
{
  QTextEdit::showEvent(e);
  emit widgetOpened();
}

void TextEdit::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

bool TextEdit::isFunctionSupported(int f)
{
  return f == DCOP::text || f == DCOP::setText || f == DCOP::selection || f == DCOP::setSelection || f == DCOP::clear || f == DCOP::setEditable || f == DCOP::geometry || f == DCOP::hasFocus || (f >= FirstFunction && f <= LastFunction);
}

QString TextEdit::handleDCOP(int function, const QStringList& args)
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
      insert(args[0]);
      break;
    case DCOP::clear:
      setWidgetText(QString());
      break;
    case DCOP::setEditable:
      setReadOnly(args[0] == "false" || args[0] == "0");
      break;
    case TE_isModified:
      return isModified() ? "1" : "0";
      break;
    case TE_selectText:
      QTextEdit::setSelection(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt());
      break;
    case TE_length:
      return QString::number(QTextEdit::length() );
      break;
    /*case TE_getCursorPosition:
      return QString::number(QTextEdit::getCursorPosition() );
      break;*/
    case TE_paragraphLength:
      return QString::number(QTextEdit::paragraphLength(args[0].toInt() ) );
      break;
    case TE_linesOfParagraph:
      return QString::number(QTextEdit::linesOfParagraph(args[0].toInt() ) );
      break;
    case TE_findText:
    {
//      int para = args[3].toInt();
//      int idx = args[4].toInt();
//      return QString::number(QTextEdit::find(args[0], args[1].toUInt(), false, args[2].toUInt(), para, idx ));
      return QString::number(QTextEdit::find(args[0], args[1].toUInt(), false ));
      break;
    }
    case TE_VAsuperScript:
      break;
      QTextEdit::setVerticalAlignment(AlignSuperScript);
    case TE_VAnormalScript:
      QTextEdit::setVerticalAlignment(AlignNormal);
      break;
    case DCOP::geometry:
    {
      QString geo = QString::number(this->x())+" "+QString::number(this->y())+" "+QString::number(this->width())+" "+QString::number(this->height());
      return geo;
      break;
    }
    case DCOP::hasFocus:
      return QString::number(this->hasFocus());
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}


#include "textedit.moc"
