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

using namespace Parse;

/******************* String function ********************************/
ParseNode stringLength(Parser*, const ParameterList& params)
{
  return params[0].toString().length(); 
}

ParseNode stringContains(Parser*, const ParameterList& params)
{
  return params[0].toString().contains(params[1].toString());
}

ParseNode stringFind(Parser*, const ParameterList& params)
{
  return params[0].toString().find(params[1].toString(), params.count() == 3 ? params[2].toInt() : 0);
}

ParseNode stringFindRev(Parser*, const ParameterList& params)
{
  return params[0].toString().find(params[1].toString(), 
    params.count() == 3 ? params[2].toInt() : params[0].toString().length());
}

ParseNode stringLeft(Parser*, const ParameterList& params)
{
  return params[0].toString().left(params[1].toInt());
}

ParseNode stringRight(Parser*, const ParameterList& params)
{
  return params[0].toString().right(params[1].toInt());
}

ParseNode stringMid(Parser*, const ParameterList& params)
{
  return params[0].toString().mid(params[1].toInt(), params.count() == 3 ? params[2].toInt() : 0xffffffff);
}

ParseNode stringRemove(Parser*, const ParameterList& params)
{
  return params[0].toString().remove(params[1].toString());
}

ParseNode stringReplace(Parser*, const ParameterList& params)
{
  return params[0].toString().replace(params[1].toString(), params[2].toString());
}

ParseNode stringLower(Parser*, const ParameterList& params)
{
  return params[0].toString().lower();
}
    
ParseNode stringUpper(Parser*, const ParameterList& params)
{
  return params[0].toString().upper();
}
  
ParseNode stringIsEmpty(Parser*, const ParameterList& params)
{
  return params[0].toString().isEmpty();
}
    
ParseNode stringSection(Parser*, const ParameterList& params)
{
  return params[0].toString().section(params[1].toString(), params[2].toInt(), 
    params.count() == 4 ? params[3].toInt() : params[2].toInt());
}
  
ParseNode stringArgs(Parser*, const ParameterList& params)
{
  if (params.count() == 2)
    return params[0].toString().arg(params[1].toString());
  else if (params.count() == 3)
    return params[0].toString().arg(params[1].toString()).arg(params[2].toString());
  else 
    return params[0].toString().arg(params[1].toString()).arg(params[2].toString()).arg(params[3].toString());
}

ParseNode stringIsNumber(Parser*, const ParameterList& params)    
{
  bool ok;
  params[0].toString().toDouble(&ok);
  return ok;
}
  
ParseNode stringToInt(Parser*, const ParameterList& params)
{
  return params[0].toString().toInt();
}

ParseNode stringToDouble(Parser*, const ParameterList& params)
{
  return params[0].toString().toDouble();
}

/******************* Debug function ********************************/
ParseNode debug(Parser*, const ParameterList& params)
{
  for (uint i=0; i<params.count(); i++)
    std::cerr << params[i].toString();
  std::cerr << "\n";
  return ParseNode();
}


/******************* File function ********************************/
ParseNode fileRead(Parser*, const ParameterList& params)
{
  QFile file(params[0].toString());
  if (!file.open(IO_ReadOnly))
    return ParseNode("");
  QTextStream text(&file);
  return text.read();
}

ParseNode fileWrite(Parser*, const ParameterList& params)
{
  QFile file(params[0].toString());
  if (!file.open(IO_WriteOnly))
    return 0;
  QTextStream text(&file);
  for (uint i=1; i<params.count(); i++)
    text << params[i].toString();
  return 1;
}

ParseNode fileAppend(Parser*, const ParameterList& params)
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
ParseNode localDCOPQuery(Parser*, const ParameterList& params)
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


ParseNode execCommand(Parser* P, const ParameterList& params)
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

  
void ParserData::registerStandardFunctions()
{
  registerFunction("str_length", Function(&stringLength, ValueInt, ValueString));
  registerFunction("str_contains", Function(&stringContains, ValueInt, ValueString, ValueString));
  registerFunction("str_find", Function(&stringFind, ValueInt, ValueString, ValueString, ValueInt, 2));
  registerFunction("str_findrev", Function(&stringFindRev, ValueInt, ValueString, ValueString, ValueInt, 2));
  registerFunction("str_left", Function(&stringLeft, ValueString, ValueString, ValueInt));
  registerFunction("str_right", Function(&stringRight, ValueString, ValueString, ValueInt));
  registerFunction("str_mid", Function(&stringMid, ValueString, ValueString, ValueInt, ValueInt, 2));
  registerFunction("str_remove", Function(&stringRemove, ValueString, ValueString, ValueString));
  registerFunction("str_replace", Function(&stringReplace, ValueString, ValueString, ValueString, ValueString));
  registerFunction("str_lower", Function(&stringLower, ValueString, ValueString));
  registerFunction("str_upper", Function(&stringUpper, ValueString, ValueString));
  registerFunction("str_section", Function(&stringSection, ValueString, ValueString, ValueString, ValueInt, ValueInt, 3));
  registerFunction("str_args", Function(&stringArgs, ValueString, ValueString, 2, 4));
  registerFunction("str_isnumber", Function(&stringArgs, ValueString, ValueInt, 1));
  registerFunction("str_toint", Function(&stringArgs, ValueString, ValueInt, 1));
  registerFunction("str_todouble", Function(&stringArgs, ValueString, ValueDouble, 1));
  registerFunction("debug", Function(&debug, ValueNone, ValueString, 1, 100));
  registerFunction("file_read", Function(&fileRead, ValueString, ValueString, 1, 1));
  registerFunction("file_write", Function(&fileWrite, ValueInt, ValueString, ValueString, 2, 100));
  registerFunction("file_append", Function(&fileAppend, ValueInt, ValueString, ValueString, 2, 100));
  registerFunction("dcop", Function(&localDCOPQuery, ValueString, ValueString, 1, 100));
  registerFunction("exec", Function(&execCommand, ValueString, ValueString, ValueString, 1, 2));
}

