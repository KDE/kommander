/***************************************************************************
                          instance.h  -  description
                             -------------------
    begin                : Tue Aug 13 2002
    copyright            : (C) 2002 by Marc Britton <consume@optushome.com.au>
                           (C) 2004 by Andras Mantia <amantia@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INSTANCE_H
#define INSTANCE_H

/* KDE INCLUDES */
/* QT INCLUDES */
#include <qstring.h>
#include <qobject.h>

/* OTHER INCLUDES */

#include "dcopkommanderif.h"

/* FORWARD DECLARATIONS */
class QWidget;
class KommanderWidget;
class QDialog;
class QFile;

/**
  *@author Marc Britton
  */

/* CLASS DEFINITION*/
class Instance : public QObject, virtual public DCOPKommanderIf
{
  Q_OBJECT
  
public: 
  Instance();
  Instance(QString, QWidget *);
  ~Instance();

//DCOP methods. The documentation is in the dcopkommanderif.h file.
  virtual void enableWidget(const QString& widgetName, bool enable);  
  virtual void changeWidgetText(const QString& widgetName, const QString& text);      
  virtual void removeListItem(const QString &widgetName, int index);
  virtual void addListItem(const QString &widgetName, const QString &item, int index);
  virtual void addListItems(const QString &widgetName, const QStringList &items, int index);
  virtual void clearList(const QString &widgetName);
  virtual void setCurrentListItem(const QString& widgetName, const QString &name);
  virtual void setCurrentTab(const QString &widgetName, int index);
  virtual void setChecked(const QString &widgetName, bool checked);
  virtual void setAssociatedText(const QString &widgetName, const QString &text);
  virtual QStringList associatedText(const QString &widgetName);
   
public slots:
  /** Builds the instance then runs*/
  bool run(QFile * =0);
  /** Sets the instance's parent */
  void setParent(QWidget *);
  /** Sets the UI file name */
  void setUIFileName(QString);
  /** Returns whether the instance is built */
  bool isBuilt();
  /** Builds the instance */
  bool build();
  /** Builds the instance from an input file */
  bool build(QFile *);
protected:
  /* Dialog Instance */
  QDialog *m_instance;
  /* Associated Text Instance */
  KommanderWidget *m_textInstance;
  /* UI Dialog file name to open */
  QString m_uiFileName;
  /** The parent widget */
  QWidget *m_parent;
};

#endif
