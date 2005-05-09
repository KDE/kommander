/***************************************************************************
                          groupbox.h - Groupbox widget 
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
#ifndef _HAVE_GROUPBOX_H_
#define _HAVE_GROUPBOX_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qgroupbox.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qobjectlist.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "kommander_export.h"
class QWidget;

class QShowEvent;
class KOMMANDER_EXPORT GroupBox : public QGroupBox, public KommanderWidget
{
  Q_OBJECT

  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
public:
  GroupBox(QWidget *a_parent, const char *a_name);
  ~GroupBox();

  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);

  virtual void insertChild(QObject*);
  virtual void removeChild(QObject*);
  
  virtual QString handleDCOP(int function, const QStringList& args);
  virtual bool isFunctionSupported(int function);
public slots:
  virtual void populate();
signals:
  void widgetOpened();
  void widgetTextChanged(const QString &);
protected:
  QObjectList m_childList;
  void showEvent(QShowEvent *e);
private:
};

#endif
