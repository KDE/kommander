/***************************************************************************
                          ScriptObject.h - Widget for holding scripts 
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
 **************************************************************************/
#ifndef _HAVE_SCRIPTOBJECT_H_
#define _HAVE_SCRIPTOBJECT_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qlabel.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QShowEvent;
class ScriptObject : public QLabel, public KommanderWidget
{
  Q_OBJECT

  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
  Q_PROPERTY(QString script READ script WRITE setWidgetText DESIGNABLE false)
public:
  ScriptObject(QWidget *a_parent, const char *a_name);
  ~ScriptObject();

  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  
  virtual QString handleDCOP(int function, const QStringList& args);
public slots:
  virtual void setWidgetText(const QString &);
  virtual void populate();
  virtual void execute();
signals:
  void widgetOpened();
  void widgetTextChanged(const QString &);

protected:
};

#endif
