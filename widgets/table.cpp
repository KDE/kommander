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
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>

/* QT INCLUDES */
#include <qmetaobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qpoint.h>

/* OTHER INCLUDES */
#include "kommanderplugin.h"
#include "specials.h"
#include "table.h"

enum Functions {
  FirstFunction = 365,
  TBL_sortColumnExtra,
  TBL_keepCellVisible,
  TBL_selectCells,
  TBL_selectRow,
  TBL_selectColumn,
  TBL_setColumnReadOnly,
  TBL_setRowReadOnly,
  TBL_rowCount,
  LastFunction
};


Table::Table(QWidget *a_parent, const char *a_name)
  : QTable(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  KommanderPlugin::setDefaultGroup(Group::DCOP);
  KommanderPlugin::registerFunction(TBL_sortColumnExtra, "sortColumnExtra(QString widget, int col, bool ascending, bool wholeRows)", i18n("Sets a column to sort ascending or descending. Optionally can sort with rows intact for database use."), 2, 4);
  KommanderPlugin::registerFunction(TBL_keepCellVisible, "keepCellVisible(QString widget, int row, int col)", i18n("Scrolls the table so the cell indicated is visible."), 3);
  KommanderPlugin::registerFunction(TBL_selectCells, "selectCells(QString widget, int row, int col, int row, int col)", i18n("Select cells using the upper left and lower right cell addresses<br /><b>Not guaranteed to have KDE4 compatiblility</b>"), 5);
  KommanderPlugin::registerFunction(TBL_selectRow, "selectRow(QString widget, int row)", i18n("Select the row with the zero based index."), 2);
  KommanderPlugin::registerFunction(TBL_selectColumn, "selectColumn(QString widget, int col)", i18n("Select the column with the zero based index.<br /><b>Not guaranteed to have KDE4 compatiblility</b>"), 2);
  KommanderPlugin::registerFunction(TBL_setColumnReadOnly, "setColumnReadOnly(QString widget, int col, bool Readonly)", i18n("Set the column read only using zero based index.<br /><b>Not guaranteed to have KDE4 compatiblility</b>"), 3);
  KommanderPlugin::registerFunction(TBL_setRowReadOnly, "setRowReadOnly(QString widget, int row, bool Readonly)", i18n("Set the row read only using zero based index.<br /><b>Not guaranteed to have KDE4 compatiblility</b>"), 3);
  KommanderPlugin::registerFunction(TBL_rowCount, "rowCount(QString widget)", i18n("Returns the number of rows of the table"), 1);

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
  QTableSelection sel = selection(currentSelection());
  return QString("%1,%2,%3,%4").arg(sel.topRow()).arg(sel.leftCol()).arg(sel.bottomRow()).arg(sel.rightCol());
}


bool Table::isFunctionSupported(int f)
{
  return f == DCOP::currentColumn || f == DCOP::currentRow || f == DCOP::insertColumn || 
      f == DCOP::insertRow || f == DCOP::cellText || f == DCOP::setCellText || f == DCOP::setCellWidget || f == DCOP::cellWidget || f == DCOP::columnCount ||
      f == DCOP::removeRow || f == DCOP::removeColumn || f == DCOP::setColumnCaption ||
      f == DCOP::setRowCaption || f == DCOP::text || f == DCOP::setText || f == DCOP::selection || f == DCOP::geometry || f == DCOP::hasFocus  || (f >= FirstFunction && f <= LastFunction);
}

void Table::setCellWidget(int row, int col, const QString & _widgetName)
{
  KommanderWidget *w = widgetByName(_widgetName);
  if (w)
  {
    QWidget *widget = static_cast<QWidget*>(w->object());
    if (QTable::cellWidget(row, col) != widget)
    { 
      setCurrentCell(-1, -1); //hack to not delete the cellwidget after clicking away to another cell. 
//I don't know why it does so, but without this on a click to another cell calls endEdit, which calls
//clearCellWidget, all this before the currentChanged signal is emitted.
//this hack does ugly things once table starts scrolling - let's try to minize damage
//we should have a way to test doe cellWidget looses focus and send it right or down too
      QTable::ensureCellVisible(row, col);
      clearCellWidget(row, col);
      QTable::setCellWidget(row, col, widget);
   }
  } else
    clearCellWidget(row, col);
}

QString Table::cellWidget(int row, int col)
{

  QWidget *widget = QTable::cellWidget(row, col);
  if (widget)  
  {
    KommanderWidget *w = widgetByName(widget->name());
    if (w)
      return widget->name();
  }
  return QString();
}

void Table::setCellText(int row, int col, const QString& text)
{
  QWidget *widget = QTable::cellWidget(row, col);
  if (widget)  
  {
    KommanderWidget *w = widgetByName(widget->name());
    if (w)
      widget->reparent(parentDialog(), QPoint(0,0));
  }  
  setText(row, col, text);
  endEdit(row, col, false, false);
}

void Table::clearCellWidget(int row, int col)
{
  QTable::clearCellWidget(row, col); //just for debugging
}

void Table::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}
/*
void Table::adjustColumn(int col)
{
  if (numRows() >= col)
    QTable::adjustColumn(col);
  else
    KMessageBox::error(this, "Attempted to size nonexistant column");
}
*/
QString Table::handleDCOP(int function, const QStringList& args)
{
  switch (function) 
  {
    case DCOP::cellText:
      return text(args[0].toInt(), args[1].toInt());
    case DCOP::setCellText:
      if (numRows() >= args[0].toInt() && numCols() >+ args[1].toInt())
        setCellText(args[0].toInt(), args[1].toInt(), args[2]);
      break;
    case DCOP::setCellWidget:
      if (numRows() >= args[0].toInt() && numCols() >+ args[1].toInt())
        setCellWidget(args[0].toInt(), args[1].toInt(), args[2]);
      break;
    case DCOP::cellWidget:
      return cellWidget(args[0].toInt(), args[1].toInt());
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
    case DCOP::columnCount:
      return QString::number(numCols());
      break;
    case DCOP::removeColumn:
    if (numCols() >= args[0].toInt())
    {
      if (!args[1].toInt())
        removeColumn(args[0].toInt());
      else
      {
        int column = args[0].toInt();
        int lines = args[1].toInt();
        for (int i = 0; i < lines; i++)
          removeColumn(column);
      }
      break;
    }
    case TBL_rowCount:
      return QString::number(numRows());
      break;
    case DCOP::removeRow:
    {
      int row = args[0].toInt();
      int lines = args[1].toInt();
      for (int i = 0; i < lines; i++)
        removeRow(row);
      break;
    }
    case DCOP::setColumnCaption:
      if (numCols() >= args[0].toInt())
        horizontalHeader()->setLabel(args[0].toInt(), args[1]);
      break;
    case DCOP::setRowCaption:
      if (numRows() >= args[0].toInt())
        verticalHeader()->setLabel(args[0].toInt(), args[1]);
      break;
    case DCOP::text:
    {
      QString rows;
      for (int r = 0; r < numRows(); r++)
      {
        QString row;
        for (int c = 0; c < numCols(); c++)
        {
          if (c)
            row += '\t';
          row += text(r,c);
        }
        if (r)
          rows += '\n';
        rows += row;
      }
      return rows;
    }
    case DCOP::setText:
    {
      int r = 0, c = 0;
      setNumCols(0);
      setNumRows(0);
      QStringList rows;
      QStringList row;
      rows = QStringList::split("\n", args[0], true);
      setNumRows(rows.count());
      for (QStringList::Iterator it = rows.begin(); it != rows.end(); ++it, ++r) 
      {
        row = QStringList::split("\t", *it, true);
        if (!r)
          setNumCols(row.count());
        c = 0;
        for (QStringList::Iterator itr = row.begin(); itr != row.end(); ++itr, ++c)
          setText(r, c, *itr);
      }
      break;
    }
    case DCOP::selection:
      return selectedArea();
      break;
    case TBL_sortColumnExtra:
      QTable::sortColumn(args[0].toInt(), args[1].toInt(), args[2].toInt());
      break;
    case TBL_keepCellVisible:
      QTable::ensureCellVisible(args[0].toInt()-1, args[1].toInt()-1);
      break;
    case   TBL_selectCells:
      QTable::selectCells (args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt());
      break;
    case TBL_selectRow:
      QTable::selectRow (args[0].toInt());
      break;
    case TBL_selectColumn:
      QTable::selectColumn (args[0].toInt());
      break;
    case TBL_setColumnReadOnly:
      QTable::setColumnReadOnly (args[0].toInt(), args[1].toUInt());
      break;
    case TBL_setRowReadOnly:
      QTable::setRowReadOnly (args[0].toInt(), args[1].toUInt());
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

#include "table.moc"

