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
#include <ktextedit.h>

/* QT INCLUDES */
#include <qobject.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QWidget;
class MyProcess;

class KOMMANDER_EXPORT Konsole : public KTextEdit, public KommanderWidget
{
  Q_OBJECT

  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_OVERRIDE(bool readOnly DESIGNABLE false)
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
    // Execute current script
    virtual void execute();
    // Stop executing current script
    virtual void cancel();
    // Process has ended
    virtual void processReceivedStdout(MyProcess*, char* a_buffer, int a_len);
    virtual void processExited(MyProcess* p);
  signals:
    void finished();
  protected:
    // Whether last line of output was ended with EOL
    bool mSeenEOL;
    // Current process or NULL if there is no process running
    MyProcess* mProcess;
};

#endif
