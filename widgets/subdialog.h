/***************************************************************************
                          subdialog.h - Subdialog widget 
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
#ifndef _HAVE_SUBDIALOG_H_
#define _HAVE_SUBDIALOG_H_

/* KDE INCLUDES */
#include <kpushbutton.h>

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "kommanderwidgets_export.h"
class QWidget;
class KDialog;

class QShowEvent;
class KOMMANDERWIDGETS_EXPORT SubDialog : public KPushButton, public KommanderWidget
{
  Q_OBJECT
  
  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QString kmdrFile READ kmdrFile WRITE setKmdrFile)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
public:
  SubDialog(QWidget *a_parent, const char *a_name);
  ~SubDialog();
  
  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  QString kmdrFile() const;

  virtual QString handleDBUS(int function, const QStringList& args);  
  using KommanderWidget::handleDBUS;
public slots:
  void setKmdrFile(QString);
  virtual void showDialog();
  virtual void slotFinished();
  virtual void populate();
signals:
  void widgetOpened();
  void widgetTextChanged(const QString &);
protected:
  KDialog *m_dialog;
  QString m_kmdrFile;
  void showEvent( QShowEvent *e );
private:
};

#endif
