/***************************************************************************
                         listbox.cpp - Listbox widget 
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
/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qlistbox.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "listbox.h"

ListBox::ListBox(QWidget *a_parent, const char *a_name)
  : QListBox(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);

//FIXME: Do we need it?
//  connect(this, SIGNAL(highlighted(int)), this, SLOT(setActivatedText(int)));

  emit widgetOpened();

}

ListBox::~ListBox()
{
}

QString ListBox::currentState() const
{
  return QString("default");
}

bool ListBox::isKommanderWidget() const
{
  return TRUE;
}

QStringList ListBox::associatedText() const
{
  return KommanderWidget::associatedText();
}

void ListBox::setAssociatedText(QStringList a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void ListBox::setWidgetText(const QString &a_text)
{
  /*
     a_text is a set of strings delimited by \n to insert into the list box
  */
  QStringList strings = QStringList::split("\n", a_text); // note : doesn't allow empty entries

  clear();

  insertStringList(strings);

  emit widgetTextChanged(a_text);
}

QString ListBox::widgetText() const
{
  QStringList strings;

  int I = 0, length = count();
  for(;I < length;++I)
  {
    if(isSelected(I))
      strings += item(I)->text();
  }
  return strings.join("\n");
}

void ListBox::setActivated(int /*a_item*/)
{
  QStringList strings;

  int I = 0, length = count();
  for(;I < length;++I)
  {
    if(isSelected(I))
      strings += item(I)->text();
  }
  emit widgetTextChanged(strings.join("\n"));
}
#include "listbox.moc"
