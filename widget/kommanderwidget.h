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
  virtual bool hasAssociatedText();
  
  // Execute given script, expanding all @macros.
  virtual QString evalAssociatedText();
  virtual QString evalAssociatedText(const QString&);
  virtual QString evalFunction(const QString&, const QStringList&);
  virtual QString evalWidgetFunction(const QString& identifier, const QString& s, int& pos);
  virtual QString evalArrayFunction(const QString&, const QStringList&) const;
  virtual QString evalStringFunction(const QString&, const QStringList&) const;
  virtual QString evalFileFunction(const QString&, const QStringList&) const;
  virtual QString evalExecBlock(const QStringList&, const QString& s, int& pos);
  virtual QString evalForBlock(const QStringList&, const QString& s, int& pos);
  
  // Population text. It will become widgetText after populate() is called
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  virtual void populate() = 0;

  
  
  /* Handles all widget-specific DCOP calls */
  virtual QString handleDCOP(int function, const QStringList& args = QStringList());
  
  // Recognizes editor vs executor mode
  static bool inEditor;
  // Prints errors in message boxes, not in stderr
  static bool showErrors;
  // Return global variable value
  static QString global(const QString& variableName);
  // Set global variable value
  static void setGlobal(const QString& variableName, const QString& value);

protected:
  virtual void setStates(const QStringList& a_states);
  virtual void setDisplayStates(const QStringList& a_displayStates);
  
  // Execute DCOP query and return its result or null on failure
  // Only QString and int are now handled
  QString DCOPQuery(const QStringList& args);
  QString localDCOPQuery(const QString function, const QStringList& args = QStringList());
  QString localDCOPQuery(const QString function, const QString& arg1, 
     const QString& arg2, const QString& arg3 = QString::null,
     const QString& arg4 = QString::null);
  // Execute given command, return its result
  QString execCommand(const QString& a_command, const QString& a_shell = QString::null) const;
  // Find and run dialog (with optional parameters)
  QString runDialog(const QString& a_dialog, const QString& a_params = QString::null);
  // Display error message a_error; display current class name if no other is given
  void printError(const QString& a_error) const;
  
  // Auxiliary functions for parser
  // Return identifier: the longest string of letters and numbers starting from i
  QString parseBrackets(const QString& s, int& from, bool& ok) const;
  // Find matching brackets starting from current position
  QString parseIdentifier(const QString& s, int& from) const;
  // Parse arguments for given function. Returns list of arguments without quotations
  QStringList parseArgs(const QString& s, bool &ok );
  // Remove quotes from given identifier
  QString parseQuotes(const QString& s) const;
  // Parse function
  QStringList parseFunction(const QString group, const QString& function,
    const QString& s, int& from, bool& ok);
  // Parse given identifier as widget name
  KommanderWidget* parseWidget(const QString& name) const;
  // Return parent dialog of this widget
  QObject* parentDialog() const;
  
  QObject *m_thisObject;
  QStringList m_states;
  QStringList m_displayStates;
  QStringList m_associatedText;
  QString m_populationText;
  
  // Global variables 
  static QMap<QString, QString> m_globals;
  // Global arrays 
  static QMap<QString, QMap<QString, QString> > m_arrays;
};


#define ESCCHAR '@'

#endif
