/***************************************************************************
                          instance.h  -  description
                             -------------------
    begin                : Tue Aug 13 2002
    copyright            : (C) 2002 by Marc Britton <consume@optushome.com.au>
                           (C) 2004 by Andras Mantia <amantia@kde.org>
                           (C) 2004 by Michal Rudolf <mrudolf@kdewebdev.org>
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

/* QT INCLUDES */
#include <qstring.h>
#include <qobject.h>
#include <qmap.h>

/* KDE INCLUDES */
#include <kurl.h>

/* OTHER INCLUDES */
#include "dcopkommanderif.h"

/* FORWARD DECLARATIONS */
class QWidget;
class QDialog;
class QFile;
class KommanderWidget;



class Instance : public QObject, virtual public DCOPKommanderIf
{
  Q_OBJECT
    
public: 
  Instance();
  Instance(const KURL&, QWidget*);
  ~Instance();
  
  /* add global variable; two forms are accepted:
     kdmr-executor file.kmdr COUNT=45 PATH=/tmp     - this adds global variables 45 and PATH
     kmdr-executor file.kmdr 45 /tmp                - this adds global variables ARG1 and ARG2
     ARGS holds space-separated list of all second-type variables (like Bash $@)
     ARGCOUNT holds number of second-type arguments
  */
  virtual void addArgument(const QString& argument);

//DCOP methods. The documentation is in the dcopkommanderif.h file.
  virtual void enableWidget(const QString& widgetName, bool enable);  
  virtual void changeWidgetText(const QString& widgetName, const QString& text);      
  virtual int currentItem(const QString &widgetName);
  virtual QString item(const QString &widgetName, int index);
  virtual void removeListItem(const QString &widgetName, int index);
  virtual void addListItem(const QString &widgetName, const QString &item, int index);
  virtual void addListItems(const QString &widgetName, const QStringList &items, int index);
  virtual void clearList(const QString &widgetName);
  virtual void setCurrentListItem(const QString& widgetName, const QString &name);
  virtual void setCurrentTab(const QString &widgetName, int index);
  virtual void setChecked(const QString &widgetName, bool checked);
  virtual void setAssociatedText(const QString &widgetName, const QString &text);
  virtual QStringList associatedText(const QString &widgetName);
  virtual QString global(const QString& variableName);
  virtual void setGlobal(const QString& variableName, const QString& value);
   
public slots:
  /** Builds the instance then executes it */
  bool run(QFile* = 0);
  /** Sets the instance's parent */
  void setParent(QWidget*);
  /** Sets the UI file name */
  void setUIFileName(const KURL&);
  /** Returns whether the instance is built */
  bool isBuilt();
  /** Builds the instance */
  bool build();
  /** Builds the instance from an input file */
  bool build(QFile*);
protected:
  /* Dialog Instance */
  QDialog *m_instance;
  /* Associated Text Instance */
  KommanderWidget *m_textInstance;
  /* UI Dialog file name to open */
  KURL m_uiFileName;
  /** The parent widget */
  QWidget *m_parent;
  /* Global variables */
  QMap<QString, QString> m_globals;
  /* Get object by name */
  QObjectList* stringToWidget(const QString& name);
  /* Number of global command-line arguments */
  uint m_cmdArguments;
};

#endif
