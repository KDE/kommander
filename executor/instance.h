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
  
  /* passing global variables: two forms are accepted.
     kdmr-executor file.kmdr COUNT=45 PATH=/tmp     - this adds global variables 45 and PATH
     kmdr-executor file.kmdr 45 /tmp                - this adds global variables ARG1 and ARG2
     ARGS holds space-separated list of all second-type variables (like Bash $@)
     ARGCOUNT holds number of second-type arguments
  */
  virtual void addArgument(const QString& argument);

  //DCOP methods. The documentation is in the widgets/specials.cpp file.
  virtual void setEnabled(const QString& widgetName, bool enable);  
  virtual void enableWidget(const QString& widgetName, bool enable);    /* DEPRECATED */
  virtual void setVisible(const QString& widgetName, bool enable);  
  virtual void setText(const QString& widgetName, const QString& text); 
  virtual void changeWidgetText(const QString& widgetName, const QString& text); /* DEPRECATED */
  virtual QString text(const QString& widgetName);
  virtual void setSelection(const QString& widgetName, const QString& text);
  virtual void setCurrentListItem(const QString& widgetName, const QString& text); /* DEPRECATED */
  virtual QString selection(const QString& widgetName);
  virtual int currentItem(const QString &widgetName);  
  virtual QString item(const QString &widgetName, int index);  
  virtual int itemDepth(const QString &widgetName, int index);  
  virtual QString itemPath(const QString &widgetName, int index);  
  virtual void removeItem(const QString &widgetName, int index);
  virtual void removeListItem(const QString &widgetName, int index); /* DEPRECATED */
  virtual void insertItem(const QString &widgetName, const QString &item, int index);
  virtual void addListItem(const QString &widgetName, const QString &item, int index); /* DEPRECATED */
  virtual void setPixmap(const QString &widgetName, const QString &iconName, int index);
  virtual void insertItems(const QString &widgetName, const QStringList &items, int index);
  virtual void addListItems(const QString &widgetName, const QStringList &items, int index);/*DEPRECATED*/
  virtual void addUniqueItem(const QString &widgetName, const QString &item);
  virtual int findItem(const QString &widgetName, const QString &item);
  virtual void clear(const QString &widgetName);
  virtual void clearList(const QString &widgetName); /* DEPRECATED */
  virtual void setCurrentItem(const QString &widgetName, int index);
  virtual void setCurrentTab(const QString &widgetName, int index); /* DEPRECATED */
  virtual void setChecked(const QString &widgetName, bool checked);
  virtual bool checked(const QString &widgetName);
  virtual void setAssociatedText(const QString &widgetName, const QString &text);
  virtual QStringList associatedText(const QString &widgetName);
  virtual QString type(const QString& widget);
  virtual QStringList children(const QString& parent, bool recursive);
  virtual QString global(const QString& variableName);
  virtual void setGlobal(const QString& variableName, const QString& value);
  virtual void setMaximum(const QString &widgetName, int value);
  virtual void execute(const QString &widgetName);
  virtual void cancel(const QString &widgetName);
  virtual int count(const QString &widgetName);
   
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
  QWidget *m_instance;
  /* Associated Text Instance */
  KommanderWidget *m_textInstance;
  /* UI Dialog file name to open */
  KURL m_uiFileName;
  /** The parent widget */
  QWidget *m_parent;
  /* Get object by name */
  QObject* stringToWidget(const QString& name);
  /* Cast to Kommander widget if possible */
  KommanderWidget* kommanderWidget(QObject* object);
  /* Number of global command-line arguments */
  uint m_cmdArguments;
};

#endif
