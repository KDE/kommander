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
#include <QtDBus/QtDBus>
/* KDE INCLUDES */
#include <kurl.h>

/* OTHER INCLUDES */
//#include "dbuskommanderif.h"

/* FORWARD DECLARATIONS */
class QWidget;
class QDialog;
class QFile;
class KommanderWidget;



class Instance : public QObject
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.kde.kommanderIf")   
  
public: 
  Instance();
  Instance(const KUrl&, QWidget*);
  ~Instance();
  
  /* passing global variables: two forms are accepted.
     kdmr-executor file.kmdr COUNT=45 PATH=/tmp     - this adds global variables 45 and PATH
     kmdr-executor file.kmdr 45 /tmp                - this adds global variables ARG1 and ARG2
     ARGS holds space-separated list of all second-type variables (like Bash $@)
     ARGCOUNT holds number of second-type arguments
  */
  virtual void addArgument(const QString& argument);
public Q_SLOTS:
  //DCOP methods. The documentation is in the widgets/specials.cpp file.
  Q_SCRIPTABLE virtual void setEnabled(const QString& widgetName, bool enable);  
  Q_SCRIPTABLE virtual void enableWidget(const QString& widgetName, bool enable);    /* DEPRECATED */
  Q_SCRIPTABLE virtual void setVisible(const QString& widgetName, bool enable);  
  Q_SCRIPTABLE virtual void setText(const QString& widgetName, const QString& text); 
  Q_SCRIPTABLE virtual void changeWidgetText(const QString& widgetName, const QString& text); /* DEPRECATED */
  Q_SCRIPTABLE virtual QString text(const QString& widgetName);
  Q_SCRIPTABLE virtual void setSelection(const QString& widgetName, const QString& text);
  Q_SCRIPTABLE virtual void setCurrentListItem(const QString& widgetName, const QString& text); /* DEPRECATED */
  Q_SCRIPTABLE virtual QString selection(const QString& widgetName);
  Q_SCRIPTABLE virtual int currentItem(const QString &widgetName);  
  Q_SCRIPTABLE virtual QString item(const QString &widgetName, int index);  
  Q_SCRIPTABLE virtual int itemDepth(const QString &widgetName, int index);  
  Q_SCRIPTABLE virtual QString itemPath(const QString &widgetName, int index);  
  Q_SCRIPTABLE virtual void removeItem(const QString &widgetName, int index);
  Q_SCRIPTABLE virtual void removeListItem(const QString &widgetName, int index); /* DEPRECATED */
  Q_SCRIPTABLE virtual void insertItem(const QString &widgetName, const QString &item, int index);
  Q_SCRIPTABLE virtual void addListItem(const QString &widgetName, const QString &item, int index); /* DEPRECATED */
  Q_SCRIPTABLE virtual void setPixmap(const QString &widgetName, const QString &iconName, int index);
  Q_SCRIPTABLE virtual void insertItems(const QString &widgetName, const QStringList &items, int index);
  Q_SCRIPTABLE virtual void addListItems(const QString &widgetName, const QStringList &items, int index);/*DEPRECATED*/
  Q_SCRIPTABLE virtual void addUniqueItem(const QString &widgetName, const QString &item);
  Q_SCRIPTABLE virtual int findItem(const QString &widgetName, const QString &item);
  Q_SCRIPTABLE virtual void clear(const QString &widgetName);
  Q_SCRIPTABLE virtual void clearList(const QString &widgetName); /* DEPRECATED */
  Q_SCRIPTABLE virtual void setCurrentItem(const QString &widgetName, int index);
  Q_SCRIPTABLE virtual void setCurrentTab(const QString &widgetName, int index); /* DEPRECATED */
  Q_SCRIPTABLE virtual void setChecked(const QString &widgetName, bool checked);
  Q_SCRIPTABLE virtual bool checked(const QString &widgetName);
  Q_SCRIPTABLE virtual void setAssociatedText(const QString &widgetName, const QString &text);
  Q_SCRIPTABLE virtual QStringList associatedText(const QString &widgetName);
  Q_SCRIPTABLE virtual QString type(const QString& widget);
  Q_SCRIPTABLE virtual QStringList children(const QString& parent, bool recursive);
  Q_SCRIPTABLE virtual QString global(const QString& variableName);
  Q_SCRIPTABLE virtual void setGlobal(const QString& variableName, const QString& value);
  Q_SCRIPTABLE virtual void setMaximum(const QString &widgetName, int value);
  Q_SCRIPTABLE virtual void execute(const QString &widgetName);
  Q_SCRIPTABLE virtual void cancel(const QString &widgetName);
  Q_SCRIPTABLE virtual int count(const QString &widgetName);
  Q_SCRIPTABLE virtual int currentColumn(const QString &widgetName);
  Q_SCRIPTABLE virtual int currentRow(const QString &widgetName);
  Q_SCRIPTABLE virtual void insertRow(const QString &widgetName, int row, int count);
  Q_SCRIPTABLE virtual void insertColumn(const QString &widgetName, int column, int count);
  Q_SCRIPTABLE virtual void setCellText(const QString &widgetName, int row, int column, const QString& text);
  Q_SCRIPTABLE virtual QString cellText(const QString &widgetName, int row, int column);
  Q_SCRIPTABLE virtual void removeRow(const QString &widgetName, int row, int count);
  Q_SCRIPTABLE virtual void removeColumn(const QString &widgetName, int column, int count);
  Q_SCRIPTABLE virtual void setRowCaption(const QString &widgetName, int row, const QString& text);
  Q_SCRIPTABLE virtual void setColumnCaption(const QString &widgetName, int column, const QString& text);
   
public slots:
  /** Builds the instance then executes it */
  bool run(QFile* = 0);
  /** Sets the instance's parent */
  void setParent(QWidget*);
  /** Sets the UI file name */
  void setUIFileName(const KUrl&);
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
  KUrl m_uiFileName;
  /** The parent widget */
  QWidget *m_parent;
  /* Get object by name */
  QObject* stringToWidget(const QString& name);
  /* Cast to Kommander widget if possible */
  KommanderWidget* kommanderWidget(QObject* object);
  /* Number of global command-line arguments */
  int m_cmdArguments;
};

#endif
