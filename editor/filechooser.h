#ifndef FILECHOOSER_H
#define FILECHOOSER_H

#include <qwidget.h>

class QLineEdit;
class QPushButton;

class FileChooser : public QWidget
{
    Q_OBJECT

    Q_ENUMS( Mode )
    Q_PROPERTY( Mode mode READ mode WRITE setMode )
    Q_PROPERTY( QString fileName READ fileName WRITE setFileName )

public:
    FileChooser( QWidget *parent = 0, const char *name = 0);

    enum Mode { File, Directory };

    QString fileName() const;
    Mode mode() const;

public slots:
    void setFileName( const QString &fn );
    void setMode( Mode m );

signals:
    void fileNameChanged( const QString & );

private slots:
    void chooseFile();

private:
    QLineEdit *lineEdit;
    QPushButton *button;
    Mode md;

};

#endif
