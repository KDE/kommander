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
#include <qstring.h>
#include <qstringlist.h>
#include <qobject.h>

class KommanderWidget
{
	friend class MyProcess;
public:
	KommanderWidget(QObject *);
	virtual ~KommanderWidget();

  virtual QStringList states() const;
  virtual QStringList displayStates() const;
  virtual QString currentState() const = 0;

  virtual bool isKommanderWidget() const = 0;
  virtual void setAssociatedText(const QStringList& a_associations);
  virtual QStringList associatedText() const;
  virtual QString evalAssociatedText() const;
  virtual QString evalAssociatedText(const QString&) const;

  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  virtual void populate() = 0;

  virtual QString widgetText() const = 0;
  virtual void setWidgetText(const QString&) = 0;
  virtual QString selectedWidgetText() const = 0;
  virtual void setSelectedWidgetText(const QString&) = 0;
  
protected:
  virtual void setStates(const QStringList& a_states);
  virtual void setDisplayStates(const QStringList& a_displayStates);
  QStringList parseArgs( const QString &args, bool &ok ) const;
  // Execute DCOP query and return its result or null on failure
  // Only QString and int are now handled
  QString dcopQuery(const QString&) const;
  // Execute given command, return its result
  QString execCommand(const QString& a_command) const;
  // Display error message a_error; display current class name if no other is given
  void printError(const QString& a_error, const QString& a_classname = QString::null) const;

  QObject *m_thisObject;
  QStringList m_states;
  QStringList m_displayStates;
  QStringList m_associatedText;
  QString m_populationText;
};


#define ESCCHAR '@'

#endif
