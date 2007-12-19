/***************************************************************************
                          assoctexteditorimpl.h - Associated text editor implementation
                             -------------------
    copyright            : (C) 2003    Marc Britton <consume@optusnet.com.au>
                           (C) 2004    Michal Rudolf <mrudolf@kdewebdev.org>
                           (C) 2006    Andras Mantia <amantia@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _HAVE_ASSOCTEXTEDITORIMPL_H_
#define _HAVE_ASSOCTEXTEDITORIMPL_H_

#include <qdict.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qwidget.h>
#include <qobject.h>
#include <qmap.h>
#include <qptrvector.h>
#include <qpixmap.h>


#include "assoctexteditor.h"
#include <kommanderwidget.h>

class FormWindow;
class PropertyEditor;

namespace KTextEditor {
  class EditInterface;
  class Document;
  class View;
}

namespace KParts{
  class PartManager;
}

class KPopupMenu;

class AssocTextEditor : public AssocTextEditorBase
{
  Q_OBJECT
public:
  AssocTextEditor(QWidget*, FormWindow*, PropertyEditor*, KParts::PartManager *partManager, QWidget*, const char* = 0, bool = true);
  ~AssocTextEditor();
  // Set interface for given widget.
  void setWidget(QWidget *);
  // Save changes for current widget
  void save() const;
  // Return population text for current widget
  QString populationText() const;
  // Return associated text for current widget
  QStringList associatedText() const;

public slots:
  // Current script text has changed - save it in m_atdict
  void textEditChanged();
  // Current widget has changed - store script and update interface
  void widgetChanged(int);
  // Current script has changed - store script and show new script
  void stateChanged(int);
  // Select widget via widget tree dialog
  void selectWidget();

  // Functions for script editing:

  // Insert given text at cursor position for current widget and state
  void insertAssociatedText(const QString&);
  // Select file and insert its content at cursor position
  void insertFile();
  // Open function browser and insert chosen function at cursor position
  void insertFunction();
  // Insert selected function at cursor position
  void insertWidgetName(int);

protected slots:

  void slotHighlightingChanged(int mode);

private:
  bool useInternalParser();

  // Current widget
  QWidget *m_widget;
  KommanderWidget* m_kommanderWidget;
//FIXME: could it be done more cleanly?
  // Current form and property editor
  FormWindow* m_formWindow;
  PropertyEditor* m_propertyEditor;
  // Current state
  QString m_currentState;
  // List of states for current widget
  QStringList m_states;
  // Current widget list
  QDict<QWidget> m_widgetList;
  // Dictionary for current widget; contains pairs (state, script associated with current state)
  QMap<QString, QString> m_atdict;
  // Current population text
  QString m_populationText;
  // Non-enmpty script pixmap
  QPixmap scriptPixmap;

  // Create list of all widgets on the same dialog as current widget
  // Set store to true, to update m_widgetList;
  QStringList buildWidgetList();
  // Convert widget to string displayed in combos
  // If formatted, return formatted string, else just widget's name
  QString widgetToString(QWidget* widget, bool formatted = true);
  // Conver combo string to widget
  QWidget* widgetFromString(const QString& name);

  KTextEditor::EditInterface *associatedTextEdit;
  KTextEditor::Document *doc;
  KTextEditor::View *view;
  KPopupMenu *highlightPopup;
  int oldHlMode;
};

#endif
