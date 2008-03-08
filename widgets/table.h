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
#include <qtablewidget.h>
#include <qobject.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QWidget;

class Table : public QTableWidget, public KommanderWidget
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

    virtual QString handleDBUS(int function, const QStringList& args);
    using KommanderWidget::handleDBUS;
    virtual bool isFunctionSupported(int function);
  public slots:
    virtual void populate();
    virtual void setWidgetText(const QString &);
};

#endif

