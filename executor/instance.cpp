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
#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

/* QT INCLUDES */
#include <qdialog.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qiodevice.h>
#include <qlabel.h>
#include <qmainwindow.h>
#include <qobject.h>
#include <qstring.h>
#include <qtabwidget.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QPixmap>

/* OTHER INCLUDES */
#include "instance.h"
#include "kommanderadaptor.h"  //d-bus include
#include "kommanderwidget.h"
#include "kommanderwindow.h"
#include "kommanderfactory.h"
#include "specials.h"
#include "fileselector.h"
#include "kommanderversion.h"

#include "specialinformation.h"

Instance::Instance()
  : m_instance(0), m_textInstance(0), m_parent(0)
{
  SpecialInformation::registerSpecials();
  new KommanderAdaptor(this);
  QDBusConnection::sessionBus().registerObject("/Kommander", this); 
}

Instance::Instance(QWidget *a_parent)
  :  m_instance(0), m_textInstance(0), m_parent(a_parent)
{
  SpecialInformation::registerSpecials();
  new KommanderAdaptor(this);
  QDBusConnection::sessionBus().registerObject("/Kommander", this);  
}

void Instance::addCmdlineArguments(const QStringList& args)
{
  if (!m_textInstance)
    return;
  // Filter out variable arguments ('var=value')
  QStringList stdArgs;
  for (QStringList::ConstIterator it = args.begin(); it != args.end(); ++it)
  {
    int pos = (*it).indexOf('=');
    if (pos != -1)
       m_textInstance->setGlobal((*it).left(pos), (*it).mid(pos+1));
    else
       stdArgs.append(*it);
  }
  int i = 0;
  for (QStringList::ConstIterator it = stdArgs.begin(); it != stdArgs.end(); ++it)
    m_textInstance->setGlobal(QString("_ARG%1").arg(++i), *it);
  m_textInstance->setGlobal("_ARGS", stdArgs.join(" "));
  m_textInstance->setGlobal("_ARGCOUNT", QString::number(stdArgs.count()));
}



Instance::~Instance()
{
  delete m_instance;
}

/** Builds the instance */
bool Instance::build(const KUrl& fname)
{
  delete m_instance;
  m_instance = 0;
  m_textInstance = 0;

  if (!fname.isValid() || !isFileValid(fname))
    return false; // Check if file is correct

  // create the main instance, must inherit QDialog
  KommanderFactory::loadPlugins();

  if (fname.isValid())
    m_instance = KommanderFactory::create(fname.path(), 0L, dynamic_cast<QWidget*>(parent()));
  else
  {
    QFile inputFile;
    inputFile.open(stdin, IO_ReadOnly);
    m_instance = KommanderFactory::create(&inputFile);
  }

  // check if build was successful
  if (!m_instance)
  {
    KMessageBox::sorry(0, i18n("<qt>Unable to create dialog.</qt>"));
    return false;
  }

  KommanderWindow* window = dynamic_cast<KommanderWindow*>((QWidget*)m_instance);  
  if (window)
    window->setFileName(fname.path().toLocal8Bit());

  // FIXME : Should verify that all of the widgets in the dialog derive from KommanderWidget
  m_textInstance = kommanderWidget(m_instance);

  if (!m_textInstance)  // Main dialog/window is not a Kommander widget - look for one
  {
    if (m_instance)
    {
      QObjectList widgets = m_instance->findChildren<QObject*>();
      foreach (QObject *w, widgets)
        if (kommanderWidget(w))
        {
          m_textInstance = kommanderWidget(w);
          break;
        }
    }
    if (!m_textInstance)
    {
      qDebug("Warning: no Kommander widget present!");
      return true;
    }
  }

  if (fname.isValid())
  {
      m_textInstance->setGlobal("KDDIR", fname.directory());
      m_textInstance->setGlobal("NAME", fname.fileName());
      m_textInstance->setGlobal("_PID", QString().setNum(getpid()));
      m_textInstance->setGlobal("VERSION", KOMMANDER_VERSION);
  }
  return true;
}

bool Instance::run()
{
  if (!isBuilt())
    return false;

  // Handle both dialogs and main windows
  if (m_instance->inherits("QDialog"))
    dynamic_cast<QDialog*>((QWidget*)m_instance)->exec();
  else if (m_instance->inherits("QMainWindow"))
  {
    kapp->setMainWidget(m_instance);
    dynamic_cast<QMainWindow*>((QWidget*)m_instance)->show();
    kapp->exec();
  }
  else return false;
  return true;
}

bool Instance::isBuilt() const
{
  return m_instance;
}

void Instance::setParent(QWidget *a_parent)
{
  m_parent = a_parent;
}

bool Instance::isFileValid(const KUrl& fname) const
{
  if (!QFileInfo(fname.path()).exists())
  {
    KMessageBox::sorry(0, i18n("<qt>Kommander file<br><b>%1</b><br>does not "
      "exist.</qt>").arg(fname.path()));
    return false;
  }

  // Check whether extension is *.kmdr
  if (!fname.fileName().endsWith(".kmdr"))
  {
    KMessageBox::error(0, i18n("<qt>This file does not have a <b>.kmdr</b> extension. As a security precaution "
           "Kommander will only run Kommander scripts with a clear identity.</qt>"),
           i18n("Wrong Extension"));
    return false;
  }

  // Check whether file is not in some temporary directory.
  QStringList tmpDirs = KGlobal::dirs()->resourceDirs("tmp");
  tmpDirs += KGlobal::dirs()->resourceDirs("cache");
  tmpDirs.append("/tmp/");
  tmpDirs.append("/var/tmp/");

  bool inTemp = false;
  for (QStringList::ConstIterator I = tmpDirs.begin(); I != tmpDirs.end(); ++I)
    if (fname.directory(KUrl::AppendTrailingSlash).startsWith(*I))
      inTemp = true;

  if (inTemp)
  {
    if (KMessageBox::warningContinueCancel(m_parent, i18n("<qt>This dialog is running from your <i>/tmp</i> directory. "
         " This may mean that it was run from a KMail attachment or from a webpage. "
         "<p>Any script contained in this dialog will have write access to all of your home directory; "
         "<b>running such dialogs may be dangerous: </b>"
             "<p>are you sure you want to continue?</qt>"), QString(), KGuiItem(i18n("Run Nevertheless"))) == KMessageBox::Cancel)
       return false;
  }
  if (!QFileInfo(fname.path()).isExecutable())
  {
    if (KMessageBox::warningContinueCancel(m_parent, i18n("<qt>The Kommander file <i>%1</i> does not have the <b>executable attribute</b> set and could possibly contain dangerous exploits.<p>If you trust the scripting (viewable in kmdr-editor) in this program, make it executable to get rid of this warning.<p>Are you sure you want to continue?</qt>", fname.pathOrUrl()), QString(), KGuiItem(i18n("Run Nevertheless"))) == KMessageBox::Cancel)
       return false;
  }
  return true;
}





// Widget functions
void Instance::setEnabled(const QString& widgetName, bool enable)
{
  QObject* child = stringToWidget(widgetName);
  if (child && child->inherits("QWidget"))
    ((QWidget*)child)->setEnabled(enable);
}

void Instance::setVisible(const QString& widgetName, bool visible)
{
  QObject* child = stringToWidget(widgetName);
  if (child && child->inherits("QWidget"))
    ((QWidget*)child)->setVisible(visible);
}

void Instance::setText(const QString& widgetName, const QString& text)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDBUS(DBUS::setText, text);
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
    return kommanderWidget(child)->handleDBUS(DBUS::text);
  else if (child && child->inherits("QLabel"))
    return ((QLabel*)child)->text();
  return QString();
}

void Instance::setSelection(const QString& widgetName, const QString& text)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDBUS(DBUS::setSelection, text);
  else if (child && child->inherits("QLabel"))
    ((QLabel*)child)->setText(text);
}

QString Instance::selection(const QString& widgetName)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDBUS(DBUS::selection);
  return QString();
}

int Instance::currentItem(const QString &widgetName)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDBUS(DBUS::currentItem).toInt();
  return -1;
}

QString Instance::item(const QString &widgetName, int i)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDBUS(DBUS::item, QString::number(i));
  return QString();      
}

void Instance::removeItem(const QString &widgetName, int index)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDBUS(DBUS::removeItem, QString::number(index));
}

void Instance::insertItem(const QString &widgetName, const QString &item, int index)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
  {
    QStringList args(item);
    args += QString::number(index);
    kommanderWidget(child)->handleDBUS(DBUS::insertItem, args);
  }
}

void Instance::insertItems(const QString &widgetName, const QStringList &items, int index)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
  {
    QStringList args(items.join("\n"));
    args += QString::number(index);
    kommanderWidget(child)->handleDBUS(DBUS::insertItems, args);
  }
}

int Instance::findItem(const QString &widgetName, const QString& item)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDBUS(DBUS::findItem, item).toInt();
  return -1;
}

void Instance::addUniqueItem(const QString &widgetName, const QString &item)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDBUS(DBUS::addUniqueItem, item);
}

int Instance::itemDepth(const QString &widgetName, int index)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDBUS(DBUS::itemDepth, QString::number(index)).toInt();
  return -1;
}

QString Instance::itemPath(const QString &widgetName, int index)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDBUS(DBUS::itemPath, QString::number(index));
  return QString();
}


void Instance::setPixmap(const QString &widgetName, const QString& iconName, int index)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
  {
    QStringList args(iconName);
    args += QString::number(index);
    kommanderWidget(child)->handleDBUS(DBUS::setPixmap, args);
  }
}

void Instance::clear(const QString &widgetName)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDBUS(DBUS::clear);
}

void Instance::setCurrentItem(const QString &widgetName, int index)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDBUS(DBUS::setCurrentItem, QString::number(index));
}

void Instance::setChecked(const QString &widgetName, bool checked)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDBUS(DBUS::setChecked, checked ? "true" : "false");
}

bool Instance::checked(const QString &widgetName)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDBUS(DBUS::checked, widgetName) == "1";
  return false;
}

void Instance::setAssociatedText(const QString &widgetName, const QString& text)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->setAssociatedText(text.split('\n', QString::KeepEmptyParts));
}

QStringList Instance::associatedText(const QString &widgetName)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->associatedText();  
  return QStringList();
}

QString Instance::type(const QString& widget)
{
  QObject* child = stringToWidget(widget);  
  if (child && child->inherits("QWidget"))
    return child->metaObject()->className();
  return QString();
}

QStringList Instance::children(const QString& parent, bool recursive)
{
  QStringList matching;
  QObject* child = stringToWidget(parent);  
  QList<QWidget *> widgets;
  if (!child)
     child = m_instance; 
  if (child->inherits("QWidget"))
  {
    widgets = child->findChildren<QWidget *>();
    QListIterator<QWidget *> widgetsit(widgets);
    while(widgetsit.hasNext())
    {
      QWidget* w = widgetsit.next();
      if (!w->objectName().isEmpty() && kommanderWidget(w))
        matching.append(w->objectName());
    }
  }
  return matching;
}

void Instance::setMaximum(const QString &widgetName, int value)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDBUS(DBUS::setMaximum, QString::number(value));
}

QString Instance::execute(const QString &widgetName)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDBUS(DBUS::execute);
  return "";
}

void Instance::cancel(const QString &widgetName)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDBUS(DBUS::cancel);
}

int Instance::count(const QString &widgetName)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDBUS(DBUS::count).toInt();
  return -1;
}

int Instance::currentColumn(const QString &widgetName)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDBUS(DBUS::currentColumn).toInt();
  return -1;
}

int Instance::currentRow(const QString &widgetName)
{
  QObject* child = stringToWidget(widgetName);
  if (kommanderWidget(child))
    return kommanderWidget(child)->handleDBUS(DBUS::currentRow).toInt();
  return -1;
}

void Instance::insertRow(const QString &widgetName, int row, int count)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
  {
    QStringList args(QString::number(row));
    args += QString::number(count);
    kommanderWidget(child)->handleDBUS(DBUS::insertRow, args);
  }
}

void Instance::insertColumn(const QString &widgetName, int column, int count)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
  {
    QStringList args(QString::number(column));
    args += QString::number(count);
    kommanderWidget(child)->handleDBUS(DBUS::insertColumn, args);
  }
}

void Instance::setCellText(const QString &widgetName, int row, int column, const QString& text)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
  {
    QStringList args(QString::number(row));
    args += QString::number(column);
    args += text;
    kommanderWidget(child)->handleDBUS(DBUS::setCellText, args);
  }
}

QString Instance::cellText(const QString &widgetName, int row, int column)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
  {
    QStringList args(QString::number(row));
    args += QString::number(column);
    return kommanderWidget(child)->handleDBUS(DBUS::cellText, args);
  }
  else
    return QString();
}

void Instance::removeRow(const QString &widgetName, int row, int count)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
  {
    QStringList args(QString::number(row));
    args += QString::number(count);
    kommanderWidget(child)->handleDBUS(DBUS::removeRow, args);
  }
}

void Instance::removeColumn(const QString &widgetName, int column, int count)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
  {
    QStringList args(QString::number(column));
    args += QString::number(count);
    kommanderWidget(child)->handleDBUS(DBUS::removeColumn, args);
  }
}

void Instance::setRowCaption(const QString &widgetName, int row, const QString& text)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
  {
    QStringList args(QString::number(row));
    args += text;
    kommanderWidget(child)->handleDBUS(DBUS::setRowCaption, args);
  }
}

void Instance::setColumnCaption(const QString &widgetName, int column, const QString& text)
{
  QObject* child = stringToWidget(widgetName);  
  if (kommanderWidget(child))
  {
    QStringList args(QString::number(column));
    args += text;
    kommanderWidget(child)->handleDBUS(DBUS::setColumnCaption, args);
  }
}










QString Instance::global(const QString& variableName)
{
  return m_textInstance ? m_textInstance->global(variableName) : QString();
}

void Instance::setGlobal(const QString& variableName, const QString& value)
{
  if (m_textInstance)
    m_textInstance->setGlobal(variableName, value);
}

QObject* Instance::stringToWidget(const QString& name)
{
  return m_instance->findChild<QObject *>(name);
}

KommanderWidget* Instance::kommanderWidget(QObject* object)
{
  return dynamic_cast<KommanderWidget*>(object);
}


/*** Deprecated methods: just call appropriate method  ***/
void Instance::changeWidgetText(const QString& widgetName, const QString& text)
{
  setText(widgetName, text);
}

void Instance::clearList(const QString &widgetName)
{
  clear(widgetName);
}

void Instance::setCurrentListItem(const QString& widgetName, const QString& item)
{
  setSelection(widgetName, item);
}

void Instance::setCurrentTab(const QString &widgetName, int index)
{
  setCurrentItem(widgetName, index);
}

void Instance::insertTab(const QString &widgetName, const QString &label, int index)
{
  QObject* child = stringToWidget(widgetName);  
  QStringList l;
  l << label;
  l << QString::number(index);
  if (kommanderWidget(child))
    kommanderWidget(child)->handleDBUS(DBUS::insertTab, l);  
}

void Instance::addListItems(const QString &widgetName, const QStringList &items, int index)
{
  insertItems(widgetName, items, index);
}

void Instance::enableWidget(const QString& widgetName, bool enable)
{
  setEnabled(widgetName, enable);
}

void Instance::removeListItem(const QString &widgetName, int index)
{
  removeItem(widgetName, index);
}

void Instance::addListItem(const QString & widgetName, const QString & item, int index)
{
  insertItem(widgetName, item, index);
}

int Instance::getWinID()
{
  return m_instance->winId();
}

void Instance::setBusyCursor(bool busy)
{
  if (busy)
    m_instance->setCursor(QCursor(Qt::WaitCursor));
 else
    m_instance->setCursor(QCursor(Qt::ArrowCursor));
}


#include "instance.moc"
