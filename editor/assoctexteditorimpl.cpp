/***************************************************************************
                          assoctexteditorimpl.cpp  - Associated text editor implementation
                             -------------------
    copyright            : (C) 2003    Marc Britton <consume@optusnet.com.au>
                           (C) 2004    Michal Rudolf <mrudolf@kdewebdev.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* KDE INCLUDES */
#include <klocale.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <ktextedit.h>
#include <kdebug.h>
#include <kpopupmenu.h>
#include <kactioncollection.h>

#include <ktexteditor/view.h>
#include <ktexteditor/editorchooser.h>
#include <ktexteditor/editinterface.h>
#include <ktexteditor/viewcursorinterface.h>
#include <ktexteditor/highlightinginterface.h>
#include <ktexteditor/popupmenuinterface.h>

#include <kparts/partmanager.h>

/* QT INCLUDES */
#include <qstringlist.h>
#include <qmetaobject.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qfile.h>
#include <qobject.h>
#include <qobjectlist.h>
#include <qtimer.h>


/* OTHER INCLUDES */
#include <cstdio>
#include "assoctexteditorimpl.h"
#include "kommanderwidget.h"
#include "formwindow.h"
#include "propertyeditor.h"
#include "command.h"
#include "metadatabase.h"
#include "choosewidgetimpl.h"
#include "functionsimpl.h"

AssocTextEditor::AssocTextEditor(QWidget *a_widget, FormWindow* a_form,
    PropertyEditor* a_property, KParts::PartManager *partManager, QWidget *a_parent, const char *a_name, bool a_modal)
    : AssocTextEditorBase(a_parent, a_name, a_modal)
{
  // text editor
/*  associatedTextEdit->setFont(KGlobalSettings::fixedFont());
  associatedTextEdit->setTabStopWidth(associatedTextEdit->fontMetrics().maxWidth() * 3);
  associatedTextEdit->setTextFormat(Qt::PlainText);*/
  
  // icon for non-empty scripts
  scriptPixmap = KGlobal::iconLoader()->loadIcon("source", KIcon::Small);

  // signals and slots connections
  m_formWindow = a_form;
  m_propertyEditor = a_property;
  m_widget = a_widget;

  // list of widgets that can be edited
  buildWidgetList();
  for (int i=0; i<widgetsComboBox->count(); i++)
    if (widgetFromString(widgetsComboBox->text(i)) == m_widget)
    {
      widgetsComboBox->setCurrentItem(i);
      break;
    }

  doc = KTextEditor::createDocument ("libkatepart", a_parent, "KTextEditor::Document");
  QGridLayout *layout = new QGridLayout(editorFrame, 1, 1);
  view = doc->createView(editorFrame);
  layout->addWidget(view, 1,1);
  partManager->addPart(doc, true);

  KPopupMenu *popup = new KPopupMenu(this);
  KAction *a = view->actionCollection()->action("edit_find");
  if (a)
    popup->insertItem(a->iconSet(), a->text(), this, SLOT(slotFind()), a->shortcut());
  a = view->actionCollection()->action("edit_find_next");
  if (a)
    popup->insertItem(a->iconSet(), a->text(), this, SLOT(slotFindNext()), a->shortcut());
  a = view->actionCollection()->action("edit_find_prev");
  if (a)
    popup->insertItem(a->iconSet(), a->text(), this, SLOT(slotFindPrev()), a->shortcut());
  a = view->actionCollection()->action("edit_replace");
  if (a)
    popup->insertItem(a->iconSet(), a->text(), this, SLOT(slotReplace()), a->shortcut());

  popup->insertSeparator();
  highlightPopup = new KPopupMenu(popup);
  connect(highlightPopup, SIGNAL(activated(int)), SLOT(slotHighlightingChanged(int)));
  
  KTextEditor::HighlightingInterface *hlIf = dynamic_cast<KTextEditor::HighlightingInterface*>(doc);
  uint hlCount = hlIf->hlModeCount();
  for (uint i = 0; i < hlCount; i++)
  {
    if (hlIf->hlModeSectionName(i) == "Scripts")
      highlightPopup->insertItem(hlIf->hlModeName(i), i);
    if (hlIf->hlModeName(i).contains("Kommander", false) > 0)
    {
      hlIf->setHlMode(i);
      highlightPopup->setItemChecked(i, true);
      oldHlMode = i;
    }
  }

  popup->insertItem(i18n("Highlighting"), highlightPopup);
  
  KTextEditor::PopupMenuInterface *popupIf = dynamic_cast<KTextEditor::PopupMenuInterface *>(view);
  popupIf->installPopup(popup);
  
  associatedTextEdit = dynamic_cast<KTextEditor::EditInterface*>(doc);
  setWidget(a_widget);

  connect(doc, SIGNAL(textChanged()), SLOT(textEditChanged()));
  connect(widgetsComboBox, SIGNAL(activated(int)), SLOT(widgetChanged(int)));
  connect(stateComboBox, SIGNAL(activated(int)), SLOT(stateChanged(int)));
  connect(filePushButton, SIGNAL(clicked()), SLOT(insertFile()));
  connect(functionButton, SIGNAL(clicked()), SLOT(insertFunction()));
  connect(widgetComboBox, SIGNAL(activated(int)), SLOT(insertWidgetName(int)));
  connect(treeWidgetButton, SIGNAL(clicked()), SLOT(selectWidget()));
  
  view->setFocus();
}

AssocTextEditor::~AssocTextEditor()
{
  save();
  delete doc;
}

void AssocTextEditor::setWidget(QWidget *a_widget)
{
  KommanderWidget *a_atw = dynamic_cast<KommanderWidget *>(a_widget);
  if (!a_widget || !a_atw)
    return;

  
  m_widget = a_widget;
  m_states = a_atw->states();
  m_populationText = a_atw->populationText();

  // list of states of current widget (plus population text)
  stateComboBox->clear();
  stateComboBox->insertStringList(a_atw->displayStates());
  stateComboBox->insertItem("population");

  // set states and population scripts
  QStringList at = a_atw->associatedText();
  m_atdict.clear();
  QStringList::iterator at_it = at.begin();
  for(QStringList::ConstIterator s_it = m_states.begin(); s_it != m_states.end(); ++s_it)
  {
    if(at_it != at.end())
    {
      m_atdict[(*s_it)] = (*at_it);
      ++at_it;
    }
    else
      m_atdict[(*s_it)] = QString::null;
  }
  m_populationText = a_atw->populationText();

  // show pixmaps for nonempty scripts
  int p_population = stateComboBox->count()-1;
  for (int i=0; i<p_population; i++)
    if (!m_atdict[stateComboBox->text(i)].isEmpty())
       stateComboBox->changeItem(scriptPixmap, stateComboBox->text(i), i);
  if (!m_populationText.isEmpty())
    stateComboBox->changeItem(scriptPixmap, stateComboBox->text(p_population), p_population);

  // initial text for initial state
  stateComboBox->setCurrentItem(0);
  m_currentState = stateComboBox->currentText();

  stateChanged(0);

}

void AssocTextEditor::save() const
{
  KommanderWidget *atw = dynamic_cast<KommanderWidget *>(m_widget);
  if (!atw)
    return;

  if (atw->associatedText() != associatedText())
  {
    QString text = i18n("Set the \'text association\' of \'%1\'").arg(m_widget->name());
    SetPropertyCommand *cmd  = new SetPropertyCommand(text, m_formWindow,
        m_widget, m_propertyEditor, "associations", atw->associatedText(),
        associatedText(), QString::null, QString::null, false);
    cmd->execute();
    m_formWindow->commandHistory()->addCommand(cmd);
    MetaDataBase::setPropertyChanged(m_widget, "associations", true);
  }
  if (atw->populationText() != populationText())
  {
    QString text = i18n("Set the \'population text\' of \'%1\'").arg(m_widget->name());
    SetPropertyCommand *cmd  = new SetPropertyCommand(text, m_formWindow, m_widget,
        m_propertyEditor, "populationText", atw->populationText(),
        populationText(), QString::null, QString::null, false);
    cmd->execute();
    m_formWindow->commandHistory()->addCommand(cmd);
    MetaDataBase::setPropertyChanged(m_widget, "populationText", true);
  }
}


QStringList AssocTextEditor::associatedText() const
{
  QStringList at;
  for(QStringList::ConstIterator it = m_states.begin(); it != m_states.end(); ++it)
    at.append(m_atdict[(*it)]);
  return at;
}

QString AssocTextEditor::populationText() const
{
  return m_populationText;
}

QStringList AssocTextEditor::buildWidgetList()
{
  QStringList widgetList;
  QObject* thisObject = m_formWindow->mainContainer();
  QObjectList *objectList = thisObject->queryList();
  objectList->prepend(thisObject);

  for (QObjectListIt it(*objectList); it.current(); ++it)
  {
    // There is a warning message with the property() function if it does not exist.
    // Verify the property exists with the meta information first */
    bool pExists = false;
    QMetaObject *metaObj = it.current()->metaObject();
    if(metaObj)
    {
      int id = metaObj->findProperty("KommanderWidget", true);
      const QMetaProperty *metaProp = metaObj->property(id, true);
      if(metaProp && metaProp->isValid())
        pExists = true;
    }
    if(pExists)
    {
      QVariant flag = (it.current())->property("KommanderWidget");
      if(flag.isValid() && !(QString(it.current()->name()).startsWith("qt_")))
      {
        widgetList.append( widgetToString( (QWidget*)it.current()) );
        m_widgetList.insert(it.current()->name(), (QWidget*)it.current());
      }
    }
  }
  delete objectList;

  widgetList.sort();
  widgetComboBox->clear();
  widgetComboBox->insertStringList(widgetList);
  widgetsComboBox->clear();
  widgetsComboBox->insertStringList(widgetList);
  return widgetList;
}

void AssocTextEditor::stateChanged(int a_index)
{
  m_currentState = stateComboBox->text(a_index);
  if (a_index == stateComboBox->count() - 1)
    associatedTextEdit->setText(m_populationText);
  else
    associatedTextEdit->setText(m_atdict[m_currentState]);
}

void AssocTextEditor::textEditChanged()
{
  if (m_currentState == "population")
    m_populationText = associatedTextEdit->text();
  m_atdict[m_currentState] = associatedTextEdit->text();
  int index = stateComboBox->currentItem();
  if (associatedTextEdit->text().isEmpty())
    stateComboBox->changeItem(QPixmap(), stateComboBox->currentText(), index);
  else if (!stateComboBox->pixmap(index) || stateComboBox->pixmap(index)->isNull())
    stateComboBox->changeItem(scriptPixmap, stateComboBox->currentText(), index);
}

void AssocTextEditor::widgetChanged(int index)
{
  save();
  setWidget( widgetFromString( widgetsComboBox->text(index)) ) ;
}

void AssocTextEditor::selectWidget()
{
  ChooseWidget cDialog(this);
  cDialog.setWidget(m_formWindow->mainContainer());
  if (cDialog.exec()) {
    QString newWidget = cDialog.selection();
    for (int i = 0; i<widgetsComboBox->count(); i++)
      if (widgetsComboBox->text(i) == newWidget) {
        widgetsComboBox->setCurrentItem(i);
        widgetChanged(i);
        break;
      }
  }
}



void AssocTextEditor::insertAssociatedText(const QString& a_text)
{
  uint line, col;
  KTextEditor::ViewCursorInterface *viewCursorIf = dynamic_cast<KTextEditor::ViewCursorInterface*>(view);
  viewCursorIf->cursorPositionReal(&line, &col);
  associatedTextEdit->insertText(line, col, a_text);
}

void AssocTextEditor::insertFile()
{
  QString fileName = KFileDialog::getOpenFileName();

  if(fileName.isEmpty())
    return;

  QFile insertFile(fileName);
  if(!insertFile.open(IO_ReadOnly))
  {
    KMessageBox::error( this, i18n("<qt>Cannot open file<br><b>%1</b></qt").arg( fileName ) );
    return;
  }
  QTextStream insertStream(&insertFile);
  QString insertText = insertStream.read();
  insertAssociatedText(insertText);
  insertFile.close();
}

void AssocTextEditor::insertWidgetName(int index)
{
  QString prefix;
  if (!KommanderWidget::useInternalParser)
    prefix = QString(QChar(ESCCHAR));
  insertAssociatedText(prefix + widgetToString(widgetFromString(widgetComboBox->text(index)), false));
}

void AssocTextEditor::insertFunction()
{
  FunctionsDialog pDialog(this, m_widgetList, 0);
  if (pDialog.exec())
    insertAssociatedText(pDialog.functionText());
}

QString AssocTextEditor::widgetToString(QWidget* widget, bool formatted)
{
   if (!widget)
     return QString::null;
   else if (formatted)
     return QString("%1 (%2)").arg(widget->name()).arg(widget->className());
   else
     return widget->name();
}

QWidget* AssocTextEditor::widgetFromString(const QString& name)
{
  QString realname = name;
  int i = realname.find(' ');
  if (i != -1)
    realname.truncate(i);
  return m_widgetList[realname];
}

void AssocTextEditor::slotFind()
{
  KAction *a = view->actionCollection()->action("edit_find");
  a->activate();
}


void AssocTextEditor::slotReplace()
{
  KAction *a = view->actionCollection()->action("edit_replace");
  a->activate();
}

void AssocTextEditor::slotFindNext()
{
  KAction *a = view->actionCollection()->action("edit_find_next");
  a->activate();
}
void AssocTextEditor::slotFindPrev()
{
  KAction *a = view->actionCollection()->action("edit_find_prev");
  a->activate();
}

void AssocTextEditor::slotHighlightingChanged(int mode)
{
  highlightPopup->setItemChecked(oldHlMode, false);
  KTextEditor::HighlightingInterface *hlIf = dynamic_cast<KTextEditor::HighlightingInterface*>(doc);
  hlIf->setHlMode(mode);
  highlightPopup->setItemChecked(mode, true);
  oldHlMode = mode;
}

#include "assoctexteditorimpl.moc"
