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
#include <qlabel.h>
#include <qtabwidget.h>
#include <qmainwindow.h>

/* OTHER INCLUDES */
#include "instance.h"
#include <kommanderwidget.h>
#include <kommanderwindow.h>
#include <kommanderfactory.h>
#include <specials.h>
#include <fileselector.h>

Instance::Instance()
  : DCOPObject("KommanderIf"), m_instance(0), m_textInstance(0), m_parent(0),
  m_cmdArguments(0)
{
  SpecialInformation::registerSpecials();
}

Instance::Instance(const KURL& a_uiFileName, QWidget *a_parent)
  : DCOPObject("KommanderIf"), m_instance(0), m_textInstance(0), m_uiFileName(a_uiFileName),
  m_parent(a_parent), m_cmdArguments(0)
{
  SpecialInformation::registerSpecials();
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
  
  KommanderWindow* window = dynamic_cast<KommanderWindow*>(m_instance);
  if (window)
    window->setFileName(m_uiFileName.path().local8Bit());

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

  m_textInstance = kommanderWidget(m_instance);
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
     if (KMessageBox::warningYesNo(0, i18n("<qt>This dialog is running from your <i>/tmp</i> directory. "
         " This may mean that it was run from a KMail attachment or from a webpage. "
         "<p>Any script contained in this dialog will have write access to all of your home directory; "
         "<b>running such dialogs may be dangerous: </b>"
         "<p>are you sure you want to continue?</qt>")) == KMessageBox::No)
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
  QObject* child = stringToWidget(widgetName);  
  if (child && child->inherits("QWidget"))
    ((QWidget*)child)->setEnabled(enable);
}

void Instance::setVisible(const QString& widgetName, bool visible)
{
  QObject* child = stringToWidget(widgetName);  
  if (child && child->inherits("QWidget"))
    ((QWidget*)child)->setShown(visible);
}

void Instance::changeWidgetText(const QString& widgetName, const QString& text)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDCOP(DCOP::setText, text);
  else if (child && child->inherits("QLabel"))
  {
    QLabel* label = (QLabel*)child;
    if (label->pixmap())
    {
      QPixmap pixmap;
      if (pixmap.load(text))
        label->setPixmap(pixmap);
    }
    else
      label->setText(text);
  }
}

QString Instance::text(const QString& widgetName)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDCOP(DCOP::text);
  else if (child && child->inherits("QLabel"))
    return ((QLabel*)child)->text();  
  return QString::null;
}

void Instance::setSelection(const QString& widgetName, const QString& text)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDCOP(DCOP::setSelection, text);
  else if (child && child->inherits("QLabel"))
    ((QLabel*)child)->setText(text);  
}

QString Instance::selection(const QString& widgetName)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDCOP(DCOP::selection);
  return QString::null;
}

int Instance::currentItem(const QString &widgetName)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDCOP(DCOP::currentItem).toInt();
  return -1;
}

QString Instance::item(const QString &widgetName, int i)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDCOP(DCOP::item, QString::number(i));
  return QString::null;      
}

void Instance::removeListItem(const QString &widgetName, int index)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDCOP(DCOP::removeItem, QString::number(index));
}

void Instance::addListItem(const QString &widgetName, const QString &item, int index)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
  {
    QStringList args(item);
    args += QString::number(index);
    kommanderWidget(child)->handleDCOP(DCOP::insertItem, args);
  }
}

void Instance::addListItems(const QString &widgetName, const QStringList &items, int index)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
  {
    QStringList args(items.join("\n"));
    args += QString::number(index);
    kommanderWidget(child)->handleDCOP(DCOP::insertItems, args);
  }
}

int Instance::findItem(const QString &widgetName, const QString& item)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDCOP(DCOP::findItem, item).toInt();
  return -1;
}

void Instance::addUniqueItem(const QString &widgetName, const QString &item)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDCOP(DCOP::addUniqueItem, item);
}

int Instance::itemDepth(const QString &widgetName, int index)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDCOP(DCOP::itemDepth, QString::number(index)).toInt();
  return -1;
}

QString Instance::itemPath(const QString &widgetName, int index)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDCOP(DCOP::itemPath, QString::number(index));
  return QString::null;
}
  

void Instance::setPixmap(const QString &widgetName, const QString& iconName, int index)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
  {
    QStringList args(iconName);
    args += QString::number(index);
    kommanderWidget(child)->handleDCOP(DCOP::setPixmap, args);
  }
}

void Instance::clearList(const QString &widgetName)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDCOP(DCOP::clear);
}

void Instance::setCurrentListItem(const QString& widgetName, const QString& item)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDCOP(DCOP::setSelection, item);
}

void Instance::setCurrentTab(const QString &widgetName, int index)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDCOP(DCOP::setCurrentItem, QString::number(index));
}

void Instance::setChecked(const QString &widgetName, bool checked)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDCOP(DCOP::setChecked, checked ? "true" : "false");
}

void Instance::setAssociatedText(const QString &widgetName, const QString& text)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->setAssociatedText(QStringList::split('\n', text, true));
}

QStringList Instance::associatedText(const QString &widgetName)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->associatedText();  
  return QString::null;
}

QString Instance::type(const QString& widget)
{
  QObject* child = stringToWidget(widget);  
  if (child->inherits("QWidget"))
    return child->className();
  return QString::null;
}

QStringList Instance::children(const QString& parent, bool recursive)
{
  QStringList matching;
  QObject* child = stringToWidget(parent);  
  QObjectList* widgets;
  if (!child)
     child = m_instance; 
  if (child->inherits("QWidget"))
  {
    widgets = child->queryList("QWidget", 0, false, recursive);
    for (QObject* w = widgets->first(); w; w = widgets->next())
      if (w->name() && kommanderWidget(w))
        matching.append(w->name());
  }
  return matching;
} 
  
QString Instance::global(const QString& variableName)
{
  return KommanderWidget::global(variableName);
}

void Instance::setGlobal(const QString& variableName, const QString& value)
{
  KommanderWidget::setGlobal(variableName, value); 
}  

QObject* Instance::stringToWidget(const QString& name)
{
  return m_instance->child(name);
}

KommanderWidget* Instance::kommanderWidget(QObject* object)
{
  return dynamic_cast<KommanderWidget*>(object);  
}
  
#include "instance.moc"
