#ifndef QCOMPLETIONEDIT_H
#define QCOMPLETIONEDIT_H

#include <qlineedit.h>
#include <qstringlist.h>

class QListBox;
class QVBox;

class QCompletionEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY( bool autoAdd READ autoAdd WRITE setAutoAdd )
    Q_PROPERTY( bool caseSensitive READ isCaseSensitive WRITE setCaseSensitive )

public:
    QCompletionEdit( QWidget *parent = 0, const char *name = 0 );

    bool autoAdd() const;
    QStringList completionList() const;
    bool eventFilter( QObject *o, QEvent *e );
    bool isCaseSensitive() const;

public slots:
    void setCompletionList( const QStringList &l );
    void setAutoAdd( bool add );
    void clear();
    void addCompletionEntry( const QString &entry );
    void removeCompletionEntry( const QString &entry );
    void setCaseSensitive( bool b );

signals:
    void chosen( const QString &text );

private slots:
    void textDidChange( const QString &text );

private:
    void placeListBox();
    void updateListBox();

private:
    bool aAdd;
    QStringList compList;
    QListBox *listbox;
    QVBox *popup;
    bool caseSensitive;

};



#endif
