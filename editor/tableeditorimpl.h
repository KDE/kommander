#ifndef TABLEEDITOR_H
#define TABLEEDITOR_H

#include "tableeditor.h"
#include <qmap.h>

class Q3ListBoxItem;
class Q3Table;
class FormWindow;

class TableEditor : public TableEditorBase
{
    Q_OBJECT

public:
    TableEditor( QWidget* parent = 0, QWidget *editWidget = 0, FormWindow *fw = 0,
		 const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~TableEditor();

protected slots:
    void columnDownClicked();
    void columnTextChanged( const QString & );
    void columnUpClicked();
    void currentColumnChanged( Q3ListBoxItem * );
    void currentFieldChanged( const QString & );
    void currentRowChanged( Q3ListBoxItem * );
    void deleteColumnClicked();
    void deleteRowClicked();
    void newColumnClicked();
    void newRowClicked();
    void okClicked();
    void rowDownClicked();
    void rowTextChanged( const QString & );
    void rowUpClicked();
    void applyClicked();
    void chooseRowPixmapClicked();
    void deleteRowPixmapClicked();
    void chooseColPixmapClicked();
    void deleteColPixmapClicked();

private:
    void readFromTable();
    void readColumns();
    void readRows();
    void saveFieldMap();
    void restoreFieldMap();

private:
    Q3Table *editTable;
    FormWindow *formWindow;
    QMap<int, QString> fieldMap;
    QMap<Q3ListBoxItem*, QString> tmpFieldMap;

};

#endif // TABLEEDITOR_H
