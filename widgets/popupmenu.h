//
// C++ Interface: popupmenu
//
// Description: 
//
//
// Author: Andras Mantia <amantia@kdewebdev.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef POPUPMENU_H
#define POPUPMENU_H

#include <qlabel.h>
#include <qmap.h>
#include <kommanderwidget.h>


class KMenu;

/**
	@author Andras Mantia <amantia@kdewebdev.org>
*/
class PopupMenu : public QLabel, public KommanderWidget
{
  Q_OBJECT
  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
public:
  PopupMenu(QWidget *parent = 0, const char *name = 0);
  ~PopupMenu();

  virtual bool isKommanderWidget() const;
  virtual QString currentState() const;
  virtual bool isFunctionSupported(int function);
  virtual QString handleDBUS(int function, const QStringList& args);
  using KommanderWidget::handleDBUS;
  virtual QStringList associatedText() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  KMenu *menu() {return m_menu;}

public slots:
  virtual void setWidgetText(const QString &);
  virtual void populate();
  void popup(int x, int y);

private slots:
  void slotMenuItemActivated(int id);

private:
  QString insertSubmenu(const QString& title, const QString &menuWidget, int index, const QString& icon);

  KMenu *m_menu;
  QMap<int, QString> m_associations;
  QStringList m_params;
};

#endif
