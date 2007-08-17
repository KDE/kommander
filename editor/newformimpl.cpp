/**********************************************************************
 This file is based on Qt Designer, Copyright (C) 2000 Trolltech AS.  All rights reserved.

 This file may be distributed and/or modified under the terms of the
 GNU General Public License version 2 as published by the Free Software
 Foundation and appearing in the file LICENSE.GPL included in the
 packaging of this file.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

 See http://www.trolltech.com/gpl/ for GPL licensing information.

 Modified for Kommander:
  (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>

**********************************************************************/

#include "newformimpl.h"
#include "mainwindow.h"
#include "pixmapchooser.h"
#include "metadatabase.h"
#include "formwindow.h"
#include "widgetfactory.h"
#include "widgetdatabase.h"
#include "actioneditorimpl.h"
#include "hierarchyview.h"
#include "resource.h"
#include "formfile.h"

#include <q3iconview.h>
#include <qlabel.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qregexp.h>
#include <qpushbutton.h>
#include <stdlib.h>
#include <qcombobox.h>
#include <qworkspace.h>
#include <qmessagebox.h>

#include <klocale.h>
#include <kglobal.h>
#include <kstandarddirs.h>

static int _forms = 0;

FormItem::FormItem(Q3IconView *view, const QString &text)    : NewItem(view, text)
{
}

void FormItem::insert()
{
  QString n = "Form" + QString::number(++_forms);
  FormWindow *fw = 0;
  FormFile *ff = new FormFile(FormFile::createUnnamedFileName(), true);
  fw = new FormWindow(ff, MainWindow::self, MainWindow::self->qWorkspace(), n);
  MetaDataBase::addEntry(fw);
  if (fType == Dialog)
  {
    QWidget *w = WidgetFactory::create(WidgetDatabase::idFromClassName("Dialog"), fw, n.toLatin1());
    fw->setMainContainer(w);
  } 
  else if (fType == Wizard)
  {
    QWidget *w = WidgetFactory::create(WidgetDatabase::idFromClassName("Wizard"), fw, n.toLatin1());
    fw->setMainContainer(w);
  }

  fw->setCaption(n);
  fw->resize(600, 480);
  MainWindow::self->insertFormWindow(fw);

  // the wizard might have changed a lot, lets update everything
  MainWindow::self->actioneditor()->setFormWindow(fw);
  MainWindow::self->objectHierarchy()->setFormWindow(fw, fw);
  fw->killAccels(fw);
  fw->setFocus();
}



CustomFormItem::CustomFormItem(Q3IconView *view, const QString &text)
    : NewItem(view, text)
{
}

static void unifyFormName(FormWindow *fw, QWorkspace *qworkspace)
{
  QStringList lst;
  QWidgetList windows = qworkspace->windowList();
  for (QWidget* w = windows.first(); w; w = windows.next())
    if (w != fw)
      lst << w->name();
  
  if (lst.findIndex(fw->name()) == -1)
    return;
  QString origName = fw->name();
  QString n = origName;
  int i = 1;
  while (lst.findIndex(n) != -1)
    n = origName + QString::number(i++);
  fw->setName(n);
  fw->setCaption(n);
}

void CustomFormItem::insert()
{
  QString filename = templateFileName();
  if (!filename.isEmpty() && QFile::exists(filename))
  {
    Resource resource(MainWindow::self);
    FormFile *ff = new FormFile(filename, true);
    if (!resource.load(ff))
    {
      QMessageBox::information(MainWindow::self, i18n("Load Template"),
          i18n("Could not load form description from template '%1'", filename));
      delete ff;
      return;
    }
    ff->setFileName(QString());
    if (MainWindow::self->formWindow())
    {
      MainWindow::self->formWindow()->setFileName(QString());
      unifyFormName(MainWindow::self->formWindow(), MainWindow::self->qWorkspace());
    }
  }
}


NewForm::NewForm(QWidget *parent, const QString &templatePath)
    : NewFormBase(parent, 0, true)
{
  connect(helpButton, SIGNAL(clicked()), MainWindow::self, SLOT(showDialogHelp()));

  Q3IconViewItem *cur = 0;
  FormItem *fi = new FormItem(templateView, i18n("Dialog"));
  allItems.append(fi);
  fi->setFormType(FormItem::Dialog);
  fi->setPixmap(PixmapChooser::loadPixmap("newform.xpm"));
  fi->setDragEnabled(false);
  cur = fi;
  fi = new FormItem(templateView, i18n("Wizard"));
  allItems.append(fi);
  fi->setFormType(FormItem::Wizard);
  fi->setPixmap(PixmapChooser::loadPixmap("newform.xpm"));
  fi->setDragEnabled(false);


  QStringList searchPaths = KGlobal::dirs()->findDirs("data", "kmdr-editor/templates");
  if (!templatePath.isEmpty())
    searchPaths.append(templatePath);
  
  
  // search each path
  for (QStringList::ConstIterator it = searchPaths.begin(); it != searchPaths.end(); ++it)
  {
    if (!QFile::exists(*it))
      continue;
    QDir dir(*it);
    const QFileInfoList* fileList = dir.entryInfoList(QDir::NoFilter, QDir::DirsFirst | QDir::Name);
    if (fileList)
      for (QFileInfoListIterator fit(*fileList); fit.current(); ++fit)
      {
        QFileInfo* fi = fit.current();
        if (!fi->isFile() || fi->extension() != "kmdr")
          continue;
        QString name = fi->baseName();
        name = name.replace("_", " ");
        CustomFormItem *ci = new CustomFormItem(templateView, name);
        allItems.append(ci);
        ci->setDragEnabled(false);
        ci->setPixmap(PixmapChooser::loadPixmap("newform.xpm"));
        ci->setTemplateFile(fi->absoluteFilePath());
      }
  }
  templateView->viewport()->setFocus();
  templateView->setCurrentItem(cur);
}

void NewForm::accept()
{
  if (!templateView->currentItem())
    return;
  ((NewItem *) templateView->currentItem())->insert();
  NewFormBase::accept();
}


void NewForm::itemChanged(Q3IconViewItem *item)
{
    Q_UNUSED(item);
}

#include "newformimpl.moc"
