/***************************************************************************
                          tabwidget.h - Widget with tabs 
                             -------------------
    copyright            : (C) 2002 by Marc Britton
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
#ifndef _HAVE_TABWIDGET_H_
#define _HAVE_TABWIDGET_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qtabwidget.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <kommander_export.h>
class QShowEvent;
class KOMMANDER_EXPORT TabWidget : public QTabWidget, public KommanderWidget
{
  Q_OBJECT
  
  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
public:
  TabWidget(QWidget *, const char *, int=0);
  ~TabWidget();
  
  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  
  virtual QString handleDCOP(int function, const QStringList& args);
  virtual bool isFunctionSupported(int function);
  
public slots:
  virtual void populate();
signals:
  void widgetOpened();
  void contextMenuRequested(int xpos, int ypos);
protected:
  void showEvent(QShowEvent *e);
  void contextMenuEvent( QContextMenuEvent * e );
private:
};

#endif
