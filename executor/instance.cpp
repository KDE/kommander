/***************************************************************************
                          instance.cpp  -  description
                             -------------------
    begin                : Tue Aug 13 2002
    copyright            : (C) 2002 by Marc Britton <consume@optushome.com.au>
                           (C) 2004 by Andras Mantia <amantia@kde.org>
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
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>

/* QT INCLUDES */
#include <qdialog.h>
#include <qwidget.h>
#include <qstring.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qiodevice.h>

#include <qobjectlist.h>
#include <qbutton.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qtextedit.h>

/* OTHER INCLUDES */
#include "instance.h"
#include <kommanderwidget.h>
#include <kommanderfactory.h>

Instance::Instance()
  : DCOPObject("KommanderIf"), m_instance(0), m_textInstance(0), m_uiFileName(""), m_parent(0)
{
}

Instance::Instance(QString a_uiFileName, QWidget *a_parent)
  : DCOPObject("KommanderIf"), m_instance(0), m_textInstance(0), m_uiFileName(a_uiFileName), m_parent(a_parent)
{
}

Instance::~Instance()
{
  delete m_instance;
}

/** Builds the instance */
bool Instance::build()
{
  delete m_instance;
  m_instance = 0;
 
  if(m_uiFileName.isEmpty())
    return FALSE;

  QFileInfo uiFileInfo(m_uiFileName);
  if(!uiFileInfo.exists())
  {
    KMessageBox::sorry(0, i18n("%1 does not exist.").arg(m_uiFileName), i18n("Error"));
    return FALSE;
  }

  // create the main instance, must inherit QDialog
  KommanderFactory::loadPlugins();
  m_instance = (QDialog *)KommanderFactory::create(m_uiFileName);
  if(!m_instance)
  {
    KMessageBox::sorry(0, i18n("Unable to create dialog from %1.").arg(m_uiFileName));
    return FALSE;
  }

  // FIXME : Should verify that all of the widgets in the dialog derive from KommanderWidget
  m_textInstance = dynamic_cast<KommanderWidget *>(m_instance);

  return TRUE;
}

bool Instance::build(QFile *a_file)
{
	if(!(a_file->isOpen()))
		return FALSE;

	delete m_instance;
	m_instance = 0;

	KommanderFactory::loadPlugins();
	m_instance = (QDialog *)KommanderFactory::create(a_file);
	if(!m_instance)
	{
    	KMessageBox::sorry(0, i18n("Unable to create dialog from input."));
		return FALSE;
	}

	m_textInstance = dynamic_cast<KommanderWidget *>(m_instance);
	return TRUE;
}

/** Builds the instance then executes it */
bool Instance::run(QFile *a_file)
{
  if(!m_instance)
  {
	if(!a_file)
	{
    	if(!build())
            return FALSE;
	}
	else
	{
		if(!build(a_file))
			return FALSE;
	}
  }

#if 0
  m_instance->show();
#else
  m_instance->exec();
#endif
  return TRUE;
}

/** Returns whether the instance is built */
bool Instance::isBuilt()
{
  return (m_instance != 0);
}
/** Sets the UI file name */
void Instance::setUIFileName(QString a_uiFileName)
{
  m_uiFileName = a_uiFileName;
}

/* Sets the Instance's parent */
void Instance::setParent(QWidget *a_parent)
{
  m_parent = a_parent;
}


void Instance::enableWidget(const QString& widgetName, bool enable)
{
  if (!m_instance)
    return;
  QObjectList* children = m_instance->queryList("QWidget", widgetName);
  QObject *child;
  for (child = children->first(); child; child = children->next())
  {
    ((QWidget*)child)->setEnabled(enable);
  }
  delete children;
}

void Instance::changeWidgetText(const QString& widgetName, const QString& text)
{
  if (!m_instance)
    return;
  QObjectList* children = m_instance->queryList(0, widgetName, false);
  QObject *child;
  for (child = children->first(); child; child = children->next())
  {
    if (child->inherits("QButton"))
      ((QButton*)child)->setText(text);
    else if (child->inherits("QLabel"))
      ((QLabel*)child)->setText(text);
    else if (child->inherits("QGroupBox"))
      ((QGroupBox*)child)->setTitle(text);
    else if (child->inherits("QLineEdit"))
      ((QLineEdit*)child)->setText(text);
    else if (child->inherits("QTextEdit"))
      ((QTextEdit*)child)->setText(text);
    else if (child->inherits("QSpinBox"))
      ((QSpinBox*)child)->setValue(text.toInt());
  }
  delete children;
}

void Instance::removeListItem(const QString &widgetName, int index)
{
  if (!m_instance)
    return;
  QObjectList* children = m_instance->queryList(0, widgetName, false);
  QObject *child;
  for (child = children->first(); child; child = children->next())
  {
    if (child->inherits("QListBox"))
      ((QListBox*)child)->removeItem(index);
    else if (child->inherits("QComboBox"))
      ((QComboBox*)child)->removeItem(index);
  }
  delete children;
}

void Instance::addListItem(const QString &widgetName, const QString &item, int index)
{
  if (!m_instance)
    return;
  QObjectList* children = m_instance->queryList(0, widgetName, false);
  QObject *child;
  for (child = children->first(); child; child = children->next())
  {
    if (child->inherits("QListBox"))
      ((QListBox*)child)->insertItem(item, index);
    else if (child->inherits("QComboBox"))
      ((QComboBox*)child)->insertItem(item, index);
  }
  delete children;
}

void Instance::addListItems(const QString &widgetName, const QStringList &items, int index)
{
  if (!m_instance)
    return;
  QObjectList* children = m_instance->queryList(0, widgetName, false);
  QObject *child;
  for (child = children->first(); child; child = children->next())
  {
    if (child->inherits("QListBox"))
      ((QListBox*)child)->insertStringList(items, index);
    else if (child->inherits("QComboBox"))
      ((QComboBox*)child)->insertStringList(items, index);
  }
  delete children;
}

void Instance::clearList(const QString &widgetName)
{
  if (!m_instance)
    return;
  QObjectList* children = m_instance->queryList(0, widgetName, false);
  QObject *child;
  for (child = children->first(); child; child = children->next())
  {
    if (child->inherits("QListBox"))
      ((QListBox*)child)->clear();
    else if (child->inherits("QComboBox"))
      ((QComboBox*)child)->clear();
  }
  delete children;
}

void Instance::setCurrentListItem(const QString& widgetName, const QString& name)
{
  if (!m_instance)
    return;
  QObjectList* children = m_instance->queryList(0, widgetName, false);
  QObject *child;
  for (child = children->first(); child; child = children->next())
  {
    if (child->inherits("QListBox"))
    {
      QListBox *box = (QListBox*)child;
      for (uint i = 0; i < box->count(); i++)
        if (box->text(i) == name)
        {
          box->setCurrentItem(i);
        }
    }
    else if (child->inherits("QComboBox"))
    {
      QComboBox *box = (QComboBox*)child;
      for (int i = 0; i < box->count(); i++)
        if (box->text(i) == name)
        {
          box->setCurrentItem(i);
        }
    }
  }
  delete children;
}

void Instance::setCurrentTab(const QString &widgetName, int index)
{
  if (!m_instance)
    return;
  QObjectList* children = m_instance->queryList("QTabWidget", widgetName, false);
  QObject *child;
  for (child = children->first(); child; child = children->next())
  {
   ((QTabWidget*)child)->setCurrentPage(index);
  }
  delete children;
}

void Instance::setChecked(const QString &widgetName, bool checked)
{
  if (!m_instance)
    return;
  QObjectList* children = m_instance->queryList(0, widgetName, false);
  QObject *child;
  for (child = children->first(); child; child = children->next())
  {
    if (child->inherits("QCheckBox"))
      ((QCheckBox*)child)->setChecked(checked);
    else if (child->inherits("QRadioButton"))
      ((QRadioButton*)child)->setChecked(checked);
  }
  delete children;
}

void Instance::setAssociatedText(const QString &widgetName, const QString &text)
{
  if (!m_instance)
    return;
  QObjectList* children = m_instance->queryList(0, widgetName, false);
  QObject *child;
  KommanderWidget *kommanderwidget;
  for (child = children->first(); child; child = children->next())
  {
    kommanderwidget = dynamic_cast<KommanderWidget*>(child);
    if (kommanderwidget)
    {
      kommanderwidget->setAssociatedText(QStringList::split('\n', text, true));
    }
  }
  delete children;
}

QStringList Instance::associatedText(const QString &widgetName)
{
  if (!m_instance)
    return QStringList();
  QObjectList* children = m_instance->queryList(0, widgetName, false);
  QObject *child;
  KommanderWidget *kommanderwidget;
  QStringList result;
  for (child = children->first(); child; child = children->next())
  {
    kommanderwidget = dynamic_cast<KommanderWidget*>(child);
    if (kommanderwidget)
    {
      result = kommanderwidget->associatedText();
    }
  }
  delete children;
  return result;
}

QString Instance::global(const QString& variableName)
{
  return m_globals[variableName];  
}

void Instance::setGlobal(const QString& variableName, const QString& value)
{
  m_globals.insert(variableName, value); 
}  


#include "instance.moc"
