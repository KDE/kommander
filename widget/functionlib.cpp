/***************************************************************************
                    functionlib.cpp - Standard library of functions
                             -------------------
    copyright          : (C) 2004-2006      Michal Rudolf <mrudolf@kdewebdwev.org>
    
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
#include "kommanderwidget.h"

#include <iostream>
#include <stdlib.h> 

#include <qfile.h>
#include <qtextstream.h>

#include <kmessagebox.h>
#include <dcopclient.h>
#include <kapplication.h>
#include <kcolordialog.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kinputdialog.h>
#include <klocale.h>
#include <kpassdlg.h>

using namespace Parse;

/******************* String functions ********************************/
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
  return params[0].toString().findRev(params[1].toString(), 
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
  fflush(stderr);
  return ParseNode();
}

static ParseNode f_echo(Parser*, const ParameterList& params)
{
  for (uint i=0; i<params.count(); i++)
    std::cout << params[i].toString();
  fflush(stdout);
  return ParseNode();
}



/******************* File function ********************************/
static ParseNode f_fileRead(Parser*, const ParameterList& params)
{
  QFile file(params[0].toString());
  if (!file.exists() || !file.open(IO_ReadOnly))
    return ParseNode("");
  QTextStream text(&file);
  return text.read();
}

static ParseNode f_fileWrite(Parser*, const ParameterList& params)
{
  QString fname = params[0].toString();
  if (fname.isEmpty())
    return 0;
  QFile file(fname);
  if (!file.open(IO_WriteOnly))
    return 0;
  QTextStream text(&file);
  for (uint i=1; i<params.count(); i++)
    text << params[i].toString();
  return 1;
}

static ParseNode f_fileAppend(Parser*, const ParameterList& params)
{
  QString fname = params[0].toString();
  if (fname.isEmpty())
    return 0;
  QFile file(fname);
  if (!file.open(IO_WriteOnly | IO_Append))
    return 0;
  QTextStream text(&file);
  for (uint i=1; i<params.count(); i++)
    text << params[i].toString();
  return 1;
}





/******************* DCOP function ********************************/
static ParseNode f_internalDcop(Parser* parser, const ParameterList& params)
{
  SpecialFunction function = SpecialInformation::functionObject("DCOP", params[0].toString());
  int functionId = SpecialInformation::function(Group::DCOP, params[0].toString());
  if (functionId == -1)
    return ParseNode::error("unknown function");
  else if ((uint)function.minArg() > params.count() - 1)
    return ParseNode::error("too few parameters");
  else if ((uint)function.maxArg() < params.count() - 1)
    return ParseNode::error("too many parameters");
  KommanderWidget* widget = parser->currentWidget();
  if (widget)
    widget = widget->widgetByName(params[1].toString());
  if (!widget)
    return ParseNode::error("unknown widget");
  QStringList args;
  ParameterList::ConstIterator it = params.begin(); 
  ++it;   // skip function
  ++it;   // skip widget
  while (it != params.end())
  {
    args += (*it).toString(); 
    ++it;
  }
  return widget->handleDCOP(functionId, args);
}


static ParseNode f_dcop(Parser*, const ParameterList& params)
{
  QCString appId = params[0].toString().latin1();
  QCString object = params[1].toString().latin1();
  QString function = params[2].toString().section('(', 0, 0);
  QStringList items = QStringList::split(",", params[2].toString().section('(', 1, 1).section(')', 0, 0));
  QByteArray byteData;
  QDataStream byteDataStream(byteData, IO_WriteOnly);

  if (items.count() != params.count() - 3)
  {
     qDebug("Wrong number of parameters");
     return ParseNode();
  }
  int i = 3;
  for (QStringList::Iterator it = items.begin(); it != items.end(); ++it)
  {
    *it = (*it).stripWhiteSpace();
    if (*it == "int")
      byteDataStream << params[i++].toInt();
    else if (*it == "long")
      byteDataStream << params[i++].toInt();
    else if (*it == "float")
      byteDataStream << params[i++].toDouble();
    else if (*it == "double")
      byteDataStream << params[i++].toDouble();
    else if (*it == "bool")
      byteDataStream << (bool)params[i++].toInt();
    else if (*it == "QStringList")
      if (params[i].toString().find('\n') != -1)
        byteDataStream << QStringList::split("\n", params[i++].toString(), true);
      else
        byteDataStream << QStringList::split("\\n", params[i++].toString(), true);
    else 
      byteDataStream << params[i++].toString();
  }
  function.append(QString("(%1)").arg(items.join(",")));
  QCString replyType, byteReply;
  DCOPClient* cl = KApplication::dcopClient();
  if (!cl || !cl->call(appId, object, function.latin1(),
       byteData, replyType, byteReply))
  {
    qDebug("DCOP failure");
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
     qDebug("%s", QString("DCOP return type %1 is not yet implemented.").arg(replyType.data()).latin1());
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

static ParseNode f_dialog(Parser* P, const ParameterList& params)
{
  QString a_dialog = params[0].toString().local8Bit();
  QString a_params = params[1].toString().local8Bit();

  QString pFileName = P->currentWidget()->global("_KDDIR") + QString("/") + a_dialog;
  QFileInfo pDialogFile(pFileName);
  if (!pDialogFile.exists()) 
  {
    pFileName = a_dialog;
    pDialogFile.setFile(pFileName);
    if (!pDialogFile.exists())
      return QString();
  }
  QString cmd = QString("kmdr-executor %1 %2 _PARENTPID=%3 _PARENTDCOPID=kmdr-executor-%4")
    .arg(pFileName).arg(a_params).arg(getpid()).arg(getpid());

  MyProcess proc(P->currentWidget());
  QString text;
  text = proc.run(cmd);

  return text;
}

static ParseNode f_i18n(Parser*, const ParameterList& params)
{
  return KGlobal::locale()->translate(params[0].toString()); 
}

static ParseNode f_env(Parser*, const ParameterList& params)
{
  return QString(getenv(params[0].toString().latin1())); 
}

/******************* Array functions ********************************/
static ParseNode f_arrayClear(Parser* P, const ParameterList& params)
{
  P->unsetArray(params[0].toString());
  return ParseNode();
}

static ParseNode f_arrayCount(Parser* P, const ParameterList& params)
{
  if (P->isArray(params[0].toString()))
    return (uint)(P->array(params[0].toString()).count());
  else
    return (uint)0;
}

static ParseNode f_arrayKeys(Parser* P, const ParameterList& params)
{
  if (!P->isArray(params[0].toString()))
    return ParseNode();
  return QStringList(P->array(params[0].toString()).keys()).join("\n");
}

static ParseNode f_arrayValues(Parser* P, const ParameterList& params)
{
  if (!P->isArray(params[0].toString()))
    return ParseNode();
  QValueList<ParseNode> values = P->array(params[0].toString()).values(); 
  QString array;
  for (QValueList<ParseNode>::ConstIterator it = values.begin(); it != values.end(); ++it ) 
    array += (*it).toString() + '\n';
  return array;
}

static ParseNode f_arrayRemove(Parser* P, const ParameterList& params)
{
  if (!P->isArray(params[0].toString()))
    P->unsetArray(params[0].toString(), params[1].toString());
  return ParseNode();
}

static ParseNode f_arrayToString(Parser* P, const ParameterList& params)
{
  QString name = params[0].toString();
  if (!P->isArray(name))
    return ParseNode();
  QString array;
  QStringList keys = P->array(name).keys();
  QValueList<ParseNode> values = P->array(name).values();
  
  QStringList::Iterator it = keys.begin(); 
  QValueList<ParseNode>::Iterator itval = values.begin();
  while (*it)
  {
    array += QString("%1\t%2\n").arg(*it).arg((*itval).toString());
    ++it;
    ++itval;
  }
  return array;
}

static ParseNode f_arrayFromString(Parser* P, const ParameterList& params)
{
  QString name = params[0].toString();
  QStringList lines = QStringList::split("\n", params[1].toString());
  for (QStringList::Iterator it = lines.begin(); it != lines.end(); ++it ) 
  {
    QString key = (*it).section('\t', 0, 0).stripWhiteSpace();
    if (!key.isEmpty())
      P->setArray(name, key, (*it).section('\t', 1));
  }
  return ParseNode();
}
 


/********** input functions *********************/
static ParseNode f_inputColor(Parser*, const ParameterList& params)
{
  QColor color;
  if (params.count())
    color.setNamedColor(params[0].toString());
  KColorDialog::getColor(color);
  return color.name();
}

static ParseNode f_inputText(Parser*, const ParameterList& params)
{
  QString value;
  if (params.count() > 2)
    value = params[0].toString();
  return KInputDialog::getText(params[0].toString(), params[1].toString(), value);
}

static ParseNode f_inputPassword(Parser*, const ParameterList& params)
{
  QCString value;
  if (params.count() > 1)
    value = params[1].toString().local8Bit();
  KPasswordDialog::getPassword(value, params[0].toString());
  return QString::fromLocal8Bit(value);
}

static ParseNode f_inputValue(Parser*, const ParameterList& params)
{
  return KInputDialog::getInteger(params[0].toString(), params[1].toString(), 
                                  params[2].toInt(), params[3].toInt(), params[4].toInt(),
                                  params.count() > 5 ? params[5].toInt() : 1,
                                  (bool*)0);
}

static ParseNode f_inputValueDouble(Parser*, const ParameterList& params)
{
  return KInputDialog::getDouble(params[0].toString(), params[1].toString(), 
                                  params[2].toDouble(), params[3].toDouble(), params[4].toDouble(),
                                  params.count() > 5 ? params[5].toDouble() : 0.1);
}

static ParseNode f_inputOpenFile(Parser*, const ParameterList& params)
{
  QString startdir, filter, caption;
  if (params.count() > 0)
    startdir = params[0].toString();
  if (params.count() > 1)
    filter = params[1].toString();
  if (params.count() > 2)
    caption = params[2].toString();
  return KFileDialog::getOpenFileName(startdir, filter, 0, caption);
}

static ParseNode f_inputOpenFiles(Parser*, const ParameterList& params)
{
  QString startdir, filter, caption;
  if (params.count() > 0)
    startdir = params[0].toString();
  if (params.count() > 1)
    filter = params[1].toString();
  if (params.count() > 2)
    caption = params[2].toString();
  return KFileDialog::getOpenFileNames(startdir, filter, 0, caption).join("\n");
}

static ParseNode f_inputSaveFile(Parser*, const ParameterList& params)
{
  QString startdir, filter, caption;
  if (params.count() > 0)
    startdir = params[0].toString();
  if (params.count() > 1)
    filter = params[1].toString();
  if (params.count() > 2)
    caption = params[2].toString();
  return KFileDialog::getSaveFileName(startdir, filter, 0, caption);
}

static ParseNode f_inputDirectory(Parser*, const ParameterList& params)
{
  QString startdir, caption;
  if (params.count() > 0)
    startdir = params[0].toString();
  if (params.count() > 1)
    caption = params[1].toString();
  return KFileDialog::getExistingDirectory(startdir, 0, caption);
}

static ParseNode f_message_info(Parser*, const ParameterList& params)
{
  QString text, caption;
  if (params.count() > 0)
    text = params[0].toString();
  if (params.count() > 1)
    caption = params[1].toString();
  KMessageBox::information(0, text, caption);
  return ParseNode();
}

static ParseNode f_message_error(Parser*, const ParameterList& params)
{
  QString text, caption;
  if (params.count() > 0)
    text = params[0].toString();
  if (params.count() > 1)
    caption = params[1].toString();
  KMessageBox::error(0, text, caption);
  return ParseNode();
}

static ParseNode f_message_warning(Parser*, const ParameterList& params)
{
  int result;
  QString text, caption, button1, button2, button3;
  if (params.count() > 0)
    text = params[0].toString();
  if (params.count() > 1)
    caption = params[1].toString();
  if (params.count() > 2)
    button1 = params[2].toString();
  if (params.count() > 3)
    button2 = params[3].toString();
  if (params.count() > 4)
    button3 = params[4].toString();
  if (button1.isNull())
    result = KMessageBox::warningYesNo(0, text, caption);
  else if (button3.isNull())
    result = KMessageBox::warningYesNo(0, text, caption, button1, button2);
  else 
    result = KMessageBox::warningYesNoCancel(0, text, caption, button1, button2, button3);
  switch(result)
  {
    case KMessageBox::Yes:  
      return 1;
    case KMessageBox::No:  
      return 2;
    case KMessageBox::Cancel:  
      return 3;
    default:
      return 0;
  }
}

static ParseNode f_message_question(Parser*, const ParameterList& params)
{
  int result;
  QString text, caption, button1, button2, button3;
  if (params.count() > 0)
    text = params[0].toString();
  if (params.count() > 1)
    caption = params[1].toString();
  if (params.count() > 2)
    button1 = params[2].toString();
  if (params.count() > 3)
    button2 = params[3].toString();
  if (params.count() > 4)
    button3 = params[4].toString();
  if (button1.isNull())
    result = KMessageBox::questionYesNo(0, text, caption);
  else if (button3.isNull())
    result = KMessageBox::questionYesNo(0, text, caption, button1, button2);
  else 
    result = KMessageBox::questionYesNoCancel(0, text, caption, button1, button2, button3);
  switch(result)
  {
    case KMessageBox::Yes:  
      return 1;
    case KMessageBox::No:  
      return 2;
    case KMessageBox::Cancel:  
      return 3;
    default:
      return 0;
  }
}

static ParseNode f_read_setting(Parser* parser, const ParameterList& params)
{
  QString def;
  if (params.count() > 1)
    def = params[1].toString();
  if (parser->currentWidget())
  {
    QString fname = parser->currentWidget()->fileName();
    if (fname.isEmpty())
      return ParseNode();
    KConfig cfg("kommanderrc", true);
    cfg.setGroup(fname);
    return cfg.readEntry(params[0].toString(), def);
  }
  return ParseNode();
}

static ParseNode f_write_setting(Parser* parser, const ParameterList& params)
{
  if (parser->currentWidget())
  {
    QString fname = parser->currentWidget()->fileName();
    if (fname.isEmpty())
      return ParseNode();
    KConfig cfg("kommanderrc", false);
    cfg.setGroup(fname);
    cfg.writeEntry(params[0].toString(), params[1].toString());
  }
  return ParseNode();
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
  registerFunction("str_isnumber", Function(&f_stringIsNumber, ValueInt, ValueString));
  registerFunction("str_isempty", Function(&f_stringIsEmpty, ValueInt, ValueString));
  registerFunction("str_toint", Function(&f_stringToInt, ValueString, ValueInt, 1));
  registerFunction("str_todouble", Function(&f_stringToDouble, ValueString, ValueDouble, 1));
  registerFunction("debug", Function(&f_debug, ValueNone, ValueString, 1, 100));
  registerFunction("echo", Function(&f_echo, ValueNone, ValueString, 1, 100));
  registerFunction("file_read", Function(&f_fileRead, ValueString, ValueString, 1, 1));
  registerFunction("file_write", Function(&f_fileWrite, ValueInt, ValueString, ValueString, 2, 100));
  registerFunction("file_append", Function(&f_fileAppend, ValueInt, ValueString, ValueString, 2, 100));
  registerFunction("internalDcop", Function(&f_internalDcop, ValueString, ValueString, ValueString, 2, 100));
  registerFunction("dcop", Function(&f_dcop, ValueString, ValueString, ValueString, 3, 100));
  registerFunction("dialog", Function(&f_dialog, ValueString, ValueString, ValueString, 1, 2));
  registerFunction("exec", Function(&f_exec, ValueString, ValueString, ValueString, 1, 2));
  registerFunction("i18n", Function(&f_i18n, ValueString, ValueString));
  registerFunction("env", Function(&f_env, ValueString, ValueString));
  registerFunction("readSetting", Function(&f_read_setting, ValueString, ValueString, ValueString, 1));
  registerFunction("writeSetting", Function(&f_write_setting, ValueNone, ValueString, ValueString));
  registerFunction("array_clear", Function(&f_arrayClear, ValueNone, ValueString));
  registerFunction("array_count", Function(&f_arrayCount, ValueInt, ValueString));
  registerFunction("array_keys", Function(&f_arrayKeys, ValueString, ValueString));
  registerFunction("array_values", Function(&f_arrayValues, ValueString, ValueString));
  registerFunction("array_tostring", Function(&f_arrayToString, ValueString, ValueString));
  registerFunction("array_fromstring", Function(&f_arrayFromString, ValueNone, ValueString, ValueString));
  registerFunction("array_remove", Function(&f_arrayRemove, ValueNone, ValueString, ValueString));
  registerFunction("input_color", Function(&f_inputColor, ValueString, ValueString, 0));
  registerFunction("input_text", Function(&f_inputText, ValueString, ValueString, ValueString, ValueString, 2));
  registerFunction("input_password", Function(&f_inputPassword, ValueString, ValueString, ValueString, 1));
  registerFunction("input_value", Function(&f_inputValue, ValueInt, ValueString, ValueString, ValueInt, ValueInt, 
                   ValueInt, ValueInt, 5));
  registerFunction("input_double", Function(&f_inputValueDouble, ValueDouble, ValueString, ValueString, ValueDouble, ValueDouble, 
                   ValueDouble, ValueDouble, 5));
  registerFunction("input_openfile", Function(&f_inputOpenFile, ValueString, ValueString, ValueString, ValueString, 0));
  registerFunction("input_openfiles", Function(&f_inputOpenFiles, ValueString, ValueString, ValueString, ValueString, 0));
  registerFunction("input_savefile", Function(&f_inputSaveFile, ValueString, ValueString, ValueString, ValueString, 0));
  registerFunction("input_directory", Function(&f_inputDirectory, ValueString, ValueString, ValueString, 0));
  registerFunction("message_info", Function(&f_message_info, ValueNone, ValueString, ValueString, 1));
  registerFunction("message_error", Function(&f_message_error, ValueNone, ValueString, ValueString, 1));
  registerFunction("message_warning", Function(&f_message_warning, ValueInt, ValueString, ValueString, 
                   ValueString, ValueString, ValueString, 1));
  registerFunction("message_question", Function(&f_message_question, ValueInt, ValueString, ValueString, 
                   ValueString, ValueString, ValueString, 1));
}

