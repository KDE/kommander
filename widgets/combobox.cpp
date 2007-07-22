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
//Added by qt3to4:
#include <QShowEvent>

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

bool ComboBox::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::selection || f == DBUS::setSelection ||
      f == DBUS::currentItem || f == DBUS::setCurrentItem || f == DBUS::item || 
      f == DBUS::removeItem || f == DBUS::insertItem || f == DBUS::insertItems ||
      f == DBUS::addUniqueItem || f == DBUS::clear || f == DBUS::count;
}

QString ComboBox::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
      return currentText();
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    case DBUS::selection:
      return currentText();
    case DBUS::currentItem:
      return QString::number(currentItem());
    case DBUS::setCurrentItem:
      setCurrentItem(args[0].toUInt());
      break;
    case DBUS::item:
    {
      int i = args[0].toInt();
      if (i >= 0 && i < count()) 
        return text(i);
      break;
    }
    case DBUS::removeItem:
      removeItem(args[0].toInt());
      break;
    case DBUS::insertItem:
      insertItem(args[0], args[1].toInt());
      break;
    case DBUS::insertItems:
      insertStringList(QStringList::split("\n", args[0]), args[1].toInt());
      break;
    case DBUS::addUniqueItem:
      for (int i=0; i<count(); i++)
        if (text(i) == args[0])
          return QString::null;
      insertItem(args[0]);
      break;
    case DBUS::clear:
      clear();
      break;
    case DBUS::count:
      return QString::number(count());
    case DBUS::setSelection:
    {
      for (int i = 0; i < count(); i++)
        if (text(i) == args[0])
        {
          setCurrentItem(i);
          break;
        }
      break;
    }
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString::null;
}

#include "combobox.moc"
