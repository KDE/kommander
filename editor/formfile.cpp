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

#include "formfile.h"
#include "timestamp.h"
#ifndef KOMMANDER
#include "project.h"
#endif
#include "formwindow.h"
#include "command.h"
#include "sourceeditor.h"
#include "mainwindow.h"
#include "languageinterface.h"
#include "resource.h"
#include "workspace.h"
#include <qmessagebox.h>
#include <qfile.h>
#include <qregexp.h>
#include <qstatusbar.h>

#ifdef HAVE_KDE
#include <kstatusbar.h>
#include <kfiledialog.h>
#endif

#ifndef KOMMANDER
static QString make_func_pretty( const QString &s )
{
    QString res = s;
    if ( res.find( ")" ) - res.find( "(" ) == 1 )
	return res;
    res.replace( QRegExp( "[(]" ), "( " );
    res.replace( QRegExp( "[)]" ), " )" );
    res.replace( QRegExp( "&" ), " &" );
    res.replace( QRegExp( "[*]" ), " *" );
    res.replace( QRegExp( "," ), ", " );
    res.replace( QRegExp( ":" ), " : " );
    res = res.simplifyWhiteSpace();
    return res;
}
#endif

#ifndef KOMMANDER
FormFile::FormFile( const QString &fn, bool temp, Project *p )
    : filename( fn ), fileNameTemp( temp ), pro( p ), fw( 0 ), ed( 0 ),
      timeStamp( 0, fn + codeExtension() ), codeEdited( FALSE ), cm( FALSE )
#else
FormFile::FormFile( const QString &fn, bool temp )
    : filename( fn ), fileNameTemp( temp ), fw( 0 ), ed( 0 ),
      timeStamp( 0, fn + codeExtension() ), codeEdited( FALSE ), cm( FALSE )
#endif
{
#ifndef KOMMANDER
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( iface )
	seperateSource = iface->supports( LanguageInterface::StoreFormCodeSeperate );
    else
	seperateSource = FALSE;
    pro->addFormFile( this );
    loadCode();
    if ( !temp )
	checkFileName( FALSE );
#else
	connect(this, SIGNAL(addedFormFile(FormFile *)), MainWindow::self->workspace(), SLOT(formFileAdded(FormFile *)));
	connect(this, SIGNAL(removedFormFile(FormFile *)), MainWindow::self->workspace(), SLOT(formFileRemoved(FormFile *)));
	emit addedFormFile(this);
    seperateSource = FALSE;
#endif

}

FormFile::~FormFile()
{
#ifndef KOMMANDER
    pro->removeFormFile( this );
#endif
    if ( formWindow() )
	formWindow()->setFormFile( 0 );
}

void FormFile::setFormWindow( FormWindow *f )
{
    if ( f == fw )
	return;
    if ( fw )
	fw->setFormFile( 0 );
    fw = f;
    if ( fw )
	fw->setFormFile( this );
    if ( seperateSource )
	parseCode( cod, FALSE );
}

void FormFile::setEditor( SourceEditor *e )
{
    ed = e;
}

void FormFile::setFileName( const QString &fn )
{
    if ( fn == filename )
	return;
    if ( fn.isEmpty() ) {
	fileNameTemp = TRUE;
	if ( filename.find( "unnamed" ) != 0 )
	    filename = createUnnamedFileName();
	return;
    }
    filename = fn;
    timeStamp.setFileName( filename + codeExtension() );
    cod = "";
    loadCode();
}

void FormFile::setCode( const QString &c )
{
    cod = c;
}

FormWindow *FormFile::formWindow() const
{
    return fw;
}

SourceEditor *FormFile::editor() const
{
    return ed;
}

QString FormFile::fileName() const
{
    return filename;
}

QString FormFile::absFileName() const
{
#ifndef KOMMANDER
    return pro->makeAbsolute( filename );
#else
    return filename;
#endif
}

QString FormFile::codeFile() const
{
    return filename + codeExtension();
}

QString FormFile::code()
{
#ifndef KOMMANDER
    bool createSource = TRUE;
    QString txt;
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( iface && iface->supports( LanguageInterface::StoreFormCodeSeperate ) ) {
	createSource = FALSE;
	txt = cod;
    }
    if ( iface && createSource ) {
	QValueList<MetaDataBase::Slot> slotList = MetaDataBase::slotList( formWindow() );
	QMap<QString, QString> bodies = MetaDataBase::functionBodies( formWindow() );
	for ( QValueList<MetaDataBase::Slot>::Iterator it = slotList.begin(); it != slotList.end(); ++it ) {
	    if ( (*it).language != pro->language() )
		continue;
	    QString sl( (*it).slot );
	    QString comments = MetaDataBase::functionComments( formWindow(), sl );
	    if ( !comments.isEmpty() )
		txt += comments + "\n";
	    txt += iface->createFunctionStart( formWindow()->name(), make_func_pretty( sl ),
					       ( (*it).returnType.isEmpty() ?
						 QString( "void" ) :
						 (*it).returnType ) );
	    QMap<QString, QString>::Iterator bit = bodies.find( MetaDataBase::normalizeSlot( (*it).slot ) );
	    if ( bit != bodies.end() )
		txt += "\n" + *bit + "\n\n";
	    else
		txt += "\n" + iface->createEmptyFunction() + "\n\n";
	}
    }
    return txt;
#else
    return QString::null;
#endif
}

bool FormFile::save( bool withMsgBox )
{
    if ( !formWindow() )
	return TRUE;
    if ( fileNameTemp )
	return saveAs();
    if ( !isModified() )
	return TRUE;
    if ( ed )
	ed->save();
    else if ( !cm )
	loadCode();

    if ( isModified( WFormWindow ) ) {
	if ( withMsgBox ) {
	    if ( !formWindow()->checkCustomWidgets() )
		return FALSE;
	}

#ifndef KOMMANDER
	if ( QFile::exists( pro->makeAbsolute( filename ) ) ) {
	    QString fn( pro->makeAbsolute( filename ) );
#else
	if(QFile::exists(filename))
	{
	    QString fn(filename);
#endif
#if defined(Q_OS_WIN32)
	    fn += ".bak";
#else
	    fn += "~";
#endif
#ifndef KOMMANDER
	    QFile f( pro->makeAbsolute( filename ) );
#else
	    QFile f(filename);
#endif
	    if ( f.open( IO_ReadOnly ) ) {
		QFile f2( fn );
		if ( f2.open( IO_WriteOnly | IO_Translate ) ) {
		    QCString data( f.size() );
		    f.readBlock( data.data(), f.size() );
		    f2.writeBlock( data );
		} else {
		    QMessageBox::warning( MainWindow::self, "Save", "The file " + codeFile() + " could not be saved" );
		}
	    }
	}
    }

#ifndef KOMMANDER
    cm = FALSE;
    if ( isModified( WFormCode ) && seperateSource ) {
	if ( QFile::exists( pro->makeAbsolute( codeFile() ) ) ) {
	    QString fn( pro->makeAbsolute( codeFile() ) );
#if defined(Q_OS_WIN32)
	    fn += ".bak";
#else
	    fn += "~";
#endif
	    QFile f( pro->makeAbsolute( codeFile() ) );
	    if ( f.open( IO_ReadOnly ) ) {
		QFile f2( fn );
		if ( f2.open( IO_WriteOnly | IO_Translate) ) {
		    QCString data( f.size() );
		    f.readBlock( data.data(), f.size() );
		    f2.writeBlock( data );
		} else {
		    QMessageBox::warning( MainWindow::self, "Save", "The file " + codeFile() + " could not be saved" );
		}
	    }
	}
    }
#endif

    Resource resource( MainWindow::self );
    resource.setWidget( formWindow() );
#ifndef KOMMANDER
    bool formCodeOnly = isModified( WFormCode ) && !isModified( WFormWindow ) && seperateSource;
    if ( !resource.save( pro->makeAbsolute( filename ), formCodeOnly ) ) {
	MainWindow::self->statusBar()->message( tr( "Failed to save file '%1'.").arg( filename ), 5000 );
	return saveAs();
    }
    MainWindow::self->statusBar()->message( tr( "'%1' saved.").arg( formCodeOnly ? codeFile() : filename ), 3000 );
#else
    if(!resource.save(filename, false))
    {
        MainWindow::self->statusBar()->message(tr("Failed to save file '%1'.").arg(filename), 5000);
	return saveAs();
    }
#endif
    MainWindow::self->statusBar()->message( tr( "'%1' saved.").arg(filename), 3000);
    timeStamp.update();
    setModified( FALSE );
    return TRUE;
}

bool FormFile::saveAs()
{
#ifndef KOMMANDER
    QString f = pro->makeAbsolute( fileName() );
    if ( fileNameTemp )
	f = pro->makeAbsolute( QString( formWindow()->name() ).lower() + ".ui" );
#else
    QString f = fileName();
    if(fileNameTemp)
#ifndef KOMMANDER
        f = QString(formWindow()->name()).lower() + ".ui";
#else
        f = QString(formWindow()->name()).lower() + ".kmdr";
#endif
#endif
    bool saved = FALSE;
    while ( !saved ) {
	QString fn = KFileDialog::getSaveFileName( QString::null,
#ifndef KOMMANDER
			tr( "*.ui|Qt User-Interface Files" ), MainWindow::self,
#else
			tr( "*.kmdr|Qt User-Interface Files" ), MainWindow::self,
#endif
					       tr( "Save Form '%1' As ...").arg( formWindow()->name() ));
					       
	if ( fn.isEmpty() )
	    return FALSE;
	QFileInfo fi( fn );
	if ( fi.extension() != "ui" )
#ifndef KOMMANDER
	    fn += ".ui";
#else
	fn += ".kmdr";
#endif
	fileNameTemp = FALSE;
#ifndef KOMMANDER
	filename = pro->makeRelative( fn );
#else
	filename = fn;
#endif

	QFileInfo relfi( filename );
	if ( relfi.exists() ) {
	    if ( QMessageBox::warning( MainWindow::self, tr( "File Already Exists" ),
		tr( "The file already exists. Do you wish to overwrite it?" ),
		QMessageBox::Yes,
		QMessageBox::No ) == QMessageBox::Yes ) {
		saved = TRUE;
	    } else {
		filename = f;
	    }

	} else {
	    saved = TRUE;
	}
    }
#ifndef KOMMANDER
    if ( !checkFileName( TRUE ) ) {
	filename = f;
	return FALSE;
    }
#endif
#ifndef KOMMANDER
    pro->setModified( TRUE );
    timeStamp.setFileName( pro->makeAbsolute( codeFile() ) );
#else
    timeStamp.setFileName(codeFile());
#endif
    if ( ed )
	ed->setCaption( tr( "Edit %1" ).arg( formWindow()->name() ) );
    setModified( TRUE );
    return save();
}

bool FormFile::close()
{
    if ( editor() ) {
	editor()->save();
	editor()->close();
    }
    if ( formWindow() )
	return formWindow()->close();
    return TRUE;
}

bool FormFile::closeEvent()
{
    if ( !isModified() && fileNameTemp ) {
#ifndef KOMMANDER
	pro->removeFormFile( this );
#endif
	emit removedFormFile(this);
	return TRUE;
    }

    if ( !isModified() )
	{
			emit removedFormFile(this);
			return TRUE;
	}

    if ( editor() )
	editor()->save();

    switch ( QMessageBox::warning( MainWindow::self, tr( "Save Form" ),
				   tr( "Save changes to form '%1'?" ).arg( filename ),
				   tr( "&Yes" ), tr( "&No" ), tr( "&Cancel" ), 0, 2 ) ) {
    case 0: // save
	if ( !save() )
	    return FALSE;
    case 1: // don't save
	loadCode();
	if ( ed )
	    ed->editorInterface()->setText( cod );
#ifndef KOMMANDER
	if ( fileNameTemp )
	    pro->removeFormFile( this );
#endif
	MainWindow::self->workspace()->update();
	break;
    case 2: // cancel
	return FALSE;
    default:
	break;
    }

	emit removedFormFile(this);
    setModified( FALSE );
    MainWindow::self->updateFunctionList(); // TODO
    setCodeEdited( FALSE );
    return TRUE;
}

void FormFile::setModified( bool m, int who )
{
    if ( ( who & WFormWindow ) == WFormWindow )
	setFormWindowModified( m );
    if ( ( who & WFormCode ) == WFormCode )
	setCodeModified( m );
}

bool FormFile::isModified( int who )
{
    if ( who == WFormWindow )
	return isFormWindowModified();
    if ( who == WFormCode )
	return isCodeModified();
    return isCodeModified() || isFormWindowModified();
}

bool FormFile::isFormWindowModified() const
{
    if ( !formWindow()  || !formWindow()->commandHistory() )
	return FALSE;
    return formWindow()->commandHistory()->isModified();
}

bool FormFile::isCodeModified() const
{
    if ( !editor() )
	return cm;
    return editor()->isModified();
}

void FormFile::setFormWindowModified( bool m )
{
    bool b = isFormWindowModified();
    if ( m == b )
	return;
    if ( !formWindow() || !formWindow()->commandHistory() )
	return;
    formWindow()->commandHistory()->setModified( m );
    emit somethingChanged( this );
}

void FormFile::setCodeModified( bool m )
{
    bool b = isCodeModified();
    if ( m == b )
	return;
    emit somethingChanged( this );
    cm = TRUE;
    if ( !editor() )
	return;
    editor()->setModified( m );
}

void FormFile::showFormWindow()
{
    if ( formWindow() ) {
	formWindow()->setFocus();
	return;
    }
#ifndef KOMMANDER
    MainWindow::self->openFormWindow( pro->makeAbsolute( filename ), TRUE, this );
#else
    MainWindow::self->openFormWindow(filename, TRUE, this);
#endif
}

SourceEditor *FormFile::showEditor()
{
    showFormWindow();
    bool modify = FALSE;
    if ( !hasFormCode() ) {
	createFormCode();
	modify = TRUE;
    }
    SourceEditor *e = MainWindow::self->openSourceEdior();
    if ( modify )
	setModified( TRUE );
    return e;
}

static int ui_counter = 0;
QString FormFile::createUnnamedFileName()
{
#ifndef KOMMANDER
    return QString( "unnamed" ) + QString::number( ++ui_counter ) + QString( ".ui" );
#else
    return QString( "unnamed" ) + QString::number( ++ui_counter ) + QString( ".kmdr" );
#endif
}

QString FormFile::codeExtension() const
{
#ifndef KOMMANDER
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( iface )
	return iface->formCodeExtension();
#endif
    return "";
}

static const char * const comment =
"/****************************************************************************\n"
"** ui.h extension file, included from the uic-generated form implementation.\n"
"**\n"
"** If you wish to add, delete or rename slots use Qt Designer which will\n"
"** update this file, preserving your code. Create an init() slot in place of\n"
"** a constructor, and a destroy() slot in place of a destructor.\n"
"*****************************************************************************/\n";


bool FormFile::hasFormCode() const
{
    if ( seperateSource )
	return !cod.isEmpty() && cod != QString( comment );
    return TRUE;
}

void FormFile::createFormCode()
{
#ifndef KOMMANDER
    if ( !formWindow() )
	return;
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( !iface )
	return;
    if ( seperateSource )
	cod = comment;
    else
	cod = "";
    QValueList<MetaDataBase::Slot> slotList = MetaDataBase::slotList( formWindow() );
    for ( QValueList<MetaDataBase::Slot>::Iterator it = slotList.begin(); it != slotList.end(); ++it ) {
	cod += "\n\n" + iface->createFunctionStart( formWindow()->name(), make_func_pretty((*it).slot),
						    (*it).returnType.isEmpty() ?
						    QString( "void" ) :
						    (*it).returnType ) +
	       "\n" + iface->createEmptyFunction();
    }
    parseCode( cod, FALSE );
#endif
}

bool FormFile::loadCode()
{
#ifndef KOMMANDER
    QFile f( pro->makeAbsolute( codeFile() ) );
    if ( !f.open( IO_ReadOnly ) ) {
	cod = "";
	return FALSE;
    }
    QTextStream ts( &f );
    cod = ts.read();
    parseCode( cod, FALSE );
    timeStamp.update();
    return TRUE;
#else
    return FALSE;
#endif
}

bool FormFile::isCodeEdited() const
{
    return codeEdited;
}

void FormFile::setCodeEdited( bool b )
{
    codeEdited = b;
}

void FormFile::parseCode( const QString &txt, bool allowModify )
{
#ifndef KOMMANDER
    if ( !formWindow() )
	return;
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( !iface )
	return;
    QValueList<LanguageInterface::Function> functions;
    QValueList<MetaDataBase::Slot> newSlots, oldSlots;
    oldSlots = MetaDataBase::slotList( formWindow() );
    iface->functions( txt, &functions );
    QMap<QString, QString> funcs;
    for ( QValueList<LanguageInterface::Function>::Iterator it = functions.begin();
	  it != functions.end(); ++it ) {
	bool found = FALSE;
	for ( QValueList<MetaDataBase::Slot>::Iterator sit = oldSlots.begin();
	      sit != oldSlots.end(); ++sit ) {
	    QString s( (*sit).slot );
	    if ( MetaDataBase::normalizeSlot( s ) ==
		 MetaDataBase::normalizeSlot( (*it).name ) ) {
		found = TRUE;
		MetaDataBase::Slot slot;
		slot.slot = make_func_pretty( (*it).name );
		slot.specifier = (*sit).specifier;
		slot.access = (*sit).access;
		slot.language = (*sit).language;
		slot.returnType = (*it).returnType;
		newSlots << slot;
		funcs.insert( (*it).name, (*it).body );
		oldSlots.remove( sit );
		break;
	    }
	}
	if ( !found ) {
	    MetaDataBase::Slot slot;
	    slot.slot = make_func_pretty( (*it).name );
	    slot.specifier = "virtual";
	    slot.access = "public";
	    slot.language = pro->language();
	    slot.returnType = (*it).returnType;
	    newSlots << slot;
	    funcs.insert( (*it).name, (*it).body );
	    if ( allowModify )
		setFormWindowModified( TRUE );
	}
	MetaDataBase::setFunctionComments( formWindow(), (*it).name, (*it).comments );
    }

    if ( allowModify && oldSlots.count() > 0 )
	setFormWindowModified( TRUE );
	
    MetaDataBase::setSlotList( formWindow(), newSlots );
    MetaDataBase::setFunctionBodies( formWindow(), funcs, pro->language(), QString::null );
#endif
}

void FormFile::syncCode()
{
#ifndef KOMMANDER
    if ( !editor() )
	return;
    parseCode( editor()->editorInterface()->text(), TRUE );
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( iface && iface->supports( LanguageInterface::StoreFormCodeSeperate ) )
	cod = editor()->editorInterface()->text();
#endif
}

void FormFile::checkTimeStamp()
{
    if ( timeStamp.isUpToDate() || !seperateSource )
	return;
    timeStamp.update();
    if ( codeEdited ) {
	if ( QMessageBox::information( MainWindow::self, tr( "Qt Designer" ),
				       tr( "File '%1' has been changed outside Qt Designer.\n"
					   "Do you want to reload it?" ).arg( timeStamp.fileName() ),
				       tr( "&Yes" ), tr( "&No" ) ) == 0 ) {
	    QFile f( timeStamp.fileName() );
	    if ( f.open( IO_ReadOnly ) ) {
		QTextStream ts( &f );
		editor()->editorInterface()->setText( ts.read() );
		editor()->save();
		MainWindow::self->slotsChanged();
	    }
	}
    } else {
	loadCode();
    }
}

void FormFile::addSlotCode( MetaDataBase::Slot slot )
{
#ifndef KOMMANDER
    if ( !hasFormCode() && !codeEdited )
	return;
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( !iface )
	return;
    QMap<QString, QString> functionBodies = MetaDataBase::functionBodies( formWindow() );
    QMap<QString, QString>::Iterator it = functionBodies.find( MetaDataBase::normalizeSlot( slot.slot ) );
    if ( it == functionBodies.end() ) {
	if ( !codeEdited && !timeStamp.isUpToDate() )
	    loadCode();
	MetaDataBase::MetaInfo mi = MetaDataBase::metaInfo( formWindow() );
	QString cn;
	if ( mi.classNameChanged )
	    cn = mi.className;
	if ( cn.isEmpty() )
	    cn = formWindow()->name();
	QString body = "\n\n" + iface->createFunctionStart( cn,
							    make_func_pretty( slot.slot ),
							    slot.returnType.isEmpty() ?
							    QString( "void" ) :
							    slot.returnType ) +
		       "\n" + iface->createEmptyFunction();
	cod += body;
	MetaDataBase::addFunctionBody( formWindow(),
				       MetaDataBase::normalizeSlot( slot.slot ), iface->createEmptyFunction() );
	if ( codeEdited ) {
	    setModified( TRUE );
	    emit somethingChanged( this );
	}
    }
#endif
}

void FormFile::functionNameChanged( const QString &oldName, const QString &newName )
{
    if ( !cod.isEmpty() ) {
	QString funcStart = QString( formWindow()->name() ) + QString( "::" );
	int i = cod.find( funcStart + oldName );
	if ( i != -1 ) {
	    cod.remove( i + funcStart.length(), oldName.length() );
	    cod.insert( i + funcStart.length(), newName );
	}
    }
}

QString FormFile::formName() const
{
    FormFile* that = (FormFile*) this;
    if ( formWindow() ) {
	that->cachedFormName = formWindow()->name();
	return cachedFormName;
    }
    if ( !cachedFormName.isNull() )
	return cachedFormName;
#ifndef KOMMANDER
    QFile f( pro->makeAbsolute( filename ) );
#else
    QFile f(filename);
#endif
    if ( f.open( IO_ReadOnly ) ) {
	QTextStream ts( &f );
	QString line;
	QString className;
	while ( !ts.eof() ) {
	    line = ts.readLine();
	    if ( !className.isEmpty() ) {
		int end = line.find( "</class>" );
		if ( end == -1 ) {
		    className += line;
		} else {
		    className += line.left( end );
		    break;
		}
		continue;
	    }
	    int start;
	    if ( ( start = line.find( "<class>" ) ) != -1 ) {
		int end = line.find( "</class>" );
		if ( end == -1 ) {
		    className = line.mid( start + 7 );
		} else {
		    className = line.mid( start + 7, end - ( start + 7 ) );
		    break;
		}
	    }
	}
	that->cachedFormName =  className;
    }
    if ( cachedFormName.isEmpty() )
	that->cachedFormName = filename;
    return cachedFormName;
}

void FormFile::formWindowChangedSomehow()
{
    emit somethingChanged( this );
}

#ifndef KOMMANDER
bool FormFile::checkFileName( bool allowBreak )
{
    FormFile *ff = pro->findFormFile( filename, this );
    if ( ff )
	QMessageBox::warning( MainWindow::self, tr( "Invalid Filename" ),
			      tr( "The project already contains a form with a\n"
				  "filename of '%1'. Please choose a new filename." ).arg( filename ) );
    while ( ff ) {
	QString fn;
	while ( fn.isEmpty() ) {
	    fn = QFileDialog::getSaveFileName( pro->makeAbsolute( fileName() ),
#ifndef KOMMANDER
			    tr( "Qt User-Interface Files (*nameui)" ) + ";;" +
#else
			    tr( "Kommander Files (*nameui)" ) + ";;" +
#endif
					       tr( "All Files (*)" ), MainWindow::self, 0,
					       tr( "Save Form '%1' As ...").
					       arg( formWindow()->name() ),
					       &MainWindow::self->lastSaveFilter );
	    if ( allowBreak && fn.isEmpty() )
		return FALSE;
	}
	filename = pro->makeRelative( fn );
	ff = pro->findFormFile( filename, this );
     }
    return TRUE;
}
#endif
