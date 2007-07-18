/**********************************************************************
 This file is based on Qt Designer, Copyright (C) 2000 Trolltech AS.  All rights reserved.

 This file may be distributed and/or modified under the terms of the
 GNU General Public License version 2 as published by the Free Software
 Foundation and appearing in the file LICENSE.GPL included in the
 packaging of this file.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

 See http://www.trolltech.com/gpl/ for GPL licensing information.

 Modified for Kommander:
  (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>

**********************************************************************/

#ifndef NEWFORMIMPL_H
#define NEWFORMIMPL_H

#include "newform.h"
#include <q3iconview.h>
//Added by qt3to4:
#include <Q3PtrList>

class FormFile;


class NewItem : public Q3IconViewItem
{
public:
  enum Type {Form, CustomForm};
  NewItem( Q3IconView *view, const QString &text ) : Q3IconViewItem( view, text ) {}
  virtual void insert( ) = 0;
};

class FormItem : public NewItem
{
public:
  enum FormType {Dialog, Wizard, MainWindow};
  FormItem( Q3IconView *view, const QString &text);
  void insert();
  int rtti() const              {return (int)Form;}
  void setFormType(FormType ft) {fType = ft;}
  FormType formType() const     {return fType;}
private:
  FormType fType;
};

class CustomFormItem : public NewItem
{
public:
  CustomFormItem( Q3IconView *view, const QString &text );
  void insert();
  int rtti() const                        {return (int)CustomForm; }
  void setTemplateFile(const QString &tf) {templFile = tf;}
  QString templateFileName() const        {return templFile;}
private:
  QString templFile;
};

class NewForm : public NewFormBase
{
    Q_OBJECT
public:
  NewForm( QWidget *parent, const QString &templatePath );
  void accept();
protected slots:
  void itemChanged(Q3IconViewItem *item);
private:
    Q3PtrList<Q3IconViewItem> allItems;
};

#endif
