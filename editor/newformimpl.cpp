/**********************************************************************
** Copyright (C) 2000-2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "newformimpl.h"
#include "mainwindow.h"
#include "pixmapchooser.h"
#include "metadatabase.h"
#ifndef KOMMANDER
#include "project.h"
#endif
#include "formwindow.h"
#include "widgetfactory.h"
#include "widgetdatabase.h"
#include "actioneditorimpl.h"
#include "hierarchyview.h"
#include "resource.h"
#ifndef KOMMANDER
#include "projectsettingsimpl.h"
#endif
#ifndef KOMMANDER
#include "sourcefile.h"
#endif
#include "formfile.h"

#include <qiconview.h>
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

static int _forms = 0;

#ifndef KOMMANDER
ProjectItem::ProjectItem( QIconView *view, const QString &text )
    : NewItem( view, text )
{
}

void ProjectItem::insert( Project * )
{
    MainWindow::self->createNewProject( lang );
}
#endif


FormItem::FormItem( QIconView *view, const QString &text )
    : NewItem( view, text )
{
}

#ifndef KOMMANDER
void FormItem::insert( Project *pro )
#else
void FormItem::insert()
#endif
{
    QString n = "Form" + QString::number( ++_forms );
    FormWindow *fw = 0;
#ifndef KOMMANDER
    FormFile *ff = new FormFile( FormFile::createUnnamedFileName(), TRUE, pro );
#else
    FormFile *ff = new FormFile( FormFile::createUnnamedFileName(), TRUE );
#endif
    fw = new FormWindow( ff, MainWindow::self, MainWindow::self->qWorkspace(), n );
#ifndef KOMMANDER
    fw->setProject( pro );
#endif
    MetaDataBase::addEntry( fw );
#ifndef KOMMANDER
    if ( fType == Widget ) {
  QWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QWidget" ),
              fw, n.latin1() );
  fw->setMainContainer( w );
    } else if ( fType == Dialog ) {
  QWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QDialog" ), fw, n.latin1() );
  fw->setMainContainer( w );
    } else if ( fType == Wizard ) {
  QWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QWizard" ),
              fw, n.latin1() );
  fw->setMainContainer( w );
    } else if ( fType == MainWindow ) {
  QWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QMainWindow" ),
              fw, n.latin1() );
  fw->setMainContainer( w );
    }
#else
  if ( fType == Dialog )
  {
    QWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "Dialog" ), fw, n.latin1() );
    fw->setMainContainer( w );
  }
  else if ( fType == Wizard )
  {
      QWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QWizard" ), fw, n.latin1() );
      fw->setMainContainer( w );
    }
#endif

    fw->setCaption( n );
    fw->resize( 600, 480 );
    MainWindow::self->insertFormWindow( fw );

#ifndef KOMMANDER
    TemplateWizardInterface *iface =
  MainWindow::self->templateWizardInterface( fw->mainContainer()->className() );
    if ( iface ) {
  iface->setup( fw->mainContainer()->className(), fw->mainContainer(),
          fw->iFace(), MainWindow::self->designerInterface() );
  iface->release();
    }
#endif

    // the wizard might have changed a lot, lets update everything
    MainWindow::self->actioneditor()->setFormWindow( fw );
    MainWindow::self->objectHierarchy()->setFormWindow( fw, fw );
#ifndef KOMMANDER
  MainWindow::self->objectHierarchy()->formDefinitionView()->refresh();
#endif
    fw->killAccels( fw );
#ifndef KOMMANDER
    fw->project()->setModified( TRUE );
#endif
    fw->setFocus();
#ifndef KOMMANDER
    if ( !pro->isDummy() ) {
  fw->setSavePixmapInProject( TRUE );
  fw->setSavePixmapInline( FALSE );
    }
#endif
}



CustomFormItem::CustomFormItem( QIconView *view, const QString &text )
    : NewItem( view, text )
{
}

static void unifyFormName( FormWindow *fw, QWorkspace *qworkspace )
{
    QStringList lst;
    QWidgetList windows = qworkspace->windowList();
    for ( QWidget *w =windows.first(); w; w = windows.next() ) {
  if ( w == fw )
      continue;
  lst << w->name();
    }

    if ( lst.findIndex( fw->name() ) == -1 )
  return;
    QString origName = fw->name();
    QString n = origName;
    int i = 1;
    while ( lst.findIndex( n ) != -1 ) {
  n = origName + QString::number( i++ );
    }
    fw->setName( n );
    fw->setCaption( n );
}

#ifndef KOMMANDER
void CustomFormItem::insert( Project *pro )
#else
void CustomFormItem::insert()
#endif
{
    QString filename = templateFileName();
    if ( !filename.isEmpty() && QFile::exists( filename ) ) {
  Resource resource( MainWindow::self );
#ifndef KOMMANDER
  FormFile *ff = new FormFile( filename, TRUE, pro );
#else
  FormFile *ff = new FormFile( filename, TRUE );
#endif
  if ( !resource.load( ff ) ) {
      QMessageBox::information( MainWindow::self, i18n("Load Template"),
                                i18n("Could not load form description from template '%1'").arg(filename) );
      delete ff;
      return;
  }
  ff->setFileName( QString::null );
  if ( MainWindow::self->formWindow() ) {
      MainWindow::self->formWindow()->setFileName( QString::null );
      unifyFormName( MainWindow::self->formWindow(), MainWindow::self->qWorkspace() );
#ifndef KOMMANDER
      if ( !pro->isDummy() ) {
    MainWindow::self->formWindow()->setSavePixmapInProject( TRUE );
    MainWindow::self->formWindow()->setSavePixmapInline( FALSE );
      }
#endif
  }
    }
}

#ifndef KOMMANDER
SourceFileItem::SourceFileItem( QIconView *view, const QString &text )
    : NewItem( view, text ), visible( TRUE )
{
}

void SourceFileItem::insert( Project *pro )
{
    SourceFile *f = new SourceFile( SourceFile::createUnnamedFileName( ext ), TRUE, pro );
    MainWindow::self->editSource( f );
}

void SourceFileItem::setProject( Project *pro )
{
    bool v = lang == pro->language();
    if ( v == visible )
  return;
    visible = v;
    if ( !visible )
  iconView()->takeItem( this );
    else
  iconView()->insertItem( this );
}



SourceTemplateItem::SourceTemplateItem( QIconView *view, const QString &text )
    : NewItem( view, text ), visible( TRUE )
{
}

void SourceTemplateItem::insert( Project *pro )
{
    SourceTemplateInterface *siface = MainWindow::self->sourceTemplateInterface( text() );
    if ( !siface )
  return;
    SourceTemplateInterface::Source src = siface->create( text(), MainWindow::self->designerInterface() );
    SourceFile *f = 0;
    if ( src.type == SourceTemplateInterface::Source::Invalid )
  return;
    if ( src.type == SourceTemplateInterface::Source::FileName )
  f = new SourceFile( src.filename, FALSE, pro );
    else
  f = new SourceFile( SourceFile::createUnnamedFileName( src.extension ), TRUE, pro );
    f->setText( src.code );
    MainWindow::self->editSource( f );
    f->setModified( TRUE );
}

void SourceTemplateItem::setProject( Project *pro )
{
    bool v = !pro->isDummy() && lang == pro->language();
    if ( v == visible )
  return;
    visible = v;
    if ( !visible )
  iconView()->takeItem( this );
    else
  iconView()->insertItem( this );
}
#endif

#ifndef KOMMANDER
NewForm::NewForm( QWidget *parent, const QStringList& projects,
      const QString& currentProject, const QString &templatePath )
#else
NewForm::NewForm( QWidget *parent, const QString &templatePath )
#endif
    : NewFormBase( parent, 0, TRUE )
{
    connect( helpButton, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );

#ifndef KOMMANDER
    projectCombo->insertStringList( projects );
    projectCombo->setCurrentText( currentProject );
#endif

#ifndef KOMMANDER
    QStringList languages = MetaDataBase::languages();
    QStringList::Iterator it;
    for ( it = languages.begin(); it != languages.end(); ++it ) {
  ProjectItem *pi = new ProjectItem( templateView, *it + " " + i18n("Project" ) );
  allItems.append( pi );
  pi->setLanguage( *it );
  pi->setPixmap( PixmapChooser::loadPixmap( "project.xpm" ) );
  pi->setDragEnabled( FALSE );
    }
#endif

    QIconViewItem *cur = 0;
    FormItem *fi = new FormItem( templateView,i18n("Dialog" ) );
    allItems.append( fi );
    fi->setFormType( FormItem::Dialog );
    fi->setPixmap( PixmapChooser::loadPixmap( "newform.xpm" ) );
    fi->setDragEnabled( FALSE );
    cur = fi;
    fi = new FormItem( templateView,i18n("Wizard" ) );
    allItems.append( fi );
    fi->setFormType( FormItem::Wizard );
    fi->setPixmap( PixmapChooser::loadPixmap( "newform.xpm" ) );
    fi->setDragEnabled( FALSE );
#ifndef KOMMANDER
    fi = new FormItem( templateView, i18n("Widget" ) );
    allItems.append( fi );
    fi->setFormType( FormItem::Widget );
    fi->setPixmap( PixmapChooser::loadPixmap( "newform.xpm" ) );
    fi->setDragEnabled( FALSE );
    fi = new FormItem( templateView, i18n("Main Window" ) );
    allItems.append( fi );
    fi->setFormType( FormItem::MainWindow );
    fi->setPixmap( PixmapChooser::loadPixmap( "newform.xpm" ) );
    fi->setDragEnabled( FALSE );
#endif

    QString templPath = templatePath;
    if ( templPath.isEmpty() || !QFileInfo( templPath ).exists() ) {
  QStringList templRoots;
  const char *qtdir = getenv( "QTDIR" ); // FIXME
  if(qtdir)
      templRoots << qtdir;
#ifdef QT_INSTALL_PREFIX
  templRoots << QT_INSTALL_PREFIX;
#endif
#ifdef QT_INSTALL_DATA
  templRoots << QT_INSTALL_DATA;
#endif
  if(qtdir) //try the tools/designer directory last!
      templRoots << QString(qtdir) + "/tools/designer";
  for ( QStringList::Iterator it = templRoots.begin(); it != templRoots.end(); ++it ) {
      QString path = (*it) + "/templates";
      if ( QFile::exists( path )) {
    templPath = path;
    break;
      }
  }
    }
    if ( !templPath.isEmpty() ) {
  QDir dir( templPath  );
  const QFileInfoList *filist = dir.entryInfoList( QDir::DefaultFilter, QDir::DirsFirst | QDir::Name );
  if ( filist ) {
      QFileInfoListIterator it( *filist );
      QFileInfo *fi;
      while ( ( fi = it.current() ) != 0 ) {
    ++it;
    if ( !fi->isFile() || fi->extension() != "ui" )
        continue;
    QString name = fi->baseName();
    name = name.replace( QRegExp( "_" ), " " );
    CustomFormItem *ci = new CustomFormItem( templateView, name );
    allItems.append( ci );
    ci->setDragEnabled( FALSE );
    ci->setPixmap( PixmapChooser::loadPixmap( "newform.xpm" ) );
    ci->setTemplateFile( fi->absFilePath() );
      }
  }
    }

#ifndef KOMMANDER
    for ( it = languages.begin(); it != languages.end(); ++it ) {
  LanguageInterface *iface = MetaDataBase::languageInterface( *it );
  if ( iface ) {
      QMap<QString, QString> extensionMap;
      iface->preferredExtensions( extensionMap );
      for ( QMap<QString, QString>::Iterator eit = extensionMap.begin();
      eit != extensionMap.end(); ++eit ) {
    SourceFileItem * si = new SourceFileItem( templateView, *eit );
    allItems.append( si );
    si->setExtension( eit.key() );
    si->setLanguage( *it );
    si->setPixmap( PixmapChooser::loadPixmap( "filenew.xpm" ) );
    si->setDragEnabled( FALSE );
      }
      iface->release();
  }
    }

    QStringList sourceTemplates = MainWindow::self->sourceTemplates();
    for ( QStringList::Iterator sit = sourceTemplates.begin(); sit != sourceTemplates.end(); ++sit ) {
  SourceTemplateInterface *siface = MainWindow::self->sourceTemplateInterface( *sit );
  if ( !siface )
      continue;
  SourceTemplateItem * si = new SourceTemplateItem( templateView, *sit );
  allItems.append( si );
  si->setTemplate( *sit );
  si->setLanguage( siface->language( *sit ) );
  si->setPixmap( PixmapChooser::loadPixmap( "filenew.xpm" ) );
  si->setDragEnabled( FALSE );
  siface->release();
    }
#endif

    templateView->viewport()->setFocus();
#ifndef KOMMANDER
    projectChanged( projectCombo->currentText() );
#endif
    templateView->setCurrentItem( cur );
}

void NewForm::accept()
{
    if ( !templateView->currentItem() )
  return;
#ifndef KOMMANDER
    Project *pro = MainWindow::self->findProject( projectCombo->currentText() );
    if ( !pro )
  return;
    MainWindow::self->setCurrentProject( pro );
#endif
    ( (NewItem*)templateView->currentItem() )->insert( );

    NewFormBase::accept();
}

#ifndef KOMMANDER // FIXME
void NewForm::projectChanged( const QString &project )
{
    Project *pro = MainWindow::self->findProject( project );
    if ( !pro )
  return;
    QIconViewItem *i;
    for ( i = allItems.first(); i; i = allItems.next() )
  ( (NewItem*)i )->setProject( pro );
    templateView->setCurrentItem( templateView->firstItem() );
    templateView->arrangeItemsInGrid( TRUE );
}
#endif

void NewForm::itemChanged( QIconViewItem *item )
{
#ifndef KOMMANDER
    labelProject->setEnabled( item->rtti() != NewItem::ProjectType );
    projectCombo->setEnabled( item->rtti() != NewItem::ProjectType );
#else
    Q_UNUSED(item);
#endif
}
#include "newformimpl.moc"
