/***************************************************************************
                          instance.h  -  description
                             -------------------
    begin                : Tue Aug 13 2002
    copyright            : (C) 2002 by Marc Britton
    email                : consume@optushome.com.au
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

/* FORWARD DECLARATIONS */
class QWidget;
class AssocTextWidget;
class QDialog;
class QFile;

/**
  *@author Marc Britton
  */

/* CLASS DEFINITION*/
class Instance : public QObject
{
  Q_OBJECT
  
public: 
	Instance();
  Instance(QString, QWidget *);
	~Instance();
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
  AssocTextWidget *m_textInstance;
  /* UI Dialog file name to open */
  QString m_uiFileName;
  /** The parent widget */
  QWidget *m_parent;
};

#endif
