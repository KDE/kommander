/***************************************************************************
                          instance.cpp  -  description
                             -------------------
    begin                : Tue Aug 13 2002
    copyright            : (C) 2002 by Marc Britton
    email                : consume@optushome.com.au
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
#include <kmessagebox.h>

/* QT INCLUDES */
#include <qdialog.h>
#include <qwidget.h>
#include <qstring.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qiodevice.h>

/* OTHER INCLUDES */
#include "instance.h"
#include "../widgets/assoctextwidget.h"
#include "ewidgetfactory.h"

Instance::Instance()
  : m_instance(0), m_textInstance(0), m_uiFileName(""), m_parent(0)
{
}

Instance::Instance(QString a_uiFileName, QWidget *a_parent)
  : m_instance(0), m_textInstance(0), m_uiFileName(a_uiFileName), m_parent(a_parent)
{
}

Instance::~Instance()
{
  if(m_instance)
    delete m_instance;
}

/** Builds the instance */
bool Instance::build()
{
  if(m_instance)
  {
    delete m_instance;
    m_instance = 0;
  }

  if(m_uiFileName.isEmpty())
    return FALSE;
    
  QFileInfo uiFileInfo(m_uiFileName);
  if(!uiFileInfo.exists())
  {
    KMessageBox::sorry(0, m_uiFileName + " does not exist", "Error");
    return FALSE;
  }
  
  // create the main instance, must inherit QDialog
  m_instance = (QDialog *)EWidgetFactory::create(m_uiFileName);
  if(!m_instance)
  {
    KMessageBox::sorry(0, QString("Unable to create dialog from ") + m_uiFileName);
    return FALSE;
  }

  // FIXME : Should verify that all of the widgets in the dialog derive from AssocTextWidget
  m_textInstance = dynamic_cast<AssocTextWidget *>(m_instance);

  return TRUE;
}

bool Instance::build(QFile *a_file)
{
	if(!(a_file->isOpen()))
		return FALSE;

	if(m_instance)
	{
		delete m_instance;
		m_instance = 0;
	}

	m_instance = (QDialog *)EWidgetFactory::create(a_file);
	if(!m_instance)
	{
    	KMessageBox::sorry(0, QString("Unable to create dialog from input")); 
		return FALSE;
	}

	m_textInstance = dynamic_cast<AssocTextWidget *>(m_instance);
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
