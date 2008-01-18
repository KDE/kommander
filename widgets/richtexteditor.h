/***************************************************************************
                          richtexteditor.h - Widget providing simple rich texxt editing 
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
#ifndef _HAVE_RICHTEXTEDITOR_H_
#define _HAVE_RICHTEXTEDITOR_H_

/* KDE INCLUDES */

/* QT INCLUDES */
#include <qstringlist.h>
#include <qstring.h>
#include <qwidget.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <kommander_export.h>

/* Forward Decls */
class QTextEdit;
class QFrame;
class QToolButton;
class QHButtonGroup;

class QShowEvent;
class KOMMANDER_EXPORT RichTextEditor : public QWidget, public KommanderWidget
{
	Q_OBJECT

	Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
	Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
	Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)
public:
  RichTextEditor(QWidget *, const char *);
  virtual ~RichTextEditor();
  
  bool isKommanderWidget() const;
  void setAssociatedText(const QStringList&);
  QStringList associatedText() const;
  QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  
  virtual QString handleDCOP(int function, const QStringList& args);
  virtual bool isFunctionSupported(int function);
public slots:
  void setTextChanged();
  void setWidgetText(const QString &);
  void textAlign(int);
  void textBold(bool);
  void textUnder(bool);
  void textItalic(bool);
  void fontChanged(const QFont &);
  void alignmentChanged(int);
  virtual void populate();
signals:
  void widgetOpened();
  void widgetTextChanged(const QString&);
  void contextMenuRequested(int xpos, int ypos);
protected:
  void showEvent( QShowEvent *e );
  void contextMenuEvent( QContextMenuEvent * e );
private:
  QFrame *m_toolbar;
  QTextEdit *m_textedit;

  QToolButton *m_buttonTextBold, *m_buttonTextUnder, *m_buttonTextItalic, 
    *m_buttonTextLeft,  *m_buttonTextCenter, *m_buttonTextRight;

  QHButtonGroup *m_formatGroup, *m_alignGroup;

};

#endif
