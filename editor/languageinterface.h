 /**********************************************************************
** Copyright (C) 2000-2001 Trolltech AS.  All rights reserved.
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

#ifndef LANGUAGEINTERFACE_H
#define LANGUAGEINTERFACE_H

//
//  W A R N I N G  --  PRIVATE INTERFACES
//  --------------------------------------
//
// This file and the interfaces declared in the file are not
// public. It exists for internal purpose. This header file and
// interfaces may change from version to version (even binary
// incompatible) without notice, or even be removed.
//
// We mean it.
//
//

#include <private/qcom_p.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qmap.h>

// {f208499a-6f69-4883-9219-6e936e55a330}
#ifndef IID_Language
#define IID_Language QUuid( 0xf208499a, 0x6f69, 0x4883, 0x92, 0x19, 0x6e, 0x93, 0x6e, 0x55, 0xa3, 0x30 )
#endif

struct LanguageInterface : public QUnknownInterface
{
    struct Function
    {
	QString name;
	QString body;
	QString returnType;
	QString comments;
	int start;
	int end;
	bool operator==( const Function &f ) const {
	    return ( name == f.name &&
		     body == f.body &&
		     returnType == f.returnType &&
		     comments == f.comments );
	}
    };

    struct Connection
    {
	QString sender;
	QString signal;
	QString slot;
	bool operator==( const Connection &c ) const {
	    return ( sender == c.sender &&
		     signal == c.signal &&
		     slot == c.slot );
	}
    };

    enum Support
    {
	ReturnType,
	ConnectionsToCustomSlots,
	AdditionalFiles,
	StoreFormCodeSeperate
    };

    virtual void functions( const QString &code, QValueList<Function> *funcs ) const = 0;
    virtual void initEventFunctions( QMap<QString, QString> &eventFuncs ) = 0;
    virtual QString createFunctionStart( const QString &className, const QString &func, const QString &returnType ) = 0;
    virtual QString createArguments( const QStringList &args ) = 0;
    virtual QString createEmptyFunction() = 0;
    virtual QStringList definitions() const = 0;
    virtual QStringList definitionEntries( const QString &definition, QUnknownInterface *designerIface ) const = 0;
    virtual void setDefinitionEntries( const QString &definition, const QStringList &entries, QUnknownInterface *designerIface ) = 0;
    virtual bool supports( Support s ) const = 0;
    virtual QStringList fileFilterList() const = 0;
    virtual QStringList fileExtensionList() const = 0;
    virtual void preferedExtensions( QMap<QString, QString> &extensionMap ) const = 0;
    virtual QString projectKeyForExtension( const QString &extension ) const = 0;
    virtual void sourceProjectKeys( QStringList &keys ) const = 0;
    virtual QString cleanSignature( const QString &sig ) = 0;
    virtual void saveFormCode( const QString &form, const QString &filename,
			       const QValueList<Function> &functions,
			       const QStringList &forwards,
			       const QStringList &includesImpl,
			       const QStringList &includesDecl,
			       const QStringList &vars,
			       const QValueList<Connection> &connections ) = 0;
    virtual void loadFormCode( const QString &form, const QString &filename,
			       QValueList<Function> &functions,
			       QStringList &forwards,
			       QStringList &includesImpl,
			       QStringList &includesDecl,
			       QStringList &vars,
			       QValueList<Connection> &connections ) = 0;
    virtual QString formCodeExtension() const = 0;

    virtual bool canConnect( const QString &signal, const QString &slot ) = 0;

};

#endif
