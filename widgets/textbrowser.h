/***************************************************************************
                          textedit.h - Rich text viewing widget with links
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

#ifndef _HAVE_TEXTBROWSER_H_
#define _HAVE_TEXTBROWSER_H_

/* KDE INCLUDES */
#include <ktextbrowser.h>

/* QT INCLUDES */
#include <qstringlist.h>
#include <qstring.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QShowEvent;
class TextBrowser: public KTextBrowser, public KommanderWidget
{
  Q_OBJECT Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget) 
public:
  TextBrowser(QWidget*);
  virtual ~TextBrowser();
  
  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList &);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString &);
  
  virtual QString handleDBUS(int function, const QStringList& args);
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
