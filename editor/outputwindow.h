#ifndef OUTPUTWINDOW_H
#define OUTPUTWINDOW_H

#include <qtabwidget.h>
#include <qstringlist.h>
#include <qvaluelist.h>

struct DesignerOutputDock;
class QTextEdit;
class QListView;

class OutputWindow : public QTabWidget
{
    Q_OBJECT

public:
    OutputWindow( QWidget *parent );
    ~OutputWindow();

    void setErrorMessages( const QStringList &errors, const QValueList<int> &lines, bool clear = TRUE );
    void appendDebug( const QString& );
    void clearErrorMessages();
    void clearDebug();
    void showDebugTab();

    DesignerOutputDock *iFace();

private:
    void setupError();
    void setupDebug();

    QTextEdit *debugView;
    QListView *errorView;

    DesignerOutputDock *iface;

    QtMsgHandler oldMsgHandler;
};

#endif
