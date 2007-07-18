#include "filechooser.h"
#include <qlineedit.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <kfiledialog.h>
#include <qlayout.h>

FileChooser::FileChooser( QWidget *parent, const char *name )
    : QWidget( parent, name ), md( File )
{
    Q3HBoxLayout *layout = new Q3HBoxLayout( this );
    layout->setMargin( 0 );

    lineEdit = new QLineEdit( this, "filechooser_lineedit" );
    layout->addWidget( lineEdit );

    connect( lineEdit, SIGNAL( textChanged( const QString & ) ),
	     this, SIGNAL( fileNameChanged( const QString & ) ) );

    button = new QPushButton( "...", this, "filechooser_button" );
    button->setFixedWidth( button->fontMetrics().width( " ... " ) );
    layout->addWidget( button );

    connect( button, SIGNAL( clicked() ),
	     this, SLOT( chooseFile() ) );

    setFocusProxy( lineEdit );
}

void FileChooser::setMode( Mode m )
{
    md = m;
}

FileChooser::Mode FileChooser::mode() const
{
    return md;
}

void FileChooser::setFileName( const QString &fn )
{
    lineEdit->setText( fn );
}

QString FileChooser::fileName() const
{
    return lineEdit->text();
}

void FileChooser::chooseFile()
{
    QString fn;
    if ( mode() == File )
	fn = KFileDialog::getOpenFileName( lineEdit->text(), QString::null, this );
    else
	fn = KFileDialog::getExistingDirectory( lineEdit->text(),this );

    if ( !fn.isEmpty() ) {
	lineEdit->setText( fn );
	emit fileNameChanged( fn );
    }
}

#include "filechooser.moc"
