/***************************************************************************
                          konsole.h - Widget that shows output of a process
                             -------------------
    copyright            : (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef _HAVE_KONSOLE_H_
#define _HAVE_KONSOLE_H_

/* KDE INCLUDES */
#include <klistbox.h>

/* QT INCLUDES */
#include <qobject.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QWidget;
class KProcess;

class Konsole : public KListBox, public KommanderWidget
{
  Q_OBJECT

  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)

  public:
    Konsole(QWidget *a_parent, const char *a_name);
    ~Konsole();

    virtual bool isKommanderWidget() const;
    virtual void setAssociatedText(const QStringList&);
    virtual QStringList associatedText() const;
    virtual QString currentState() const;

    virtual QString populationText() const;
    virtual void setPopulationText(const QString&);

    virtual QString handleDCOP(int function, const QStringList& args);
    virtual bool isFunctionSupported(int function);
  public slots:
    virtual void populate();
    virtual void setWidgetText(const QString &);
    virtual void execute();
    // Process has ended
    virtual void processReceivedStdout(MyProcess*, char* a_buffer, int a_len);
    virtual void processExited(MyProcess* p);
  signals:
    void widgetTextChanged(const QString&);
};

#endif
