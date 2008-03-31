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
#include <QTableWidgetItem>
#include <QContextMenuEvent>

#include <klocale.h>

/* OTHER INCLUDES */
#include "kommanderplugin.h"
#include "specials.h"
#include "table.h"

enum Functions {
  FirstFunction = 365,
  TBL_sortColumnExtra,
  TBL_keepCellVisible,
  LastFunction
};


Table::Table(QWidget *a_parent, const char *a_name)
  : QTableWidget(a_parent), KommanderWidget(this)
{
  setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  KommanderPlugin::setDefaultGroup(Group::DBUS);
  KommanderPlugin::registerFunction(TBL_sortColumnExtra, "sortColumnExtra(QString widget, int col, bool ascending, bool wholeRows)", i18n("Sets a column to sort ascending or descending. Optionally can sort with rows intact for database use. wholeRows is ignored under KDE4."), 2, 4);
  KommanderPlugin::registerFunction(TBL_keepCellVisible, "keepCellVisible(QString widget, int row, int col)", i18n("Scrolls the table so the cell indicated is visible."), 3);

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

QString Table::selectedArea()
{
  QList<QTableWidgetSelectionRange> sel = selectedRanges();
  if (sel.isEmpty())
     return "";
  return QString("%1,%2,%3,%4").arg(sel[0].topRow()).arg(sel[0].leftColumn()).arg(sel[0].bottomRow()).arg(sel[0].rightColumn());
}


bool Table::isFunctionSupported(int f)
{
  return f == DBUS::currentColumn || f == DBUS::currentRow || f == DBUS::insertColumn || 
      f == DBUS::insertRow || f == DBUS::cellText || f == DBUS::setCellText || f == DBUS::setCellWidget || f == DBUS::cellWidget ||
      f == DBUS::removeRow || f == DBUS::removeColumn || f == DBUS::setColumnCaption ||
      f == DBUS::setRowCaption || f == DBUS::text || f == DBUS::setText || f == DBUS::selection || (f >= FirstFunction && f <= LastFunction);
}

void Table::setCellWidget(int row, int col, const QString & _widgetName)
{
  KommanderWidget *w = widgetByName(_widgetName);
  if (w)
  {
    QWidget *widget = static_cast<QWidget*>(w->object());
    if (QTableWidget::cellWidget(row, col) != widget)
    { 
      setCurrentCell(-1, -1); //hack to not delete the cellwidget after clicking away to another cell. 
//I don't know why it does so, but without this on a click to another cell calls endEdit, which calls
//clearCellWidget, all this before the currentChanged signal is emitted.
      clearCellWidget(row, col);
      QTableWidget::setCellWidget(row, col, widget);
   }
  } else
    clearCellWidget(row, col);
}

QString Table::cellWidget(int row, int col)
{

  QWidget *widget = QTableWidget::cellWidget(row, col);
  if (widget)  
  {
    KommanderWidget *w = widgetByName(widget->objectName());
    if (w)
      return widget->objectName();
  }
  return QString();
}

void Table::setCellText(int row, int col, const QString& text)
{
  QWidget *widget = QTableWidget::cellWidget(row, col);
  if (widget)  
  {
    KommanderWidget *w = widgetByName(widget->objectName());
    if (w)
      widget->reparent(parentDialog(), QPoint(0,0));
  }  
  setItem(row, col, new QTableWidgetItem(text));
//FIXME??  endEdit(row, col, false, false);
}

void Table::clearCellWidget(int row, int col)
{
  QTableWidget::removeCellWidget(row, col); //just for debugging
}

void Table::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}


QString Table::handleDBUS(int function, const QStringList& args)
{
  switch (function) 
  {
    case DBUS::cellText:
      return item(args[0].toInt(), args[1].toInt())->text();
    case DBUS::setCellText:
      setItem(args[0].toInt(), args[1].toInt(), new QTableWidgetItem(args[2]));
      break;
    case DBUS::setCellWidget:
      setCellWidget(args[0].toInt(), args[1].toInt(), args[2]);
      break;
    case DBUS::cellWidget:
      return cellWidget(args[0].toInt(), args[1].toInt());
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
      horizontalHeaderItem(args[0].toInt())->setText(args[1]);
      break;
    case DBUS::setRowCaption:
      verticalHeaderItem(args[0].toInt())->setText(args[1]);
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
          row += item(r,c)->text();
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
      rows = args[0].split('\n');
      setRowCount(rows.count());
      for (QStringList::Iterator it = rows.begin(); it != rows.end(); ++it, ++r) 
      {
        
        row = (*it).split('\t');
        if (!r)
          setColumnCount(row.count());
        c = 0;
        for (QStringList::Iterator itr = row.begin(); itr != row.end(); ++itr, ++c)
          setItem(r, c, new QTableWidgetItem(*itr));
      }
      break;
    }
    case DBUS::selection:
      return selectedArea();
      break;
    case TBL_sortColumnExtra:
      sortItems(args[0].toInt(), args[1] == "0" ? Qt::AscendingOrder : Qt::DescendingOrder);
      break;
    case TBL_keepCellVisible:
      scrollToItem(item(args[0].toInt(), args[1].toInt()));
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
      
  }  
  return QString();
}

#include "table.moc"

