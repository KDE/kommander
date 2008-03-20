/***************************************************************************
                          wizard.cpp - Widget providing a wizard 
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

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <q3wizard.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QShowEvent>

/* OTHER INCLUDES */
#include <kommanderfactory.h>
#include <kommanderwidget.h>
#include "wizard.h"
#include "myprocess.h"
#include "specials.h"

Wizard::Wizard(QWidget *a_parent, const char *a_name, bool a_modal, int a_flags)
  : Q3Wizard(a_parent, a_name, a_modal), KommanderWidget(this)
{
  Q_UNUSED(a_flags);
  QStringList states;
  states << "default";
  states << "initialization";
  states << "destroy";
  setStates(states);
  setDisplayStates(states);

  m_firstShow = true;
  connect(this, SIGNAL(helpClicked()), SLOT(runHelp()));
}

Wizard::~Wizard()
{
  if (!inEditor)
    destroy();
}

QString Wizard::currentState() const
{
  return QString("default");
}

bool Wizard::isKommanderWidget() const
{
  return true;
}

QStringList Wizard::associatedText() const
{
  return KommanderWidget::associatedText();
}

void Wizard::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void Wizard::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString Wizard::populationText() const
{
  return KommanderWidget::populationText();
}

void Wizard::populate()
{
  QString txt = KommanderWidget::evalAssociatedText(populationText());
//FIXME: implement me
}

void Wizard::setWidgetText(const QString &a_text)
{
  setWindowTitle(a_text);
  emit widgetTextChanged(a_text);
}

void Wizard::initialize()
{
  setFinishEnabled(page(pageCount() - 1), true);
  const QStringList assoc = associatedText();
  if (assoc.count() > 1 && !assoc[1].isEmpty()) 
  {
    MyProcess proc(this);
    proc.run( KommanderWidget::evalAssociatedText(assoc[1]) );
  }
}

void Wizard::destroy()
{
  const QStringList assoc = associatedText();
  if (assoc.count() > 2 && !assoc[2].isEmpty()) 
  {
    MyProcess proc(this);
    proc.run(KommanderWidget::evalAssociatedText(assoc[2]));
  }
}

void Wizard::exec()
{
  Q3Wizard::exec();
  emit finished();
}

void Wizard::setVisible(bool visible)
{
  //avoid initialization on every show/hide
  if (!m_firstShow) 
  {
    Q3Wizard::setVisible(visible);
  } else
  {
    m_firstShow = false;
    Q3Wizard::setVisible(visible);
    if (!inEditor)
      initialize();
  }
}

void Wizard::runHelp()
{
  if (helpAction() == Command)
  {
    KProcess::startDetached(helpActionText());
  }
  else if (helpAction() == Dialog)
  {
    KommanderFactory::loadPlugins();
    QDialog *dialog = (QDialog *)KommanderFactory::create(helpActionText());
    dialog->exec();
    delete dialog;
  }
}

Wizard::HelpAction Wizard::helpAction() const
{
  return m_helpAction;
}

void Wizard::setHelpAction(HelpAction a_helpAction)
{
  m_helpAction = a_helpAction;
}

QString Wizard::helpActionText() const
{
  return m_helpActionText;
}

void Wizard::setHelpActionText(const QString& a_helpActionText)
{
  m_helpActionText = a_helpActionText;
}


void Wizard::showEvent(QShowEvent *e)
{
  Q3Wizard::showEvent( e );
  emit widgetOpened();
}

void Wizard::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

QString Wizard::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::setEnabled:
      setFinishEnabled(page(pageCount() - 1), args[0] != "false");
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}


#include "wizard.moc"
