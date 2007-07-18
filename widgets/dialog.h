/***************************************************************************
                          dialog.h  -  Kommander dialog
                             -------------------
    copyright            : (C) 2002 by Marc Britton <consume@optushome.com.au>
                           (C) 2004 by Andras Mantia <amantia@kde.org>
                           (C) 2004 by Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _HAVE_DIALOG_H_
#define _HAVE_DIALOG_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qdialog.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <kommanderwindow.h>
#include "kommander_export.h"
class QShowEvent;
class KOMMANDER_EXPORT Dialog : public QDialog, public KommanderWindow
{
  Q_OBJECT

  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
  Q_PROPERTY(bool useInternalParser READ useInternalParser WRITE setUseInternalParser)
  Q_OVERRIDE(bool modal DESIGNABLE false)
  
public:
  Dialog(QWidget *, const char *, bool=TRUE, int=0);
  ~Dialog();
  virtual bool isKommanderWidget() const;
  virtual bool useInternalParser() const;
  virtual void setUseInternalParser(bool b);
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  virtual void show();
  
  virtual QString handleDCOP(int function, const QStringList& args);
  virtual bool isFunctionSupported(int function);
public slots:
  virtual void setWidgetText(const QString&);
  virtual void exec();
  virtual void populate();
  virtual void done(int r);
protected slots:
  virtual void initialize();
  virtual void destroy();
  
signals:
  void widgetOpened();
  void widgetTextChanged(const QString&);
  void finished();
protected:
  void showEvent( QShowEvent *e );
  
private:
  QString m_fileName;
};

#endif
