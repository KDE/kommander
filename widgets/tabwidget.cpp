/***************************************************************************
                          tabwidget.cpp - Widget with tabs 
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
/* KDE INCLUDES */
#include <klocale.h>
#include <kiconloader.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qtabwidget.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "kommanderplugin.h"
#include <specials.h>
#include "tabwidget.h"

enum Functions {
  FirstFunction = 355,
  TAB_setTabIcon,
  TAB_tabLabel,
  TAB_isTabEnabled,
  TAB_setTabEnabled,
  LastFunction
};

TabWidget::TabWidget(QWidget *a_parent, const char *a_name, int a_flags)
	: QTabWidget(a_parent, a_name, a_flags), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);

  KommanderPlugin::setDefaultGroup(Group::DCOP);
  KommanderPlugin::registerFunction(TAB_setTabIcon, "setTabIcon(QString widget, int Tab, QString Icon)", i18n("Sets an icon on the specified tab. Index is zero based."), 3);
  KommanderPlugin::registerFunction(TAB_tabLabel, "tabLabel(QString widget, int Tab)", i18n("Returns the tab label at the given index. Index is zero based."), 2);
  KommanderPlugin::registerFunction(TAB_isTabEnabled, "isTabEnabled(QString widget, int Tab)", i18n("Returns true if tab at specified index is enabled, otherwise returns false."), 2);
  KommanderPlugin::registerFunction(TAB_setTabEnabled, "setTabEnabled(QString widget, int Tab, bool Enabled)", i18n("Sets the tab at the given index to enabled or disabled."), 3);
}

TabWidget::~TabWidget()
{
}

QString TabWidget::currentState() const
{
  return QString("default");
}

bool TabWidget::isKommanderWidget() const
{
  return true;
}

QStringList TabWidget::associatedText() const
{
  return KommanderWidget::associatedText();
}

void TabWidget::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void TabWidget::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText( a_text );
}

QString TabWidget::populationText() const
{
  return KommanderWidget::populationText();
}

void TabWidget::populate()
{
}

void TabWidget::showEvent(QShowEvent* e)
{
  QTabWidget::showEvent(e);
  emit widgetOpened();
}

void TabWidget::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

bool TabWidget::isFunctionSupported(int f)
{
  return f == DCOP::currentItem || f == DCOP::setCurrentItem || f == DCOP::insertTab || (f >= FirstFunction && f <= LastFunction) ;
}

QString TabWidget::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::currentItem:
      return QString::number(currentPageIndex());
    case DCOP::setCurrentItem:
      setCurrentPage(args[0].toUInt());
      break;
    case DCOP::insertTab:
      insertTab(0L, args[0], args[1].toUInt());
      break;
    case TAB_tabLabel:
    {
      QString s = this->label(args[0].toInt());
      return s.remove("&");
      break;
    }
    case TAB_setTabIcon:
    {
      QWidget *w = page(args[0].toInt());
      setTabIconSet(w, KGlobal::iconLoader()->loadIcon(args[1], KIcon::NoGroup, KIcon::SizeMedium));
      break;
    }
    case TAB_isTabEnabled:
    {
      QWidget *w = page(args[0].toInt());
      return QString::number(this->isTabEnabled(w));
      break;
    }
    case TAB_setTabEnabled:
    {
      QWidget *w = page(args[0].toInt());
      this->setTabEnabled(w, args[1].toInt());
     break;
    }
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}

#include "tabwidget.moc"
