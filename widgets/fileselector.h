/***************************************************************************
                          fileselector.h - File selection widget
                             -------------------
    copyright            : (C) 2002 by Marc Britton
    email                : consume@optusnet.com.au
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _HAVE_FILESELECTOR_H_
#define _HAVE_FILESELECTOR_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qwidget.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>

class QBoxLayout;
class KPushButton;
class KLineEdit;

class QShowEvent;
class FileSelector : public QWidget, public KommanderWidget
{
  Q_OBJECT

  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
  Q_PROPERTY(bool showEditor READ showEditor WRITE setShowEditor)
  Q_PROPERTY(QString buttonCaption READ caption WRITE setCaption)
  Q_PROPERTY(QString selectionFilter READ selectionFilter WRITE setSelectionFilter)
  Q_PROPERTY(QString selectionCaption READ selectionCaption WRITE setSelectionCaption)
  Q_PROPERTY(SelectionType selectionType READ selectionType WRITE setSelectionType)
  Q_ENUMS(SelectionType)
  Q_PROPERTY(bool selectionOpenMultiple READ selectionOpenMultiple WRITE setSelectionOpenMultiple)
  
public:
  FileSelector(QWidget *a_parent, const char *a_name);
  ~FileSelector();

  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);

  enum SelectionType { Open, Save, Directory };
  SelectionType selectionType() const;
  void setSelectionType(SelectionType);
  bool selectionOpenMultiple() const;
  void setSelectionOpenMultiple(bool);
  QString selectionFilter() const;
  void setSelectionFilter(const QString&); 
  QString selectionCaption() const;
  void setSelectionCaption(const QString&);
  QString caption() const;
  void setCaption(const QString&);
  bool showEditor() const;
  void setShowEditor(bool);

  virtual QString handleDCOP(int function, const QStringList& args);
  virtual bool isFunctionSupported(int function);

public slots:
  virtual void setWidgetText(const QString&);
  virtual void makeSelection();
  virtual void populate();

signals:
  void widgetOpened();
  void widgetTextChanged(const QString&);
protected:
  bool m_openMultiple;
  QString m_caption;
  QString m_filter;
  SelectionType m_selectionType;
  KLineEdit *m_lineEdit;
  KPushButton *m_selectButton;
  QBoxLayout *m_boxLayout;
  void showEvent(QShowEvent *e);
private:
};

#endif
