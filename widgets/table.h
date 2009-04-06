/***************************************************************************
                          table.h - Table widget
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


#ifndef _HAVE_TABLE_H_
#define _HAVE_TABLE_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qtable.h>
#include <qobject.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QWidget;

class KOMMANDER_EXPORT Table : public QTable, public KommanderWidget
{
  Q_OBJECT

      Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
      Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
      Q_OVERRIDE(bool readOnly DESIGNABLE false)
      Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)

  public:
    Table(QWidget *a_parent, const char *a_name);
    ~Table();

    virtual bool isKommanderWidget() const;
    virtual void setAssociatedText(const QStringList&);
    virtual QStringList associatedText() const;
    virtual QString currentState() const;

    virtual QString populationText() const;
    virtual void setPopulationText(const QString&);

    virtual QString handleDCOP(int function, const QStringList& args);
    virtual bool isFunctionSupported(int function);
    virtual void clearCellWidget(int row, int col);
    virtual void columnClicked(int col);
  public slots:
    virtual void populate();
    virtual void setWidgetText(const QString &);
    //void adjustColumn(int col);
  signals:
    void contextMenuRequested(int xpos, int ypos);
    void columnHeaderClicked(int col);
  protected:
    void contextMenuEvent( QContextMenuEvent * e );
  private:
    QString selectedArea();
    QString cellWidget(int row, int col);
    void setCellWidget(int row, int col, const QString &widgetName);
    void setCellText(int row, int col, const QString &text);
};

#endif

