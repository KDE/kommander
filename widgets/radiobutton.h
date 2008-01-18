/***************************************************************************
                          radiobutton.h - Radiobutton widget 
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
#ifndef _HAVE_RADIOBUTTON_H_
#define _HAVE_RADIOBUTTON_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qradiobutton.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qstring.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <kommander_export.h>
class QWidget;

class QShowEvent;
class KOMMANDER_EXPORT RadioButton : public QRadioButton, public KommanderWidget
{
	Q_OBJECT

	Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
public:
  RadioButton(QWidget *a_parent, const char *a_name);
  ~RadioButton();

  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  
  virtual QString handleDCOP(int function, const QStringList& args);
  virtual bool isFunctionSupported(int function);
public slots:
  virtual void setWidgetText(const QString &);
  virtual void populate();
signals:
  void widgetOpened();
  void widgetTextChanged(const QString &);
  void contextMenuRequested(int xpos, int ypos);
protected:
  void showEvent( QShowEvent *e );
  void contextMenuEvent( QContextMenuEvent * e );
private:
};

#endif
