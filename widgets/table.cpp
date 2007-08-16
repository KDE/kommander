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
  : Q3Table(this), KommanderWidget(this)
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
  return true;
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

void Table::setWidgetText(const QString&)
{
}

bool Table::isFunctionSupported(int f)
{
  return f == DBUS::currentColumn || f == DBUS::currentRow || f == DBUS::insertColumn || 
      f == DBUS::insertRow || f == DBUS::cellText || f == DBUS::setCellText ||
      f == DBUS::removeRow || f == DBUS::removeColumn || f == DBUS::setColumnCaption ||
      f == DBUS::setRowCaption || f == DBUS::text || f == DBUS::setText;
}

QString Table::handleDBUS(int function, const QStringList& args)
{
  switch (function) 
  {
    case DBUS::cellText:
      return item(args[0].toInt(), args[1].toInt());
    case DBUS::setCellText:
      setItem(args[0].toInt(), args[1].toInt(), new QTableWidgetItem(args[2]));
      break;
    case DBUS::insertRow:
      if (args.count() == 1)
        insertRow(args[0].toInt());
      else
        for(int i=args[0].toInt(); i < args[1].toInt();++i)
          insertRow(i);
    case DBUS::insertColumn:
      if (args.count() == 1)
        insertColumn(args[0].toInt());
      else
        for(int i=args[0].toInt(); i < args[1].toInt();++i)
          insertColumn(i);
      break;
    case DBUS::currentColumn:
      return QString::number(currentColumn());
    case DBUS::currentRow:
      return QString::number(currentRow());
    case DBUS::removeColumn:
    {
      int column = args[0].toInt();
      int lines = args[1].toInt();
      for (int i = 0; i < lines; i++)
        removeColumn(column);
      break;
    }  
    case DBUS::removeRow:
    {
      int row = args[0].toInt();
      int lines = args[1].toInt();
      for (int i = 0; i < lines; i++)
        removeRow(row);
      break;
    }
    case DBUS::setColumnCaption:
      horizontalHeader()->setLabel(args[0].toInt(), args[1]);
      break;
    case DBUS::setRowCaption:
      verticalHeader()->setLabel(args[0].toInt(), args[1]);
      break;
    case DBUS::text:
    {
      QString row;
      QString rows;
      for (int r = 0; r < rowCount(); r++)
      {
        row = "";
        for (int c = 0; c < columnCount(); c++)
        {
          if (c)
            row += "\t";
          row += text(r,c);
        }
        if (r) 
          rows += "\n";
        rows += row;
      }
      return rows;
    }
    case DBUS::setText:
    {
      int r = 0, c = 0;
      setColumnCount(0);
      setRowCount(0);
      QStringList rows;
      QStringList row;
      rows = QStringList::split("\n", args[0]);
      setRowCount(rows.count());
      for (QStringList::Iterator it = rows.begin(); it != rows.end(); ++it, ++r) 
      {
        
        row = QStringList::split("\t", *it);
        if (!r)
          setColumCount(row.count());
        c = 0;
        for (QStringList::Iterator itr = row.begin(); itr != row.end(); ++itr, ++c)
          setItem(r, c, new QTableWidgetItem(*itr));
      }
      break;
    }
    default:
      return KommanderWidget::handleDBUS(function, args);
      
  }  
  return QString();
}

#include "table.moc"

