/***************************************************************************
                          kommanderwidget.cpp - Text widget core functionality 
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
#ifndef _HAVE_KOMMANDERWIDGET_H_
#define _HAVE_KOMMANDERWIDGET_H_

/* KDE INCLUDES */
#include <kprocess.h>

/* QT INCLUDES */
#include <qmap.h>
#include <qobject.h>
#include <qpair.h>
#include <qstring.h>
#include <qstringlist.h>


class KommanderWidget
{
  friend class MyProcess;
public:
  KommanderWidget(QObject *);
  virtual ~KommanderWidget();

  //List of available states. Most widgets have only one state, but f. e. radiobutton has both 
  // 'checked' and 'unchecked'
  virtual QStringList states() const;
  virtual QStringList displayStates() const;
  virtual QString currentState() const = 0;

  virtual bool isKommanderWidget() const = 0;
  
  // Associated script
  virtual void setAssociatedText(const QStringList& a_associations);
  virtual QStringList associatedText() const;
  
  // Execute given script, expanding all @macros.
  virtual QString evalAssociatedText() const;
  virtual QString evalAssociatedText(const QString&) const;

  // Population text. It will become widgetText after populate() is called
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  virtual void populate() = 0;

  // Text of all item(s). Multiple items are separated by \n
  virtual QString widgetText() const = 0;
  virtual void setWidgetText(const QString&) = 0;
  
  // Text of selected item(s). Multiple items are separated by \n
  virtual QString selectedWidgetText() const = 0;
  // Choose selected item(s). Only existing items can be selected.
  virtual void setSelectedWidgetText(const QString&) = 0;
  // Recognizes editor vs executor mode
  static bool inEditor;
  
protected:
  virtual void setStates(const QStringList& a_states);
  virtual void setDisplayStates(const QStringList& a_displayStates);
  
  // Execute DCOP query and return its result or null on failure
  // Only QString and int are now handled
  QString dcopQuery(const QStringList& args) const;
  QString localDcopQuery(const QString function, const QStringList& args) const;
  QString localDcopQuery(const QString function, const QString& arg1, 
     const QString& arg2, const QString& arg3 = QString::null,
     const QString& arg4 = QString::null) const;
  // Execute given command, return its result
  QString execCommand(const QString& a_command, const QString& a_shell = QString::null) const;
  // Find and run dialog (with optional parameters)
  QString runDialog(const QString& a_dialog, const QString& a_params = QString::null) const;
  // Get parent pid
  QString parentPid() const;
  // Display error message a_error; display current class name if no other is given
  void printError(const QString& a_error, const QString& a_classname = QString::null) const;
  
  // Auxiliary functions for parser
  // Return identifier: the longest string of letters and numbers starting from i
  QString parseBrackets(const QString& s, int& from, bool& ok) const;
  // Find matching brackets starting from current position
  QString parseIdentifier(const QString& s, int& from) const;
  // Parse arguments for given function. Returns list of arguments without quotations
  QStringList parseArgs(const QString& s, bool &ok ) const;
  // Remove quotes from given identifier
  QString parseQuotes(const QString& s) const;
  // Parse given identifier as widget name
  KommanderWidget* parseWidget(const QString& name) const;
  // Return parent dialog of this widget
  QObject* parentDialog() const;
  // Register all known functions with number of arguments
  void registerFunctions();
  // Register single function
  void registerFunction(const QString& name, uint minarg = 0, uint maxarg = 0);

  QObject *m_thisObject;
  QStringList m_states;
  QStringList m_displayStates;
  QStringList m_associatedText;
  QString m_populationText;
  
  // List of functions with minimal and maximal arguments count
  static QMap<QString, QPair<uint, uint> > m_functions;
  
};


#define ESCCHAR '@'

#endif
