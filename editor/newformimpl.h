/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef NEWFORMIMPL_H
#define NEWFORMIMPL_H

#include "newform.h"
#include <qiconview.h>

#ifndef KOMMANDER
class Project;
#endif
class FormFile;

class NewItem : public QIconViewItem
{
public:
    enum Type {
	ProjectType,
	Form,
	CustomForm,
	SourceFileType,
	SourceTemplateType
    };

    NewItem( QIconView *view, const QString &text )
	: QIconViewItem( view, text ) {}
#ifndef KOMMANDER
    virtual void insert( Project *pro ) = 0;
    virtual void setProject( Project * ) {}
#else
    virtual void insert( ) = 0;
#endif

};

#ifndef KOMMANDER
class ProjectItem : public NewItem
{
public:
    ProjectItem( QIconView *view, const QString &text );
    void insert( Project *pro );
    int rtti() const { return (int)ProjectType; }

    void setLanguage( const QString &l ) { lang = l; }
    QString language() const { return lang; }

private:
    QString lang;

};
#endif

class FormItem : public NewItem
{
public:
    enum FormType {
	Widget,
	Dialog,
	Wizard,
	MainWindow
    };

    FormItem( QIconView *view, const QString &text );
#ifndef KOMMANDER
    void insert( Project *pro );
#else
    void insert();
#endif
    int rtti() const { return (int)Form; }

    void setFormType( FormType ft ) { fType = ft; }
    FormType formType() const { return fType; }

private:
    FormType fType;

};

class CustomFormItem : public NewItem
{
public:
    CustomFormItem( QIconView *view, const QString &text );
#ifndef KOMMANDER
    void insert( Project *pro );
#else
    void insert();
#endif
    int rtti() const { return (int)CustomForm; }

    void setTemplateFile( const QString &tf ) { templFile = tf; }
    QString templateFileName() const { return templFile; }

private:
    QString templFile;

};

#ifndef KOMMANDER
class SourceFileItem : public NewItem
{
public:
    SourceFileItem( QIconView *view, const QString &text );
    void insert( Project *pro );
    int rtti() const { return (int)SourceFileType; }

    void setExtension( const QString &e ) { ext = e; }
    QString extension() const { return ext; }
    void setLanguage( const QString &l ) { lang = l; }
    void setProject( Project *pro );

private:
    QString ext;
    QString lang;
    bool visible;

};

class SourceTemplateItem : public NewItem
{
public:
    SourceTemplateItem( QIconView *view, const QString &text );
    void insert( Project *pro );
    int rtti() const { return (int)SourceTemplateType; }

    void setTemplate( const QString &t ) { templ = t; }
    QString tenplate() const { return templ; }
    void setLanguage( const QString &l ) { lang = l; }
    void setProject( Project *pro );

private:
    QString templ;
    QString lang;
    bool visible;

};
#endif

class NewForm : public NewFormBase
{
    Q_OBJECT

public:
#ifndef KOMMANDER
    NewForm( QWidget *parent, const QStringList& projects,
	     const QString& currentProject, const QString &templatePath );
#else
    NewForm( QWidget *parent, const QString &templatePath );
#endif
    void accept();

protected slots:
#ifndef KOMMANDER
	void projectChanged( const QString &project );
#endif
    void itemChanged( QIconViewItem *item );
signals:
private:
    QPtrList<QIconViewItem> allItems;

};

#endif
