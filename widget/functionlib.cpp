/***************************************************************************
                    functionlib.cpp - Standard library of functions
                             -------------------
    copyright          : (C) 2004      Michal Rudolf <mrudolf@kdewebdwev.org>
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "parserdata.h"
#include "parser.h"
#include "specials.h"
#include "specialinformation.h"
#include "myprocess.h"

#include <iostream>

#include <qfile.h>
#include <qtextstream.h>

#include <dcopclient.h>
#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>

using namespace Parse;

/******************* String function ********************************/
static ParseNode f_stringLength(Parser*, const ParameterList& params)
{
  return params[0].toString().length(); 
}

static ParseNode f_stringContains(Parser*, const ParameterList& params)
{
  return params[0].toString().contains(params[1].toString());
}

static ParseNode f_stringFind(Parser*, const ParameterList& params)
{
  return params[0].toString().find(params[1].toString(), params.count() == 3 ? params[2].toInt() : 0);
}

static ParseNode f_stringFindRev(Parser*, const ParameterList& params)
{
  return params[0].toString().find(params[1].toString(), 
    params.count() == 3 ? params[2].toInt() : params[0].toString().length());
}

static ParseNode f_stringLeft(Parser*, const ParameterList& params)
{
  return params[0].toString().left(params[1].toInt());
}

static ParseNode f_stringRight(Parser*, const ParameterList& params)
{
  return params[0].toString().right(params[1].toInt());
}

static ParseNode f_stringMid(Parser*, const ParameterList& params)
{
  return params[0].toString().mid(params[1].toInt(), params.count() == 3 ? params[2].toInt() : 0xffffffff);
}

static ParseNode f_stringRemove(Parser*, const ParameterList& params)
{
  return params[0].toString().remove(params[1].toString());
}

static ParseNode f_stringReplace(Parser*, const ParameterList& params)
{
  return params[0].toString().replace(params[1].toString(), params[2].toString());
}

static ParseNode f_stringLower(Parser*, const ParameterList& params)
{
  return params[0].toString().lower();
}
    
static ParseNode f_stringUpper(Parser*, const ParameterList& params)
{
  return params[0].toString().upper();
}
  
static ParseNode f_stringIsEmpty(Parser*, const ParameterList& params)
{
  return params[0].toString().isEmpty();
}
    
static ParseNode f_stringSection(Parser*, const ParameterList& params)
{
  return params[0].toString().section(params[1].toString(), params[2].toInt(), 
    params.count() == 4 ? params[3].toInt() : params[2].toInt());
}
  
static ParseNode f_stringArgs(Parser*, const ParameterList& params)
{
  if (params.count() == 2)
    return params[0].toString().arg(params[1].toString());
  else if (params.count() == 3)
    return params[0].toString().arg(params[1].toString()).arg(params[2].toString());
  else 
    return params[0].toString().arg(params[1].toString()).arg(params[2].toString()).arg(params[3].toString());
}

static ParseNode f_stringIsNumber(Parser*, const ParameterList& params)    
{
  bool ok;
  params[0].toString().toDouble(&ok);
  return ok;
}
  
static ParseNode f_stringToInt(Parser*, const ParameterList& params)
{
  return params[0].toString().toInt();
}

static ParseNode f_stringToDouble(Parser*, const ParameterList& params)
{
  return params[0].toString().toDouble();
}

/******************* Debug function ********************************/
static ParseNode f_debug(Parser*, const ParameterList& params)
{
  for (uint i=0; i<params.count(); i++)
    std::cerr << params[i].toString();
  std::cerr << "\n";
  return ParseNode();
}


/******************* File function ********************************/
static ParseNode f_fileRead(Parser*, const ParameterList& params)
{
  QFile file(params[0].toString());
  if (!file.open(IO_ReadOnly))
    return ParseNode("");
  QTextStream text(&file);
  return text.read();
}

static ParseNode f_fileWrite(Parser*, const ParameterList& params)
{
  QFile file(params[0].toString());
  if (!file.open(IO_WriteOnly))
    return 0;
  QTextStream text(&file);
  for (uint i=1; i<params.count(); i++)
    text << params[i].toString();
  return 1;
}

static ParseNode f_fileAppend(Parser*, const ParameterList& params)
{
  QFile file(params[0].toString());
  if (!file.open(IO_WriteOnly | IO_Append))
    return 0;
  QTextStream text(&file);
  for (uint i=1; i<params.count(); i++)
    text << params[i].toString();
  return 1;
}



/******************* DCOP function ********************************/
static ParseNode f_dcop(Parser*, const ParameterList& params)
{
  QCString appId = kapp->dcopClient()->appId();
  QCString object = "KommanderIf";
  SpecialFunction function = SpecialInformation::functionObject("DCOP", params[0].toString());
  
  if (!function.isValidArg(params.count() - 1))
    return ParseNode();
  
  QByteArray byteData;
  QDataStream byteDataStream(byteData, IO_WriteOnly);
  for (uint i=0 ; i<params.count()-1; i++) 
  {
    if (function.argumentType(i) == "int")
      byteDataStream << params[i+1].toInt();
    else if (function.argumentType(i) == "long")
      byteDataStream << params[i+1].toInt();
    else if (function.argumentType(i) == "float")
      byteDataStream << params[i+1].toDouble();
    else if (function.argumentType(i) == "double")
      byteDataStream << params[i+1].toDouble();
    else if (function.argumentType(i) == "bool")
      byteDataStream << (bool)params[i+1].toInt();
    else if (function.argumentType(i) == "QStringList")
      if (params[i].toString().find('\n') != -1)
        byteDataStream << QStringList::split("\n", params[i+1].toString(), true);
      else
        byteDataStream << QStringList::split("\\n", params[i+1].toString(), true);
    else 
      byteDataStream << params[i+1].toString();
  }
  
  QCString replyType, byteReply;
  DCOPClient* cl = KApplication::dcopClient();
  if (!cl || !cl->call(appId, object, function.prototype(SpecialFunction::NoSpaces).latin1(), 
       byteData, replyType, byteReply))
  {
    //printError(i18n("Tried to perform DCOP query, but failed."));
    qDebug("Failure");
    return ParseNode();
  }
  QDataStream byteReplyStream(byteReply, IO_ReadOnly);
  if (replyType == "QString")
  {
    QString text;
    byteReplyStream >> text;
    return text;
  }
  else if(replyType == "int")
  {
    int i;
    byteReplyStream >> i;
    return i;
  }
  else if(replyType == "bool")
  {
    bool b;
    byteReplyStream >> b;
    return b;
  }
  else if (replyType == "QStringList")
  {
    QStringList text;
    byteReplyStream >> text;
    return text.join("\n");
  }
  else if(replyType != "void")
  {
  //printError(i18n("DCOP return type %1 is not yet implemented.").arg(replyType.data()));
  }

  return ParseNode();
}


static ParseNode f_exec(Parser* P, const ParameterList& params)
{
  MyProcess proc(P->currentWidget());
  QString text;
  qDebug("Trying %s", params[0].toString().latin1());
  if (params.count() > 1)
    text = proc.run(params[0].toString().local8Bit(), params[1].toString());
  else
    text = proc.run(params[0].toString().local8Bit());
  return text;
}

ParseNode f_i18n(Parser* P, const ParameterList& params)
{
  return KGlobal::locale()->translate(params[0].toString()); 
}
 
    
  
void ParserData::registerStandardFunctions()
{
  registerFunction("str_length", Function(&f_stringLength, ValueInt, ValueString));
  registerFunction("str_contains", Function(&f_stringContains, ValueInt, ValueString, ValueString));
  registerFunction("str_find", Function(&f_stringFind, ValueInt, ValueString, ValueString, ValueInt, 2));
  registerFunction("str_findrev", Function(&f_stringFindRev, ValueInt, ValueString, ValueString, ValueInt, 2));
  registerFunction("str_left", Function(&f_stringLeft, ValueString, ValueString, ValueInt));
  registerFunction("str_right", Function(&f_stringRight, ValueString, ValueString, ValueInt));
  registerFunction("str_mid", Function(&f_stringMid, ValueString, ValueString, ValueInt, ValueInt, 2));
  registerFunction("str_remove", Function(&f_stringRemove, ValueString, ValueString, ValueString));
  registerFunction("str_replace", Function(&f_stringReplace, ValueString, ValueString, ValueString, ValueString));
  registerFunction("str_lower", Function(&f_stringLower, ValueString, ValueString));
  registerFunction("str_upper", Function(&f_stringUpper, ValueString, ValueString));
  registerFunction("str_section", Function(&f_stringSection, ValueString, ValueString, ValueString, ValueInt, ValueInt, 3));
  registerFunction("str_args", Function(&f_stringArgs, ValueString, ValueString, 2, 4));
  registerFunction("str_isnumber", Function(&f_stringArgs, ValueString, ValueInt, 1));
  registerFunction("str_toint", Function(&f_stringArgs, ValueString, ValueInt, 1));
  registerFunction("str_todouble", Function(&f_stringArgs, ValueString, ValueDouble, 1));
  registerFunction("debug", Function(&f_debug, ValueNone, ValueString, 1, 100));
  registerFunction("file_read", Function(&f_fileRead, ValueString, ValueString, 1, 1));
  registerFunction("file_write", Function(&f_fileWrite, ValueInt, ValueString, ValueString, 2, 100));
  registerFunction("file_append", Function(&f_fileAppend, ValueInt, ValueString, ValueString, 2, 100));
  registerFunction("dcop", Function(&f_dcop, ValueString, ValueString, 1, 100));
  registerFunction("exec", Function(&f_i18n, ValueString, ValueString, ValueString, 1, 2));
  registerFunction("i18n", Function(&f_i18n, ValueString, ValueString));
}

