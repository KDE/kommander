//
// C++ Interface: colordialog
//
// Description: 
//
//
// Author: Andras Mantia <amantia@kdewebdev.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FONTDIALOG_H
#define FONTDIALOG_H

#include <qlabel.h>
#include <kommanderwidget.h>

#include <qfont.h>

/**
	@author Andras Mantia <amantia@kdewebdev.org>
*/
class FontDialog : public QLabel, public KommanderWidget
{
  Q_OBJECT
  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
public:
    FontDialog(QWidget *parent = 0, const char *name = 0);

    ~FontDialog();

  virtual bool isKommanderWidget() const;
  virtual QString currentState() const;
  virtual bool isFunctionSupported(int function);
  virtual QString handleDCOP(int function, const QStringList& args);
  virtual QStringList associatedText() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);

public slots:
  virtual void setWidgetText(const QString &);
  virtual void populate();

  QFont m_font;
};

#endif
