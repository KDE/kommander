/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
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

#include "sourcefile.h"
#include <qfile.h>
#include <qtextstream.h>
#include "designerappiface.h"
#include "sourceeditor.h"
#include "metadatabase.h"
#include "languageinterface.h"
#include <qfiledialog.h>
#include <qmessagebox.h>
#include "mainwindow.h"
#include "workspace.h"

#ifndef KOMMANDER
SourceFile::SourceFile( const QString &fn, bool temp, Project *p )
    : filename( fn ), ed( 0 ), fileNameTemp( temp ), timeStamp( 0, p->makeAbsolute( fn ) ), pro( p )
{
#else
SourceFile::SourceFile( const QString &fn, bool temp )
    : filename( fn ), ed( 0 ), fileNameTemp( temp ), timeStamp( 0, fn )
{
#endif

    load();
    iface = 0;
#ifndef KOMMANDER
    pro->addSourceFile( this );
#endif
    MetaDataBase::addEntry( this );
    if ( !temp )
  checkFileName( FALSE );
}

SourceFile::~SourceFile()
{
    delete iface;
}

QString SourceFile::text() const
{
    return txt;
}

void SourceFile::setText( const QString &s )
{
    txt = s;
}

bool SourceFile::save()
{
    if ( fileNameTemp )
  return saveAs();
    if ( !isModified() )
  return TRUE;
    if ( ed )
  ed->save();

#ifndef KOMMANDER
    if ( QFile::exists( pro->makeAbsolute( filename ) ) ) {
  QString fn( pro->makeAbsolute( filename ) );
#else
    if ( QFile::exists( filename  ) ) {
  QString fn( filename );
#endif

#if defined(Q_OS_WIN32)
  fn += ".bak";
#else
  fn += "~";
#endif
#ifndef KOMMANDER
  QFile f(pro->makeRelative(filename));
#else
  QFile f( filename );
#endif
  if ( f.open( IO_ReadOnly ) ) {
      QFile f2( fn );
      if ( f2.open( IO_WriteOnly | IO_Translate ) ) {
    QCString data( f.size() );
    f.readBlock( data.data(), f.size() );
    f2.writeBlock( data );
      }
  }
    }

#ifndef KOMMANDER
    QFile f( pro->makeAbsolute( filename ) );
#else
    QFile f(filename);
#endif
    if ( !f.open( IO_WriteOnly | IO_Translate ) )
  return saveAs();

    QTextStream ts( &f );
    ts << txt;
    timeStamp.update();
    setModified( FALSE );
    return TRUE;
}

bool SourceFile::saveAs()
{
#ifndef KOMMANDER
  LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
#else
  LanguageInterface *iface = MetaDataBase::languageInterface( "C++" );
#endif
    QString filter;
    if ( iface )
  filter = iface->fileFilterList().join(";;");

    QString old = filename;
#ifndef KOMMANDER
    QString fn = QFileDialog::getSaveFileName( pro->makeAbsolute( filename ), filter );
#else
    QString fn = QFileDialog::getSaveFileName( filename , filter );
#endif
    if ( fn.isEmpty() )
  return FALSE;
    fileNameTemp = FALSE;
#ifndef KOMMANDER
    filename = pro->makeRelative( fn );
#else
    filename = fn ;
#endif
    if ( !checkFileName( TRUE ) ) {
  filename = old;
  return FALSE;
    }
#ifndef KOMMANDER
    pro->setModified( TRUE );
#endif
#ifndef KOMMANDER
    timeStamp.setFileName( pro->makeAbsolute( filename ) );
#else
    timeStamp.setFileName( filename );
#endif
    if ( ed )
  ed->setCaption( tr( "Edit %1" ).arg( filename ) );
    setModified( TRUE );
    return save();
}

bool SourceFile::load()
{
#ifndef KOMMANDER
  QFile f( pro->makeAbsolute( filename ) );
#else
  QFile f( filename );
#endif
    if ( !f.open( IO_ReadOnly ) )
  return FALSE;
    QTextStream ts( &f );
    txt = ts.read();
    timeStamp.update();
    return TRUE;
}

DesignerSourceFile *SourceFile::iFace()
{
    if ( !iface )
  iface = new DesignerSourceFileImpl( this );
    return iface;
}

void SourceFile::setEditor( SourceEditor *e )
{
    ed = e;
}

bool SourceFile::isModified() const
{
    if ( !ed )
  return FALSE;
    return ed->isModified();
}

static QMap<QString, int> *extensionCounter;
QString SourceFile::createUnnamedFileName( const QString &extension )
{
    if ( !extensionCounter )
  extensionCounter = new QMap<QString, int>;
    int count = -1;
    QMap<QString, int>::Iterator it;
    if ( ( it = extensionCounter->find( extension ) ) != extensionCounter->end() ) {
  count = *it;
  ++count;
  extensionCounter->replace( extension, count );
    } else {
  count = 1;
  extensionCounter->insert( extension, count );
    }

    return "unnamed" + QString::number( count ) + "." + extension;
}

void SourceFile::setModified( bool m )
{
    if ( !ed )
  return;
    ed->setModified( m );
}

bool SourceFile::closeEvent()
{
    if ( !isModified() && fileNameTemp ) {
#ifndef KOMMANDER
      pro->removeSourceFile( this );
#else
      ;
#endif
  return TRUE;
    }

    if ( !isModified() )
  return TRUE;

    if ( ed )
  ed->save();

    switch ( QMessageBox::warning( MainWindow::self, tr( "Save Code" ),
           tr( "Save changes to '%1'?" ).arg( filename ),
           tr( "&Yes" ), tr( "&No" ), tr( "&Cancel" ), 0, 2 ) ) {
    case 0: // save
  if ( !save() )
      return FALSE;
  break;
    case 1: // don't save
  load();
  if ( ed )
      ed->editorInterface()->setText( txt );
  if ( fileNameTemp )
#ifndef KOMMANDER
    pro->removeSourceFile( this );
#else
  ;
#endif
  MainWindow::self->workspace()->update();
  break;
    case 2: // cancel
  return FALSE;
    default:
  break;
    }
    setModified( FALSE );
    return TRUE;
}

bool SourceFile::close()
{
    if ( !ed )
  return TRUE;
    return ed->close();
}

#ifndef KOMMANDER
Project *SourceFile::project() const
{
    return pro;
}
#endif

void SourceFile::checkTimeStamp()
{
    if ( timeStamp.isUpToDate() )
  return;
    timeStamp.update();
    if ( QMessageBox::information( MainWindow::self, tr( "Qt Designer" ),
           tr( "File '%1' has been changed outside Qt Designer.\n"
               "Do you want to reload it?" ).arg( filename ),
           tr( "&Yes" ), tr( "&No" ) ) == 0 ) {
  load();
  if ( ed )
      ed->editorInterface()->setText( txt );
    }
}

bool SourceFile::checkFileName( bool allowBreak )
{
#ifndef KOMMANDER
    SourceFile *sf = pro->findSourceFile( filename, this );
    if ( sf )
  QMessageBox::warning( MainWindow::self, tr( "Invalid Filename" ),
            tr( "The project already contains a source file with \n"
          "filename '%1'. Please choose a new filename." ).arg( filename ) );
    while ( sf ) {
  LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
  QString filter;
  if ( iface )
      filter = iface->fileFilterList().join(";;");
  QString fn;
  while ( fn.isEmpty() ) {
      fn = QFileDialog::getSaveFileName( pro->makeAbsolute( filename ), filter );
      if ( allowBreak && fn.isEmpty() )
    return FALSE;
  }
  filename = pro->makeRelative( fn );
  sf = pro->findSourceFile( filename, this );
    }
#else
    Q_UNUSED(allowBreak);
#endif
    return TRUE;
}
#include "sourcefile.moc"
