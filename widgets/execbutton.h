/***************************************************************************
                          execbutton.h - Button that runs its text association 
                             -------------------
    copyright            : (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
                           (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef _HAVE_EXECBUTTON_H_
#define _HAVE_EXECBUTTON_H_

/* KDE INCLUDES */
#include <kpushbutton.h>

/* QT INCLUDES */
#include <qobject.h>
//Added by qt3to4:
#include <QShowEvent>
#include <QProcess>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "kommanderwidgets_export.h"

class QWidget;
class MyProcess;

class QShowEvent;
class KOMMANDERWIDGETS_EXPORT ExecButton : public KPushButton, public KommanderWidget
{
  Q_OBJECT

  Q_ENUMS(Blocking)
  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
  Q_PROPERTY(bool writeStdout READ writeStdout WRITE setWriteStdout)
  Q_PROPERTY(Blocking blockGUI READ blockGUI WRITE setBlockGUI)

public:
  ExecButton(QWidget *a_parent, const char *a_name);
  ~ExecButton();

  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;

  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);

  // Handle stdout setting
  virtual void setWriteStdout(bool);
  bool writeStdout() const;
  // Handle blocking
  enum Blocking { None, Button, GUI };
  virtual void setBlockGUI(Blocking a_enable);
  Blocking blockGUI() const;
  
  virtual QString handleDBUS(int function, const QStringList& args);
  using KommanderWidget::handleDBUS;
  virtual bool isFunctionSupported(int function);
public slots:
  virtual void populate();
  virtual void setWidgetText(const QString &);
  
  // Execute script from associastedText
  virtual void startProcess();
  // Process has ended
  virtual void processExited(MyProcess* process, int exitCode, QProcess::ExitStatus exitStatus);
signals:
  void widgetOpened();
  void widgetTextChanged(const QString&);
  
protected:
  // Whether output from process should be put in real stdout
  bool m_writeStdout;
  // Whether pressing execubtton should block GUI until process ends
  Blocking m_blockGUI;
  // Output from process
  QString m_output;
  void showEvent( QShowEvent *e );
private:
};

#endif
