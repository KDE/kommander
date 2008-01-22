//
// C++ Interface: aboutdialog
//
// Description: 
//
//
// Author: Andras Mantia <amantia@kdewebdev.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <qlabel.h>
#include <kommanderwidget.h>

class KAboutApplication;
class KAboutData;

/**
	@author Andras Mantia <amantia@kdewebdev.org>
*/
class AboutDialog : public QLabel, public KommanderWidget
{
  Q_OBJECT
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
public:
    AboutDialog(QWidget *parent = 0, const char *name = 0);

    ~AboutDialog();

  virtual bool isKommanderWidget() const;
  virtual QString currentState() const;
  virtual bool isFunctionSupported(int function);
  virtual QString handleDCOP(int function, const QStringList& args);
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);

public slots:
  virtual void populate();

private:
  void setProductInfo(const QString& appName, const QString &icon, const QString& version, const QString& copyright);  
  void addAuthor(const QString& author, const QString &task, const QString& email, const QString &webAddress);
  void addTranslator(const QString& author, const QString& email);
  void setDescription(const QString& description);
  void setHomepage(const QString &homepage);
  void setBugAddress(const QString &bugAddress);
  void setLicense(const QString &license);

  KAboutData *m_aboutData;
  QString m_version;
};

#endif
