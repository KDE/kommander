//
// C++ Implementation: toolbox
//
// Description: 
//
//
// Author: Andras Mantia <amantia@kdewebdev.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "toolbox.h"
#include "kommanderplugin.h"
#include "specials.h"


#include <klocale.h>

#define ADDWIDGET 120
#define CURRENTWIDGET 121
#define REMOVEWIDGET 122
#define REMOVEWIDGETAT 123
#define SETCURRENTWIDGET 124
#define CURRENTINDEX 125
#define WIDGETAT 126
#define INDEXOF 127
#define FIRST_FUNCTION ADDWIDGET
#define LAST_FUNCTION INDEXOF 

ToolBox::ToolBox(QWidget *parent, const char *name)
 : QToolBox(parent, name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  KommanderPlugin::setDefaultGroup(Group::DCOP);
  KommanderPlugin::registerFunction(ADDWIDGET, "addWidget(QString widget, QString widgetName, QString Label)",
         i18n("Adds a widget to the toolbox. Returns the index of the widget."), 3);
  KommanderPlugin::registerFunction(CURRENTWIDGET, "currentWidget(QString widget)",
         i18n("Returns the name of the active widget."), 1);
  KommanderPlugin::registerFunction(REMOVEWIDGET, "removeWidget(QString widget, QString widgetName)", i18n("Remove the selected widget, returns the index of the removed widget or -1 if no such widget was found."), 2);
  KommanderPlugin::registerFunction(REMOVEWIDGETAT, "removeWidgetAt(QString widget, int index)", i18n("Remove the widget from the index position, returns the index of the removed widget or -1 if no widget was found."), 2);
  KommanderPlugin::registerFunction(SETCURRENTWIDGET, "setCurrentWidget(QString widget, QString widgetName)",
         i18n("Activates the selected widget."), 2);
  KommanderPlugin::registerFunction(CURRENTINDEX, "currentIndex(QString widget)",
         i18n("Returns the index of the active widget."), 1);
  KommanderPlugin::registerFunction(WIDGETAT, "widgetAt(QString widget, int index)",
         i18n("Returns the widget having the supplied index."), 2);
  KommanderPlugin::registerFunction(INDEXOF, "indexOf(QString widget, QString widgetName)",
         i18n("Returns the index of the widget, -1 if the widget is not part of the toolbox."), 2);

}


ToolBox::~ToolBox()
{
}

QString ToolBox::currentState() const
{
  return QString("default");
}

bool ToolBox::isKommanderWidget() const
{
  return true;
}

void ToolBox::populate()
{
  setAssociatedText(KommanderWidget::evalAssociatedText( populationText()));
}

QStringList ToolBox::associatedText() const
{
  return KommanderWidget::associatedText();
}

void ToolBox::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void ToolBox::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString ToolBox::populationText() const
{
  return KommanderWidget::populationText();
}


QString ToolBox::addWidget(const QString& widgetName, const QString &label)
{
  KommanderWidget *w = widgetByName(widgetName);
  if (w)
  {
    int idx = addItem(dynamic_cast<QWidget*>(w), label);
    adjustSize();
    return QString::number(idx);
  } else
    return QString("-1");

}

void ToolBox::showEvent(QShowEvent* e)
{
  QToolBox::showEvent(e);
  emit widgetOpened();
}

void ToolBox::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

bool ToolBox::isFunctionSupported(int f)
{
  return f == DCOP::count || f == DCOP::geometry || (f >= FIRST_FUNCTION && f <=  LAST_FUNCTION) ;
}

QString ToolBox::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case ADDWIDGET:
      return addWidget(args[0], args[1]);   
      break;
    case CURRENTWIDGET:
    {
      QWidget *w = currentItem();
      if (w) 
        return w->name();
      else
        return QString();
      break;
    }
    case SETCURRENTWIDGET:
    {
      KommanderWidget *w = widgetByName(args[0]);
      setCurrentItem(dynamic_cast<QWidget*>(w));
      return QString();
    }
    case REMOVEWIDGET:
    {
      KommanderWidget *w = widgetByName(args[0]);
      return QString::number(removeItem(dynamic_cast<QWidget*>(w)));
    }
    case REMOVEWIDGETAT:
    {
      QWidget *w = item(args[0].toInt());
      return QString::number(removeItem(w));
    }
    case CURRENTINDEX:
    {
      return QString::number(currentIndex());
      break;
    }
    case WIDGETAT:
    {
      QWidget *w = item(args[0].toInt());
      if (w) 
        return w->name();
      else
        return QString();
      break;
    }
    case INDEXOF:
    {
      KommanderWidget *w = widgetByName(args[0]);
      return QString::number(indexOf(dynamic_cast<QWidget*>(w)));
    }
    case DCOP::count:
      return QString::number(count());
    case DCOP::geometry:
    {
      QString geo = QString::number(this->x())+" "+QString::number(this->y())+" "+QString::number(this->width())+" "+QString::number(this->height());
      return geo;
      break;
    }
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}
#include "toolbox.moc"
