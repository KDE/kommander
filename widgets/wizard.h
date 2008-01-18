/***************************************************************************
                          wizard.h - Widget providing a wizard 
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
#ifndef _HAVE_WIZARD_H_
#define _HAVE_WIZARD_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qwizard.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <kommander_export.h>
#include <myprocess.h>

class QShowEvent;
class KOMMANDER_EXPORT Wizard : public QWizard, public KommanderWidget
{
  Q_OBJECT

  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
  Q_PROPERTY(HelpAction helpAction READ helpAction WRITE setHelpAction)
  Q_ENUMS(HelpAction)
  Q_PROPERTY(QString helpActionText READ helpActionText WRITE setHelpActionText)

public:
  Wizard(QWidget *, const char *, bool = true, int = 0);
  ~Wizard();

  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);

  virtual QString handleDCOP(int function, const QStringList& args);
  enum HelpAction { None, Command, Dialog };
  HelpAction helpAction() const;
  void setHelpAction(HelpAction);
  QString helpActionText() const;
  void setHelpActionText(const QString&);

public slots:
  virtual void setWidgetText(const QString &);
  virtual void exec();
  virtual void show();
  virtual void runHelp();
  virtual void populate();
protected slots:
    virtual void initialize();
    virtual void destroy();

signals:
  void widgetOpened();
  void widgetTextChanged(const QString &);
  void finished();
  void contextMenuRequested(int xpos, int ypos);
protected:
  void showEvent( QShowEvent *e );
  void contextMenuEvent( QContextMenuEvent * e );

  HelpAction m_helpAction;
  QString m_helpActionText;
};

#endif
