/***************************************************************************
                         combobox.cpp - Combobox widget 
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

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>

/* OTHER INCLUDES */
#include <specials.h>
#include "combobox.h"

ComboBox::ComboBox(QWidget *a_parent, const char *a_name)
  : KComboBox(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);

  connect(this, SIGNAL(activated(int)), this, SLOT(emitWidgetTextChanged(int)));
}

ComboBox::~ComboBox()
{
}

QString ComboBox::currentState() const
{
  return QString("default");
}

bool ComboBox::isKommanderWidget() const
{
  return true;
}

QStringList ComboBox::associatedText() const
{
  return KommanderWidget::associatedText();
}

void ComboBox::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void ComboBox::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString ComboBox::populationText() const
{
  return KommanderWidget::populationText();
}

void ComboBox::populate()
{
  setWidgetText(KommanderWidget::evalAssociatedText( populationText()));
}

void ComboBox::setWidgetText(const QString& a_text)
{
  clear();
  insertStringList(QStringList::split("\n", a_text));
  emit widgetTextChanged(a_text);
}

void ComboBox::emitWidgetTextChanged(int a_index)
{
  emit widgetTextChanged(text(a_index));
}

void ComboBox::showEvent(QShowEvent *e)
{
    QComboBox::showEvent( e );
    emit widgetOpened();
}

void ComboBox::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();  
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}


bool ComboBox::isFunctionSupported(int f)
{
  return f == DCOP::text || f == DCOP::selection || f == DCOP::setSelection ||
      f == DCOP::currentItem || f == DCOP::setCurrentItem || f == DCOP::item || 
      f == DCOP::removeItem || f == DCOP::insertItem || f == DCOP::insertItems ||
      f == DCOP::addUniqueItem || f == DCOP::clear || f == DCOP::count || f == DCOP::setEditable;
}

QString ComboBox::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::text:
      return currentText();
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    case DCOP::selection:
      return currentText();
    case DCOP::currentItem:
      return QString::number(currentItem());
    case DCOP::setCurrentItem:
      setCurrentItem(args[0].toUInt());
      break;
    case DCOP::item:
    {
      int i = args[0].toInt();
      if (i >= 0 && i < count()) 
        return text(i);
      break;
    }
    case DCOP::removeItem:
      removeItem(args[0].toInt());
      break;
    case DCOP::insertItem:
      insertItem(args[0], args[1].toInt());
      break;
    case DCOP::insertItems:
      insertStringList(QStringList::split("\n", args[0]), args[1].toInt());
      break;
    case DCOP::addUniqueItem:
      for (int i=0; i<count(); i++)
        if (text(i) == args[0])
          return QString();
      insertItem(args[0]);
      break;
    case DCOP::clear:
      clear();
      break;
    case DCOP::count:
      return QString::number(count());
    case DCOP::setSelection:
    {
      for (int i = 0; i < count(); i++)
        if (text(i) == args[0])
        {
          setCurrentItem(i);
          break;
        }
      break;
    }
    case DCOP::setEditable:
      setEditable(args[0] != "false" && args[0] != "0");
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}

#include "combobox.moc"
