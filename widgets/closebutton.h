/***************************************************************************
                          closebutton.h - Button that when clicked, closes the dialog it's in
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


#ifndef _HAVE_CLOSEBUTTON_H_
#define _HAVE_CLOSEBUTTON_H_

/* KDE INCLUDES */
#include <kpushbutton.h>

/* QT INCLUDES */
#include <qobject.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QWidget;

class QShowEvent;
class CloseButton : public KPushButton, public KommanderWidget
{
  Q_OBJECT

  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
  Q_PROPERTY(bool writeStdout READ writeStdout WRITE setWriteStdout)
public:
  CloseButton(QWidget* a_parent, const char* a_name);
  ~CloseButton();

  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  bool writeStdout() const;
  
  virtual QString handleDCOP(int function, const QStringList& args);
  virtual bool isFunctionSupported(int function);
public slots:
  virtual void setWriteStdout(bool);
  virtual void setWidgetText(const QString &);
  virtual void startProcess();
  virtual void appendOutput(KProcess*, char*, int);
  virtual void endProcess(KProcess*);
  virtual void populate();
signals:
  void widgetOpened();
  void widgetTextChanged(const QString&);
protected:
  bool m_writeStdout;
  QString m_output;
  void showEvent(QShowEvent* e);
private:
};

#endif
