/***************************************************************************
                          table.cpp - Table widget
                             -------------------
    copyright            : (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
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
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>

/* OTHER INCLUDES */
#include <specials.h>
#include "table.h"

Table::Table(QWidget *a_parent, const char *a_name)
  : QTable(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
}

Table::~Table()
{
}

QString Table::currentState() const
{
  return QString("default");
}

bool Table::isKommanderWidget() const
{
  return TRUE;
}

QStringList Table::associatedText() const
{
  return KommanderWidget::associatedText();
}

void Table::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void Table::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString Table::populationText() const
{
  return KommanderWidget::populationText();
}

void Table::populate()
{
  setWidgetText(KommanderWidget::evalAssociatedText( populationText()));
}

void Table::setWidgetText(const QString& a_text)
{
}

bool Table::isFunctionSupported(int f)
{
  return f == DCOP::currentColumn || f == DCOP::currentRow || f == DCOP::insertColumn || 
      f == DCOP::insertRow || f == DCOP::cellText || f == DCOP::setCellText ||
      f == DCOP::removeRow || f == DCOP::removeColumn || f == DCOP::setColumnCaption ||
      f == DCOP::setRowCaption;
}

QString Table::handleDCOP(int function, const QStringList& args)
{
  switch (function) 
  {
    case DCOP::cellText:
      return text(args[0].toInt(), args[1].toInt());
    case DCOP::setCellText:
      setText(args[0].toInt(), args[1].toInt(), args[2]);
      break;
    case DCOP::insertRow:
      insertRows(args[0].toInt(), args.count() == 1 ? 1 : args[1].toInt());
      break;
    case DCOP::insertColumn:
      insertColumns(args[0].toInt(), args.count() == 1 ? 1 : args[1].toInt());
      break;
    case DCOP::currentColumn:
      return QString::number(currentColumn());
    case DCOP::currentRow:
      return QString::number(currentRow());
    case DCOP::removeColumn:
    {
      int column = args[0].toInt();
      int lines = args[1].toInt();
      for (int i = 0; i < lines; i++)
        removeColumn(column);
      break;
    }  
    case DCOP::removeRow:
    {
      int row = args[0].toInt();
      int lines = args[1].toInt();
      for (int i = 0; i < lines; i++)
        removeRow(row);
      break;
    }
    case DCOP::setColumnCaption:
      horizontalHeader()->setLabel(args[0].toInt(), args[1]);
      break;
    case DCOP::setRowCaption:
      verticalHeader()->setLabel(args[0].toInt(), args[1]);
      break;
  }  
  return QString::null;
}

#include "table.moc"

