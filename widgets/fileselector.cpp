/***************************************************************************
                         fileselector.cpp - File selection widget 
                             -------------------
    copyright            : (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
                           (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
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
#include <kfiledialog.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <kurlcompletion.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qlayout.h>
#include <qsizepolicy.h>
//Added by qt3to4:
#include <QShowEvent>
#include <QHBoxLayout>

/* OTHER INCLUDES */
#include <specials.h>
#include "fileselector.h"

FileSelector::FileSelector(QWidget * a_parent, const char *a_name)
   : QWidget(a_parent), KommanderWidget(this)
{
  setObjectName(a_name);
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);

  m_lineEdit = new KLineEdit(this);
  
  m_selectButton = new KPushButton("...", this);

  m_boxLayout = new QHBoxLayout(this);
  m_boxLayout->setMargin(0);
  m_boxLayout->setSpacing(11);
  m_boxLayout->addWidget(m_lineEdit);
  m_boxLayout->addWidget(m_selectButton);

  setSizePolicy(m_lineEdit->sizePolicy());
  m_lineEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  m_selectButton->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));

  connect(m_selectButton, SIGNAL(clicked()), this, SLOT(makeSelection()));

  setSelectionType(Open);
  setSelectionOpenMultiple(false);
}

FileSelector::~FileSelector()
{
}

QString FileSelector::currentState() const
{
  return QString("default");
}

bool FileSelector::isKommanderWidget() const
{
  return true;
}

QStringList FileSelector::associatedText() const
{
  return KommanderWidget::associatedText();
}

void FileSelector::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void FileSelector::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString FileSelector::populationText() const
{
  return KommanderWidget::populationText();
}

void FileSelector::populate()
{
  QString txt = KommanderWidget::evalAssociatedText(populationText());
  setWidgetText(txt);
}

void FileSelector::setWidgetText(const QString& a_text)
{
  m_lineEdit->setText(a_text);
  emit widgetTextChanged(a_text);
}

FileSelector::SelectionType FileSelector::selectionType() const
{
  return m_selectionType;
}

void FileSelector::setSelectionType(SelectionType a_selectionType)
{
  m_selectionType = a_selectionType;
  delete m_lineEdit->completionObject();
  if (m_selectionType == Directory) 
     m_lineEdit->setCompletionObject(new KUrlCompletion(KUrlCompletion::DirCompletion));
  else
     m_lineEdit->setCompletionObject(new KUrlCompletion(KUrlCompletion::FileCompletion));
  
}

QString FileSelector::selectionFilter() const
{
  return m_filter;
}

void FileSelector::setSelectionFilter(const QString& a_filter)
{
  m_filter = a_filter;
}

QString FileSelector::selectionCaption() const
{
  return m_caption;
}

void FileSelector::setSelectionCaption(const QString& a_caption)
{
  m_caption = a_caption;
}

bool FileSelector::selectionOpenMultiple() const
{
  return m_openMultiple;
}

void FileSelector::setSelectionOpenMultiple(bool a_openMultiple)
{
  m_openMultiple = a_openMultiple;
}

void FileSelector::makeSelection()
{
  QString text;
  if (m_selectionType == Open && m_openMultiple)
    text = KFileDialog::getOpenFileNames(m_lineEdit->text(), m_filter, this, m_caption).join("\n");
  else if (m_selectionType == Open && !m_openMultiple)
    text = KFileDialog::getOpenFileName(m_lineEdit->text(), m_filter, this, m_caption);
  else if (m_selectionType == Save)
    text = KFileDialog::getSaveFileName(m_lineEdit->text(), m_filter, this, m_caption);
  else if (m_selectionType == Directory)
    text = KFileDialog::getExistingDirectory(m_lineEdit->text(), this, m_caption);
  
  if (!text.isEmpty())
    setWidgetText(text);
}

QString FileSelector::caption() const
{
  return m_selectButton->text();
}

void FileSelector::setCaption(const QString& a_text)
{
  m_selectButton->setText(a_text);
}

bool FileSelector::showEditor() const
{
  return m_lineEdit->isVisible();
}

void FileSelector::setShowEditor(bool a_visible)
{
  if (a_visible)
    m_lineEdit->show();
  else
    m_lineEdit->hide();
}

void FileSelector::showEvent(QShowEvent * e)
{
  QWidget::showEvent(e);
  emit widgetOpened();
}

void FileSelector::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

bool FileSelector::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::setText || f == DBUS::selection || f == DBUS::setSelection ||
      f == DBUS::clear;
}

QString FileSelector::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
      return m_lineEdit->text();
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    case DBUS::selection:
      return m_lineEdit->selectedText();
    case DBUS::setSelection:
    { 
      int f = m_lineEdit->text().indexOf(args[0]);
      if (f != -1)
        m_lineEdit->setSelection(f, args[0].length());
      break;
    }
    case DBUS::clear:
      setWidgetText(QString::null);	//krazy:exclude=nullstrassign for old broken gcc
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}


#include "fileselector.moc"
