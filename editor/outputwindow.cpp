#include "outputwindow.h"
#include "designerappiface.h"
#include "metadatabase.h"

#include <qlistview.h>
#include <qtextedit.h>
#include <qapplication.h>
#include <qheader.h>
#include <stdlib.h>
#include <stdio.h>

static QTextEdit *debugoutput = 0;
bool debugToStderr = FALSE;

OutputWindow::OutputWindow( QWidget *parent )
    : QTabWidget( parent, "output_window" ), debugView( 0 ), errorView( 0 )
{
    setupDebug();
    setupError();
    iface = new DesignerOutputDockImpl( this );
}

OutputWindow::~OutputWindow()
{
    debugoutput = debugView = 0;
    errorView = 0;
    if ( !debugToStderr )
	qInstallMsgHandler( oldMsgHandler );
    delete iface;
}

void OutputWindow::setupError()
{
    errorView = new QListView( this, "OutputWindow::errorView" );

    if ( MetaDataBase::languages().count() > 1 )
	addTab( errorView, tr( "Error Messages" ) );
    else
	errorView->hide();
    errorView->addColumn( tr( "Message" ) );
    errorView->addColumn( tr( "Line" ) );
    errorView->header()->setStretchEnabled( TRUE );
    errorView->setAllColumnsShowFocus( TRUE );
}

void debugMessageOutput( QtMsgType type, const char *msg )
{
    QString s( msg );

    if ( type != QtFatalMsg ) {
	if ( debugoutput )
	    debugoutput->append( s + "\n" );
    } else {
	fprintf( stderr, msg );
	abort();
    }

    qApp->flush();
}

void OutputWindow::setupDebug()
{
    debugoutput = debugView = new QTextEdit( this, "OutputWindow::debugView" );
    //debugView->setReadOnly( TRUE );
    addTab( debugView, "Debug Output" );

    if ( !debugToStderr )
	oldMsgHandler = qInstallMsgHandler( debugMessageOutput );
}

void OutputWindow::setErrorMessages( const QStringList &errors, const QValueList<int> &lines, bool clear )
{
    if ( clear )
	errorView->clear();
    QStringList::ConstIterator mit = errors.begin();
    QValueList<int>::ConstIterator lit = lines.begin();
    for ( ; lit != lines.end() && mit != errors.end(); ++lit, ++mit )
	(void)new QListViewItem( errorView, *mit, QString::number( *lit ) );
    setCurrentPage( 1 );
}

DesignerOutputDock *OutputWindow::iFace()
{
    return iface;
}

void OutputWindow::appendDebug( const QString &text )
{
    debugView->append( text + "\n" );
}

void OutputWindow::clearErrorMessages()
{
    errorView->clear();
}

void OutputWindow::clearDebug()
{
    debugView->clear();
}

void OutputWindow::showDebugTab()
{
    showPage( debugView );
}
#include "outputwindow.moc"
