/***************************************************************************
                          combobox.h - Combobox widget 
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

#ifndef _HAVE_COMBOBOX_H_
#define _HAVE_COMBOBOX_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qstringlist.h>
#include <qobject.h>
#include <qstring.h>

/* OTHER INCLUDES */
#include <kcombobox.h>
#include <kommanderwidget.h>

class QWidget;

class QShowEvent;
class ComboBox : public KComboBox, public KommanderWidget
{
	Q_OBJECT

	Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
	
//	Q_PROPERTY(QStringList items READ items WRITE setItems RESET resetItems);
public:
  ComboBox(QWidget *a_parent, const char *a_name);
  ~ComboBox();

  virtual QString widgetText() const;
  virtual QString selectedWidgetText() const;

  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;

  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);

public slots:
  virtual void setWidgetText(const QString&);
  virtual void setSelectedWidgetText(const QString & a_text);
  virtual void populate();
  
  virtual void emitWidgetTextChanged(int);
signals:
  void widgetOpened();
  void widgetTextChanged(const QString&);
protected:
  void showEvent(QShowEvent *e);
private:
};

#endif
