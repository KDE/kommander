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
#include <kprocess.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qwizard.h>
#include <qdialog.h>

/* OTHER INCLUDES */
#include <myprocess.h>
#include <kommanderfactory.h>
#include <specials.h>
#include "wizard.h"


Wizard::Wizard(QWidget *a_parent, const char *a_name, bool a_modal, int a_flags)
  : QWizard(a_parent, a_name, a_modal, a_flags), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  states << "initialization";
  states << "destroy";
  setStates(states);
  setDisplayStates(states);

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
  setCaption(a_text);
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
  QWizard::exec();
  emit finished();
}

void Wizard::show()
{
  QWizard::show();
  if (!inEditor)
    initialize();
}

void Wizard::runHelp()
{
  if (helpAction() == Command)
  {
    KProcess proc;
    proc << helpActionText();
    proc.start(KProcess::DontCare, KProcess::NoCommunication);
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
  QWizard::showEvent(e);
  emit widgetOpened();
}

void Wizard::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

QString Wizard::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::setEnabled:
      setFinishEnabled(page(pageCount() - 1), args[0] != "false");
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}



#include "wizard.moc"
