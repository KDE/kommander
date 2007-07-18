/***************************************************************************
                          textedit.h - Rich text editing widget 
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

#ifndef _HAVE_TEXTEDIT_H_
#define _HAVE_TEXTEDIT_H_

/* KDE INCLUDES */
#include <ktextedit.h>

/* QT INCLUDES */
#include <qstringlist.h>
#include <qstring.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "kommander_export.h"

class QShowEvent;
class KOMMANDER_EXPORT TextEdit: public KTextEdit, public KommanderWidget
{
  Q_OBJECT Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget) 
public:
  TextEdit(QWidget *, const char *);
  virtual ~TextEdit();

  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList &);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString &);
  
  virtual QString handleDCOP(int function, const QStringList& args);
  virtual bool isFunctionSupported(int function);

public slots:
  virtual void setTextChanged();
  virtual void setWidgetText(const QString &);
  virtual void populate();

signals: 
  void widgetOpened();
  void widgetTextChanged(const QString&);

protected:
  void showEvent(QShowEvent* e);
};

#endif
