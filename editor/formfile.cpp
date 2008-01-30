/**********************************************************************
 This file is based on Qt Designer, Copyright (C) 2000 Trolltech AS. All rights reserved.

 This file may be distributed and/or modified under the terms of the
 GNU General Public License version 2 as published by the Free Software
 Foundation and appearing in the file LICENSE.GPL included in the
 packaging of this file.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

 See http://www.trolltech.com/gpl/ for GPL licensing information.

 Modified for Kommander:
  (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
  (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>

**********************************************************************/

// Other includes
#include "formfile.h"
#include "timestamp.h"
#include "formwindow.h"
#include "command.h"
#include "mainwindow.h"
#include "resource.h"
#include "workspace.h"

// Qt includes
#include <qfile.h>
#include <qregexp.h>
#include <qstatusbar.h>
#include <qtimer.h>

// KDE includes
#include <kdebug.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstatusbar.h>
#include <kstdguiitem.h>

#include <sys/stat.h>

FormFile::FormFile(const QString &fn, bool temp)
    : filename(fn), fileNameTemp(temp), fw(0)
{
  QTimer::singleShot(0, this, SLOT(init()));
}

void FormFile::init()
{
  connect(this, SIGNAL(addedFormFile(FormFile *)), MainWindow::self->workspace(),
    SLOT(formFileAdded(FormFile*)));
  connect(this, SIGNAL(removedFormFile(FormFile *)), MainWindow::self->workspace(),
    SLOT(formFileRemoved(FormFile*)));
  emit addedFormFile(this);
}

FormFile::~FormFile()
{
  if (formWindow())
    formWindow()->setFormFile(0);
}

void FormFile::setFormWindow(FormWindow *f)
{
  if (f == fw)
    return;
  if (fw)
    fw->setFormFile(0);
  fw = f;
  if (fw)
  fw->setFormFile(this);
}

void FormFile::setFileName(const QString &fn)
{
  if (fn == filename)
    return;
  if (fn.isEmpty()) {
    fileNameTemp = true;
    if (filename.find("unnamed"))
      filename = createUnnamedFileName();
    return;
  }
  filename = fn;
}

FormWindow *FormFile::formWindow() const
{
  return fw;
}

QString FormFile::fileName() const
{
  return filename;
}

QString FormFile::absFileName() const
{
  return filename;
}


bool FormFile::save(bool withMsgBox)
{
  if (!formWindow())
    return true;
  if (fileNameTemp)
    return saveAs();
  if (!isModified())
    return true;
  else if (withMsgBox && !formWindow()->checkCustomWidgets())
    return false;

  Resource resource(MainWindow::self);
  resource.setWidget(formWindow());
  if (!resource.save(filename, false))
  {
    if (KMessageBox::questionYesNo(MainWindow::self, i18n("Failed to save file '%1'.\n"
        "Do you want to use another file name?").arg(filename), QString::null, i18n("Try Another"), i18n("Do Not Try")) == KMessageBox::Yes)
      return saveAs();
    else 
      return false;
  }
  MainWindow::self->statusBar()->message(i18n("'%1' saved.").arg(filename), 3000);
  ::chmod(filename.local8Bit(), S_IRWXU);
  setModified(false);
  return true;
}

bool FormFile::saveAs()
{
  QString f = fileName();
  if(fileNameTemp)
    f = QString(formWindow()->name()).lower() + ".kmdr";
  bool saved = false;
  while (!saved) {
    QString fn = KFileDialog::getSaveFileName(QString::null,
      i18n("*.kmdr|Kommander Files"), MainWindow::self,
      i18n("Save Form '%1' As").arg(formWindow()->name()));
    if (fn.isEmpty())
      return false;
    QFileInfo fi(fn);
    if (fi.extension() != "kmdr")
      fn += ".kmdr";
    fileNameTemp = false;
    filename = fn;

    QFileInfo relfi(filename);
    if (relfi.exists()) {
      if (KMessageBox::warningContinueCancel(MainWindow::self,
         i18n("The file already exists. Do you wish to overwrite it?"),
         i18n("Overwrite File?"), i18n("Overwrite")) == KMessageBox::Continue)
        saved = true;
      else
        filename = f;
    }
    else
      saved = true;
  }
  setModified(true);
  return save();
}

bool FormFile::close()
{
  if (formWindow())
    return formWindow()->close();
  return true;
}

bool FormFile::closeEvent()
{
  if (!isModified())
  {
    emit removedFormFile(this);
    QFile f(filename + ".backup");
    f.remove();
    return true;
  }

  switch (KMessageBox::warningYesNoCancel(MainWindow::self, i18n("Dialog '%1' was modified."
  "Do you want to save it?").arg(filename), i18n("Save File?"), KStdGuiItem::save(), KStdGuiItem::discard())) {
    case KMessageBox::Yes:
      if (!save())
        return false;
    case KMessageBox::No: //fall through
      MainWindow::self->workspace()->update();
      break;
    case KMessageBox::Cancel:
      return false;
    default:
      break;
    }

  emit removedFormFile(this);
  setModified(false);
  QFile f(filename + ".backup");
  f.remove();
  return true;
}

void FormFile::setModified(bool m)
{
  setFormWindowModified(m);
}

bool FormFile::isModified()
{
  return isFormWindowModified();
}

bool FormFile::isFormWindowModified() const
{
  if (!formWindow() || !formWindow()->commandHistory())
    return false;
  return formWindow()->commandHistory()->isModified();
}

void FormFile::setFormWindowModified(bool m)
{
  if (m == isFormWindowModified() || !formWindow() || !formWindow()->commandHistory())
    return;
  formWindow()->commandHistory()->setModified(m);
  emit somethingChanged(this);
}

void FormFile::showFormWindow()
{
  if (formWindow())
  {
    formWindow()->setFocus();
    return;
  }
  MainWindow::self->openFormWindow(filename, true, this);
}


static int ui_counter = 0;
QString FormFile::createUnnamedFileName()
{
  return i18n("unnamed") + QString::number(++ui_counter) + QString(".kmdr");
}

QString FormFile::formName() const
{
  FormFile* that = (FormFile*) this;
  if (formWindow()) {
    that->cachedFormName = formWindow()->name();
    return cachedFormName;
  }
  if (!cachedFormName.isNull())
    return cachedFormName;
  QFile f(filename);
  if (f.open(IO_ReadOnly))
  {
    QTextStream ts(&f);
    QString line;
    QString className;
    while (!ts.eof())
    {
      line = ts.readLine();
      if (!className.isEmpty())
      {
        int end = line.find("</class>");
        if (end == -1)
          className += line;
        else
        {
          className += line.left(end);
          break;
        }
        continue;
      }
      int start;
      if ((start = line.find("<class>")) != -1)
      {
        int end = line.find("</class>");
        if (end == -1)
          className = line.mid(start + 7);
        else
        {
          className = line.mid(start + 7, end - (start + 7));
          break;
        }
      }
    }
    that->cachedFormName =  className;
  }
  if (cachedFormName.isEmpty())
    that->cachedFormName = filename;
  return cachedFormName;
}

void FormFile::formWindowChangedSomehow()
{
  emit somethingChanged(this);
}

#include "formfile.moc"
