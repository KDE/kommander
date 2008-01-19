//
// C++ Interface: toolbox
//
// Description: 
//
//
// Author: Andras Mantia <amantia@kdewebdev.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <qtoolbox.h>
#include <kommanderwidget.h>

/**
	@author Andras Mantia <amantia@kdewebdev.org>
*/
class ToolBox : public QToolBox, public KommanderWidget
{
  Q_OBJECT
  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
public:
  ToolBox(QWidget *parent = 0, const char *name = 0);

  ~ToolBox();

  virtual bool isKommanderWidget() const;
  virtual QString currentState() const;
  virtual bool isFunctionSupported(int function);
  virtual QString handleDCOP(int function, const QStringList& args);

public slots:
  virtual void populate();

signals:
  void widgetOpened();
  void contextMenuRequested(int xpos, int ypos);
protected:
  void showEvent(QShowEvent *e);
  void contextMenuEvent( QContextMenuEvent * e );
  QString addWidget(const QString& widgetName, const QString &label);

};

#endif
