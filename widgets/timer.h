/***************************************************************************
                          timer.h - Widget for running scripts periodically
                             -------------------
    copyright            : (C) 2004 Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#ifndef _HAVE_TIMER_H_
#define _HAVE_TIMER_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qlabel.h>
 
/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QTimer;
class Timer : public QLabel, public KommanderWidget
{
  Q_OBJECT

  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
  Q_PROPERTY(int interval READ interval WRITE setInterval)
  Q_PROPERTY(bool singleShot READ singleShot WRITE setSingleShot)
            
  public:
    Timer(QWidget *a_parent, const char *a_name);
    ~Timer();

    virtual bool isKommanderWidget() const;
    virtual void setAssociatedText(const QStringList&);
    virtual QStringList associatedText() const;
    virtual QString currentState() const;
    virtual QString populationText() const;
    virtual void setPopulationText(const QString&);
    virtual int interval() const;
    virtual void setInterval(int a_interval);
    virtual bool singleShot() const;
    virtual void setSingleShot(bool a_shot);
    virtual QString handleDCOP(int function, const QStringList& args);
    virtual bool isFunctionSupported(int function);
  public slots:
    virtual void setWidgetText(const QString &);
    virtual void populate();
    virtual void execute();
    virtual void cancel(); 
  protected slots:
    virtual void timeout();
  signals:
    void finished();

  protected:
    virtual void executeProcess(bool blocking);
    QTimer* mTimer;
    int mInterval;
    bool mSingleShot;
  
};

#endif
