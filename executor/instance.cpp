/***************************************************************************
                          instance.cpp  -  running instance of a dialog
                             -------------------
    begin                : Tue Aug 13 2002
    copyright            : (C) 2002 by Marc Britton <consume@optushome.com.au>
                           (C) 2004 by Andras Mantia <amantia@kde.org>
                           (C) 2004 by Michal Rudolf <mrudolf@kdewebdev.org>
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
#include <kapplication.h>
#include <kstandarddirs.h>

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
#include <qbuttongroup.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qmainwindow.h>

/* OTHER INCLUDES */
#include "instance.h"
#include <kommanderwidget.h>
#include <kommanderfactory.h>
#include <fileselector.h>

Instance::Instance()
  : DCOPObject("KommanderIf"), m_instance(0), m_textInstance(0), m_parent(0),
  m_cmdArguments(0)
{
  registerDCOP();
  registerSpecials();
}

Instance::Instance(const KURL& a_uiFileName, QWidget *a_parent)
  : DCOPObject("KommanderIf"), m_instance(0), m_textInstance(0), m_uiFileName(a_uiFileName),
  m_parent(a_parent), m_cmdArguments(0)
{
  registerDCOP();
  registerSpecials();
}

void Instance::addArgument(const QString& argument)
{
  int pos = argument.find('=');
  if (pos == -1)
    KommanderWidget::setGlobal(QString("ARG%1").arg(++m_cmdArguments), argument);
  else 
    KommanderWidget::setGlobal(argument.left(pos), argument.mid(pos+1));
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
 
  if (m_uiFileName.isEmpty())
    return false;

  if (!QFileInfo(m_uiFileName.path()).exists())
  {
    KMessageBox::sorry(0, i18n("<qt>Kommander file<br><b>%1</b><br>does not "
      "exist.</qt>").arg(m_uiFileName.path()));
    return false;
  }

  // create the main instance, must inherit QDialog
  KommanderFactory::loadPlugins();
  m_instance = KommanderFactory::create(m_uiFileName.path());
  if (!m_instance)
  {
    KMessageBox::sorry(0, i18n("<qt>Unable to create dialog from "
      "file<br><b>%1</b></qt>").arg(m_uiFileName.path()));
    return false;
  }
  m_instance->setName(m_uiFileName.path().local8Bit());

  // FIXME : Should verify that all of the widgets in the dialog derive from KommanderWidget
  m_textInstance = dynamic_cast<KommanderWidget *>(m_instance);

  return true;
}

bool Instance::build(QFile *a_file)
{
  if(!(a_file->isOpen()))
    return false;

  delete m_instance;
  m_instance = 0;

  KommanderFactory::loadPlugins();
  m_instance = KommanderFactory::create(a_file);
  if (!m_instance)
  {
    KMessageBox::sorry(0, i18n("Unable to create dialog from input."));
    return false;
  }

  m_textInstance = dynamic_cast<KommanderWidget *>(m_instance);
  return true;
}

bool Instance::run(QFile *a_file)
{
  /* add runtime arguments */
  if (m_cmdArguments) {
    QString args;
    for (uint i=1; i<=m_cmdArguments; i++)
    {
      args += global(QString("ARG%1").arg(i));
      if (i < m_cmdArguments) 
        args += " ";
    }
    KommanderWidget::setGlobal("ARGS", args);
  }
  KommanderWidget::setGlobal("ARGCOUNT", QString("%1").arg(m_cmdArguments));
    
  if (m_uiFileName.directory().startsWith(locateLocal("tmp", "") + "/") ||
      m_uiFileName.directory().startsWith("/tmp/"))
  {
     if (KMessageBox::warningYesNo(0, "<qt>This dialog is running from your <i>/tmp</i> directory. "
         " This may mean that it was run from a KMail attachment or from a webpage. "
         "<p>Any script contained in this dialog will have write access to all of your home directory! "
         "<b>Running such dialogs may be dangerous!</b>"
         "<p>Are you sure you want to continue?</qt>") == KMessageBox::No)
       return false;
  }
  
  if (!m_uiFileName.isEmpty()) 
  {
    KommanderWidget::setGlobal("_KDDIR", m_uiFileName.directory());
    KommanderWidget::setGlobal("_NAME", m_uiFileName.fileName());
  }
  
  if (!m_instance)
    if (!a_file && !build())
      return false;
    else if(a_file && !build(a_file))
      return false;

  if (m_instance->inherits("QDialog"))
    ((QDialog*)m_instance)->exec();
  else if (m_instance->inherits("QMainWindow"))
  {
    kapp->setMainWidget(m_instance);
    ((QMainWindow*)m_instance)->show();
    kapp->exec();
  }
  return true;
}

bool Instance::isBuilt()
{
  return m_instance;
}

void Instance::setUIFileName(const KURL& a_uiFileName)
{
  m_uiFileName = a_uiFileName;
}

void Instance::setParent(QWidget *a_parent)
{
  m_parent = a_parent;
}

void Instance::enableWidget(const QString& widgetName, bool enable)
{
  QObjectList* children = stringToWidget(widgetName);  
  if (children)
    for (QObject* child = children->first(); child; child = children->next())
      ((QWidget*)child)->setEnabled(enable);
  delete children;
}

void Instance::changeWidgetText(const QString& widgetName, const QString& text)
{
  QObjectList* children = stringToWidget(widgetName);  
  if (children)
    for (QObject* child = children->first(); child; child = children->next())
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
      else if (child->inherits("FileSelector"))
        ((FileSelector*)child)->setWidgetText(text);
      else if (child->inherits("QSpinBox"))
        ((QSpinBox*)child)->setValue(text.toInt());
      else if (child->inherits("QListBox"))
      {
        ((QListBox*)child)->clear();
        ((QListBox*)child)->insertStringList(QStringList::split("\n", text));
      }
      else if (child->inherits("QComboBox"))
      {
        ((QComboBox*)child)->clear();
        ((QComboBox*)child)->insertStringList(QStringList::split("\n", text));
      }
    }
  delete children;
}

int Instance::currentItem(const QString &widgetName)
{
  QObjectList* children = stringToWidget(widgetName);  
  if (children)
    for (QObject* child = children->first(); child; child = children->next())
    {
      if (child->inherits("QListBox"))
        return ((QListBox*)child)->currentItem();
      else if (child->inherits("QComboBox"))
        return ((QComboBox*)child)->currentItem();
    }
  delete children;
  return -1;
}

QString Instance::item(const QString &widgetName, int i)
{
  QObjectList* children = stringToWidget(widgetName);  
  if (children && i>=0)
    for (QObject* child = children->first(); child; child = children->next())
    {
      if (child->inherits("QListBox"))
        return (uint)i < ((QListBox*)child)->count() ? ((QListBox*)child)->item(i)->text() : QString::null;
      else if (child->inherits("QComboBox"))
        return i < ((QComboBox*)child)->count() ? ((QComboBox*)child)->text(i) : QString::null;
    }
  delete children;
  return QString::null;
}


void Instance::removeListItem(const QString &widgetName, int index)
{
  QObjectList* children = stringToWidget(widgetName);  
  if (children)
    for (QObject* child = children->first(); child; child = children->next())
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
  QObjectList* children = stringToWidget(widgetName);  
  if (children)
    for (QObject* child = children->first(); child; child = children->next())
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
  QObjectList* children = stringToWidget(widgetName);  
  if (children)
    for (QObject* child = children->first(); child; child = children->next())
    {
      if (child->inherits("QListBox"))
        ((QListBox*)child)->insertStringList(items, index);
      else if (child->inherits("QComboBox"))
        ((QComboBox*)child)->insertStringList(items, index);
    }
  delete children;
}

int Instance::findItem(const QString &widgetName, const QString& item)
{
  QObjectList* children = stringToWidget(widgetName);  
  int value = -1;
  if (children)
    for (QObject* child = children->first(); child; child = children->next())
    {
      if (child->inherits("QListBox"))
      {
        QListBox* list = (QListBox*)child;
        QListBoxItem* found = list->findItem(item, Qt::ExactMatch);
        if (!found) 
          found = list->findItem(item, Qt::BeginsWith);
        if (!found) 
          found = list->findItem(item, Qt::Contains);
        if (found)
          value = list->index(found);
      }
    }
  delete children;
  return value;
}

void Instance::addUniqueItem(const QString &widgetName, const QString &item)
{
  QObjectList* children = stringToWidget(widgetName);  
  if (children)
    for (QObject* child = children->first(); child; child = children->next())
    {
      if (child->inherits("QListBox") && !((QListBox*)child)->findItem(item, Qt::ExactMatch))
        ((QListBox*)child)->insertItem(item);
    }
  delete children;
}

void Instance::clearList(const QString &widgetName)
{
  QObjectList* children = stringToWidget(widgetName);  
  if (children)
    for (QObject* child = children->first(); child; child = children->next())
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
  QObjectList* children = stringToWidget(widgetName);  
  if (children)
    for (QObject* child = children->first(); child; child = children->next())
    {
      if (child->inherits("QListBox"))
      {
        QListBox *box = (QListBox*)child;
        for (uint i = 0; i < box->count(); i++)
          if (box->text(i) == name)
            box->setCurrentItem(i);
      }
      else if (child->inherits("QComboBox"))
      {
        QComboBox *box = (QComboBox*)child;
        for (int i = 0; i < box->count(); i++)
          if (box->text(i) == name)
            box->setCurrentItem(i);
      }
    }
  delete children;
}

void Instance::setCurrentTab(const QString &widgetName, int index)
{
  QObjectList* children = stringToWidget(widgetName);  
  if (children)
    for (QObject* child = children->first(); child; child = children->next())
     ((QTabWidget*)child)->setCurrentPage(index);
  delete children;
}

void Instance::setChecked(const QString &widgetName, bool checked)
{
  QObjectList* children = stringToWidget(widgetName);  
  if (children)
    for (QObject* child = children->first(); child; child = children->next())
    {
      if (child->inherits("QCheckBox"))
        ((QCheckBox*)child)->setChecked(checked);
      else if (child->inherits("QRadioButton"))
        ((QRadioButton*)child)->setChecked(checked);
      else if (child->inherits("QButtonGroup"))
      {
        ((QButtonGroup*)child)->setCheckable(true);
        ((QButtonGroup*)child)->setChecked(checked);
      }
    }
  delete children;
}

void Instance::setAssociatedText(const QString &widgetName, const QString &text)
{
  QObjectList* children = stringToWidget(widgetName);  
  if (children)
    for (QObject* child = children->first(); child; child = children->next()) 
    {
      KommanderWidget* kommanderwidget = dynamic_cast<KommanderWidget*>(child);
      if (kommanderwidget)
        kommanderwidget->setAssociatedText(QStringList::split('\n', text, true));
    }
  delete children;
}

QStringList Instance::associatedText(const QString &widgetName)
{
  QObjectList* children = stringToWidget(widgetName);  
  if (children)
  for (QObject* child = children->first(); child; child = children->next())
  {
    KommanderWidget* kommanderwidget = dynamic_cast<KommanderWidget*>(child);
    if (kommanderwidget)
    {
      delete children;
      return kommanderwidget->associatedText();
    }
  }
  return QStringList();
}

QString Instance::global(const QString& variableName)
{
  return KommanderWidget::global(variableName);
}

void Instance::setGlobal(const QString& variableName, const QString& value)
{
  KommanderWidget::setGlobal(variableName, value); 
}  

QObjectList* Instance::stringToWidget(const QString& name)
{
  if (!m_instance)
    return 0;
  return m_instance->queryList(0, name, false);
}

  
#include "instance.moc"
