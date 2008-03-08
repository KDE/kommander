/***************************************************************************
                          scriptobject.h - Widget for holding scripts 
                             -------------------
    copyright            : (C) 2002      Marc Britton <consume@optusnet.com.au>
                           (C) 2003-2004 Michal Rudolf <mrudolf@kdewebdev.org>
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#ifndef _HAVE_SCRIPTOBJECT_H_
#define _HAVE_SCRIPTOBJECT_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qlabel.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "kommander_export.h"

class KOMMANDER_EXPORT ScriptObject : public QLabel, public KommanderWidget
{
  Q_OBJECT

  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
      
public:
  ScriptObject(QWidget *a_parent, const char *a_name);
  ~ScriptObject();

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
  virtual void setWidgetText(const QString &);
  virtual void populate();
  virtual void execute();
signals:
  void widgetOpened();

protected:
  virtual void executeProcess(bool blocking);
};

#endif
