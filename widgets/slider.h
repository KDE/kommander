/***************************************************************************
                          slider.h - Slider widget
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

#ifndef _HAVE_SLIDER_H_
#define _HAVE_SLIDER_H_

/* KDE INCLUDES */
#include <qslider.h>

/* QT INCLUDES */
#include <qstringlist.h>
#include <qstring.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QShowEvent;
class Slider: public QSlider, public KommanderWidget
{
  Q_OBJECT Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget) 
public:
  Slider(QWidget*, const char*);
  virtual ~Slider();
  
  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList &);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString &);
  
  virtual QString handleDCOP(int function, const QStringList& args);
  virtual bool isFunctionSupported(int function);

public slots:
  virtual void setWidgetText(const QString &);
  virtual void populate();

signals: 
  void widgetOpened();

protected:
  void showEvent(QShowEvent* e);
};

#endif
