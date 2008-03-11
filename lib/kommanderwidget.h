/***************************************************************************
                    kommanderwidget.h - Text widget core functionality 
                             -------------------
    copyright          : (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
                         (C) 2004      Michal Rudolf <mrudolf@kdewebdwev.org>
    
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
#include <k3process.h>

#include "kommander_export.h"
/* QT INCLUDES */
#include <qmap.h>
#include <qobject.h>
#include <qpair.h>
#include <qstring.h>
#include <qstringlist.h>

class ParserData;

class KOMMANDER_EXPORT KommanderWidget
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
  virtual bool hasAssociatedText();
  
  // Execute default script, expanding all @macros.
    virtual QString evalAssociatedText();
  // Execute given script, expanding all @macros.
  virtual QString evalAssociatedText(const QString&);
  // Evaluate given Kommander function using given args.
  virtual QString evalFunction(const QString& function, const QStringList& args);
  // Parse and evaluate function for given widget, converting it to appropriate DBUS call.
  virtual QString evalWidgetFunction(const QString& identifier, const QString& s, int& pos);
  // Evaluate given array function using given args.
  virtual QString evalArrayFunction(const QString&, const QStringList&) const;
  // Parse and evaluate given execBegin..execEnd block.
  virtual QString evalExecBlock(const QStringList&, const QString& s, int& pos);
  // Parse and evaluate given forEach..end block.
  virtual QString evalForEachBlock(const QStringList&, const QString& s, int& pos);
  // Parse and evaluate given for..end block.
  virtual QString evalForBlock(const QStringList&, const QString& s, int& pos);
  // Parse and evaluate given switch..case..end block.
  virtual QString evalSwitchBlock(const QStringList&, const QString& s, int& pos);
  // Parse and evaluate given if..endif block.
  virtual QString evalIfBlock(const QStringList&, const QString& s, int& pos);
  
  
  // Population text. It will become widgetText after populate() is called
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  virtual void populate() = 0;

  
  
  // Handles all widget-specific DBUS calls 
  virtual QString handleDBUS(int function, const QStringList& args = QStringList());
  // helper function
  virtual QString handleDBUS(int function, const QString arg){
    return handleDBUS(function, arg.split("\n"));
  }
  
  // Checks if appropriate function is supported by widget. By default all functions
  // are reported as supported: use this to allow recognizing incorrect function calls.
  virtual bool isFunctionSupported(int function);
  // Checks if the function is common widget function (i. e. supported by all widgets)
  virtual bool isCommonFunction(int function);
  // Checks if the string is a valid widget name)
  virtual bool isWidget(const QString& a_name) const;
  // Returns widget from name 
  virtual KommanderWidget* widgetByName(const QString& a_name) const;
  // Returns filename associated with the dialog
  virtual QString fileName();
  // Returns current widget name;
  virtual QString widgetName() const;
  QObject* object() { return m_thisObject;}

  
  // Recognizes editor vs executor mode
  static bool inEditor;
  // Prints errors in message boxes, not in stderr
  static bool showErrors;
  // Default parser
  static bool useInternalParser;
  // Return global variable value
  static QString global(const QString& variableName);
  // Set global variable value
  static void setGlobal(const QString& variableName, const QString& value);
  // Return parent dialog of this widget
  QWidget* parentDialog() const;

protected:
  virtual void setStates(const QStringList& a_states);
  virtual void setDisplayStates(const QStringList& a_displayStates);
  
  // Execute DBUS query and return its result or null on failure
  // Only QString and int are now handled
  QString DBUSQuery(const QStringList& args);
  QString localDBUSQuery(const QString function, const QStringList& args = QStringList());
  QString localDBUSQuery(const QString function, const QString& arg1, 
     const QString& arg2, const QString& arg3 = QString(),
     const QString& arg4 = QString());
  // Execute given command, return its result
  QString execCommand(const QString& a_command, const QString& a_shell = QString()) const;
  // Find and run dialog (with optional parameters)
  QString runDialog(const QString& a_dialog, const QString& a_params = QString());
  // Display error message a_error; display current class name if no other is given
  void printError(const QString& a_error) const;
  
  // Auxiliary functions for parser
  // Find matching brackets starting from current position
  QString parseBrackets(const QString& s, int& from, bool& ok) const;
  // Return identifier: the longest string of letters and numbers starting from i
  QString parseIdentifier(const QString& s, int& from) const;
  // Parse arguments for given function. Returns list of arguments without quotations
  QStringList parseArgs(const QString& s, bool &ok );
  // Remove quotes from given identifier
  QString parseQuotes(const QString& s) const;
  // Parse function
  QStringList parseFunction(const QString group, const QString& function,
    const QString& s, int& from, bool& ok);
  // Detect and return block boundary
  int parseBlockBoundary(const QString& s, int from, const QStringList& args) const;
  int parseBlockBoundary(const QString& s, int from, const QString& arg) const;
    
  // Parse given identifier as widget name
  KommanderWidget* parseWidget(const QString& name) const;
  QString substituteVariable(QString text, QString variable, QString value) const;
   
  ParserData* internalParserData();
      
  QObject *m_thisObject;
  QStringList m_states;
  QStringList m_displayStates;
  QStringList m_associatedText;
  QString m_populationText;
  
  // Global variables 
  static QMap<QString, QString> m_globals;
  // Global arrays 
  static QMap<QString, QMap<QString, QString> > m_arrays;
  // Internal parser data
  static ParserData* m_parserData;
};


#define ESCCHAR '@'

#endif
