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

//FIXME DCOP to DBUS conversion

#include "parserdata.h"
#include "parser.h"
#include "specials.h"
#include "specialinformation.h"
#include "myprocess.h"
#include "kommanderwidget.h"
#include "invokeclass.h"
#include "kommanderfactory.h"

#include <iostream>
#include <stdlib.h> 
#include <unistd.h>

#include <qfile.h>
#include <qtextstream.h>
//Added by qt3to4:
#include <QString>
#include <QList>
#include <QMetaMethod>

#include <kmessagebox.h>
#include <kapplication.h>
#include <kcolordialog.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kinputdialog.h>
#include <klocale.h>
#include <kpassworddialog.h>
#include <kdebug.h>

using namespace Parse;

/******************* String functions ********************************/
static ParseNode f_stringLength(Parser*, const ParameterList& params)
{
  return params[0].toString().length(); 
}

static ParseNode f_stringContains(Parser*, const ParameterList& params)
{
  return ParseNode(params[0].toString().contains(params[1].toString()));
}

static ParseNode f_stringCompare(Parser*, const ParameterList& params)
{
  int result = QString::compare(params[0].toString(),params[1].toString());
  if (result < 0)
  {
    result = -1;
  } else
    if (result > 0)
    {
      result = 1;
    }
    return result;
}

static ParseNode f_stringFind(Parser*, const ParameterList& params)
{
  return params[0].toString().indexOf(params[1].toString(), params.count() == 3 ? params[2].toInt() : 0);
}

static ParseNode f_stringFindRev(Parser*, const ParameterList& params)
{
  return params[0].toString().indexOf(params[1].toString(), 
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
  return params[0].toString().toLower();
}
    
static ParseNode f_stringUpper(Parser*, const ParameterList& params)
{
  return params[0].toString().toUpper();
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

static ParseNode f_stringRound(Parser*, const ParameterList& params)
{
  QString s;
  s.sprintf(QString("%."+params[1].toString()+"f").toLatin1(), params[0].toDouble());
  return s;
}

static ParseNode f_return(Parser* p, const ParameterList& params)
{
  KommanderWidget * w = p->currentWidget();
  if (w)
    w->setGlobal(w->widgetName() + "_RESULT", params[0].toString());
  return params[0];
}


/******************* Debug function ********************************/
static ParseNode f_debug(Parser*, const ParameterList& params)
{
  for (int i=0; i<params.count(); i++)
    std::cerr << params[i].toString().toUtf8().data();
  std::cerr << "\n";
  fflush(stderr);
  return ParseNode();
}

static ParseNode f_echo(Parser*, const ParameterList& params)
{
  for (int i=0; i<params.count(); i++)
    std::cout << params[i].toString().toUtf8().data();
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
  return text.readAll();
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
  for (int i=1; i<params.count(); i++)
    text << params[i].toString();
  text.flush();
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
  for (int i=1; i<params.count(); i++)
    text << params[i].toString();
  text.flush();
  return 1;
}

static ParseNode f_fileExists(Parser*, const ParameterList& params)
{
  QFile file(params[0].toString());
  if (!file.exists())
    return 0;
  else
    return 1;
}

static ParseNode f_executeSlot(Parser* parser, const ParameterList& params)
{
  ParameterList::ConstIterator it = params.begin(); 
  QString slotName = (*it).toString()+"("; 
  ++it;
  QString widgetName = (*it).toString();
  KommanderWidget* widget = parser->currentWidget();
  if (!widget)
    return ParseNode::error("unknown widget");
  widget = widget->widgetByName(widgetName);
  if (!widget)
    return ParseNode::error("unknown widget");
  QObject *object = widget->object();
  if (!object)
    return ParseNode::error("unknown widget");
  QStringList slotNames;
  uint methodCount = object->metaObject()->methodCount();
  for (uint i = 0; i < methodCount; i++)
  {
    QMetaMethod method = object->metaObject()->method(i);
    if (method.methodType() == QMetaMethod::Slot)
    {
      slotNames.append(method.signature());
    }
  }
  int slotNum = -1;
  int i = 0;
  while (i < slotNames.count())
  {
    if (slotNames[i].startsWith(slotName))
    {
      slotNum = i;
      break;
    }  
    i++;
  }
  if (slotNum == -1)
    return ParseNode::error("unknown function");
  QStringList args;
  ++it;   // skip widget
  while (it != params.end())
  {
    args += (*it).toString(); 
    ++it;
  }
  InvokeClass* inv = new InvokeClass(0);
  inv->invokeSlot(object, slotNames.at(slotNum), args);
  inv->deleteLater();

  return ParseNode();
}




/******************* DBUS function ********************************/
static ParseNode f_dcopid(Parser*, const ParameterList& )
{
 //FIXME return QString(kapp->dcopClient()->appId());
  return QString();
}

static ParseNode f_pid(Parser*, const ParameterList& )
{
  return QString::number(getpid());
}

static ParseNode f_parentPid(Parser*p, const ParameterList& )
{
  return p->variable("_PARENTPID").toString().isEmpty() ? QString::number(getppid()) : p->variable("_PARENTPID");
}

static ParseNode f_internalDcop(Parser* parser, const ParameterList& params)
{
  SpecialFunction function = SpecialInformation::functionObject("DBUS", params[0].toString());
  int functionId = SpecialInformation::function(Group::DBUS, params[0].toString());
  if (functionId == -1)
    return f_executeSlot(parser, params);
  else if ((int)function.minArg() > params.count() - 1)
    return ParseNode::error("too few parameters");
  else if ((int)function.maxArg() < params.count() - 1)
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
  return widget->handleDBUS(functionId, args);
}


static ParseNode f_dcop(Parser*, const ParameterList& params)
{
//  Q3CString appId = kapp->dcopClient()->appId();
  QString object = "KommanderIf";
  SpecialFunction function = SpecialInformation::functionObject("DBUS", params[0].toString());
  
  if (!function.isValidArg(params.count() - 1))
    return ParseNode();
  /*
  QByteArray byteData;
  QDataStream byteDataStream(byteData, QIODevice::WriteOnly);
  for (int i=0 ; i<params.count()-1; i++) 
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
  
  Q3CString replyType, byteReply;
  DCOPClient* cl = KApplication::dcopClient();
  if (!cl || !cl->call(appId, object, function.prototype(SpecialFunction::NoSpaces).toLatin1(), 
       byteData, replyType, byteReply))
  {
    //printError(i18n("Tried to perform DCOP query, but failed."));
    qDebug("Failure");
    return ParseNode();
  }
  QDataStream byteReplyStream(byteReply, QIODevice::ReadOnly);
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
  //printError(i18n("DCOP return type %1 is not yet implemented.", replyType.data()));
  }
  */
  return ParseNode();
}

static ParseNode f_createWidget(Parser* p, const ParameterList& params)
{
  QString widgetName = params[0].toString();
  QString widgetType = params[1].toString();
  QString parentName = params[2].toString();
  KommanderWidget *widget = p->currentWidget()->widgetByName(parentName);
  if (!widget)
    return ParseNode::error("unknown widget");
  QWidget *parent = dynamic_cast<QWidget*>(widget->object());
  QWidget *w = KommanderFactory::createWidget(widgetType, parent, widgetName.toLatin1());
  if (w)
    w->adjustSize();
  return ParseNode();
}

static ParseNode f_widgetExists(Parser* p, const ParameterList& params)
{
  QString widgetName = params[0].toString();
  KommanderWidget *widget = p->currentWidget()->widgetByName(widgetName);
  return (widget ? true : false);
}


static ParseNode f_connect(Parser* p, const ParameterList& params)
{
  QString sender = params[0].toString();
  QString signal = QString::number(QSIGNAL_CODE) + params[1].toString();
  QString receiver = params[2].toString();
  QString slot = QString::number(QSLOT_CODE) + params[3].toString();
  KommanderWidget *senderW = p->currentWidget()->widgetByName(sender);
  if (!senderW)
    return ParseNode::error("unknown widget");
  KommanderWidget *receiverW = p->currentWidget()->widgetByName(receiver);
  if (!receiverW)
    return ParseNode::error("unknown widget");
  dynamic_cast<QObject*>(senderW)->connect(dynamic_cast<QObject*>(senderW), signal.toAscii(), dynamic_cast<QObject*>(receiverW), slot.toAscii());
  return ParseNode();
}

static ParseNode f_disconnect(Parser* p, const ParameterList& params)
{
  QString sender = params[0].toString();
  QString signal = QString::number(QSIGNAL_CODE) + params[1].toString();
  QString receiver = params[2].toString();
  QString slot = QString::number(QSLOT_CODE) + params[3].toString();
  KommanderWidget *senderW = p->currentWidget()->widgetByName(sender);
  if (!senderW)
    return ParseNode::error("unknown widget");
  KommanderWidget *receiverW = p->currentWidget()->widgetByName(receiver);
  if (!receiverW)
    return ParseNode::error("unknown widget");
  dynamic_cast<QObject*>(senderW)->disconnect(dynamic_cast<QObject*>(senderW), signal.toAscii(), dynamic_cast<QObject*>(receiverW), slot.toAscii());
  return ParseNode();
}


static ParseNode f_exec(Parser* P, const ParameterList& params)
{
  MyProcess proc(P->currentWidget());
  QString text;
  kDebug() << "Trying %s" << params[0].toString();
  if (params.count() > 1)
    text = proc.run(params[0].toString(), params[1].toString());
  else
    text = proc.run(params[0].toString());
  return text;
}

static ParseNode f_execBackground(Parser* P, const ParameterList& params)
{
  MyProcess proc(P->currentWidget());
  proc.setBlocking(false);
  QString text;
  kDebug() << "Trying " << params[0].toString();
  if (params.count() > 1)
    text = proc.run(params[0].toString().toLocal8Bit(), params[1].toString());
  else
    text = proc.run(params[0].toString().toLocal8Bit());
  return text;
}

static ParseNode f_dialog(Parser* P, const ParameterList& params)
{
  QString a_dialog = params[0].toString().toLocal8Bit();
  QString a_params = params[1].toString().toLocal8Bit();

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
  return KGlobal::locale()->translateQt("kommander",params[0].toString().toUtf8(),""); 
}

static ParseNode f_env(Parser*, const ParameterList& params)
{
  return QString(getenv(params[0].toString().toLatin1())); 
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
  QList<ParseNode> values = P->array(params[0].toString()).values(); 
  QString array;
  for (QList<ParseNode>::ConstIterator it = values.constBegin(); it != values.constEnd(); ++it ) 
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
//FIXME Q3ValueList
  QStringList keys = P->array(name).keys();
  QList<ParseNode> values = P->array(name).values();
  
  QStringList::Iterator it = keys.begin(); 
  QList<ParseNode>::Iterator itval = values.begin();
  while (it!=keys.end())
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
  QStringList lines = params[1].toString().split("\n");
  for (QStringList::Iterator it = lines.begin(); it != lines.end(); ++it ) 
  {
    QString key = (*it).section('\t', 0, 0).trimmed();
    if (!key.isEmpty())
      P->setArray(name, key, (*it).section('\t', 1));
  }
  return ParseNode();
}
 
static ParseNode f_arrayIndexedFromString(Parser* P, const ParameterList& params)
{
  QString name = params[0].toString();
  QStringList lines;
  if (params.count() == 2)
    lines = params[1].toString().split('\t');
  else
    lines = params[1].toString().split(params[2].toString());
  int i = 0;
  for (QStringList::Iterator it = lines.begin(); it != lines.end(); ++it ) 
  {
    P->setArray(name, QString::number(i), (*it));
    i++;
  }
  return ParseNode();
}

static ParseNode f_arrayIndexedToString(Parser* P, const ParameterList& params)
{
  QString name = params[0].toString();
  if (!P->isArray(name))
    return ParseNode();
  QString separator = "\t";
  if (params.count() == 2)
    separator = params[1].toString();
  QString array;
  int count = P->array(name).keys().count();
  QList<ParseNode> values = P->array(name).values();
  
  for (int i = 0; i < count; i++)
  {
    if (i != 0)
      array.append(separator);
    array.append(P->arrayValue(name, QString::number(i)).toString());
  }
  return array;
}

static ParseNode f_arrayIndexedRemoveElements(Parser* P, const ParameterList& params)
{
  QString name = params[0].toString();
  if (!P->isArray(name))
    return ParseNode();
  int key = params[1].toInt();
  int num = 0;
  if (params.count() == 3)
    num = params[2].toInt() - 1;
  if (num < 0)
    num = 0;
  QString array;
  QStringList keys = P->array(name).keys();
  int count = keys.count();
  if (key + num > count - 1 || key < 0)
    return ParseNode(); //out of index range
  for (int i = 0; i < count; i++)
  {
    if (keys.contains(QString::number(i)) != 1)
      return ParseNode(); //array is not indexed
  }
  for (int i = key; i <= key + num; i++)
  {
    P->unsetArray(name, QString::number(i));  
  }
  int j = key;
  for (int i = key + num + 1; i < count; i++)
  {
    P->setArray(name, QString::number(j), P->arrayValue(name, QString::number(i)));
    j++;
  }
  for (int i = 1; i <= num + 1; i++)
  {
    P->unsetArray(name, QString::number(count - i));
  }  
  return ParseNode();
}


static ParseNode f_arrayIndexedInsertElements(Parser* P, const ParameterList& params)
{
  QString name = params[0].toString();
  if (!P->isArray(name))
    return ParseNode();
  int key = params[1].toInt();
  QStringList keys = P->array(name).keys();
  int count = keys.count();
  if (key > count || key < 0)
    return ParseNode(); //out of index range
  QString separator = "\t";
  if (params.count() == 4)
    separator = params[3].toString();
  QStringList elements = params[2].toString().split(separator);
  int num = elements.count();
  for (int i = count - 1; i >= key; i--)
  {
    P->setArray(name, QString::number(i + num), P->arrayValue(name, QString::number(i)));
  }
  int i = key;
  for (QStringList::Iterator it = elements.begin(); it != elements.end(); ++it ) 
  {
    P->setArray(name, QString::number(i), (*it));
    i++;
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
    
static ParseNode f_inputPassword(Parser* p, const ParameterList& params)
{
  QWidget *parent = 0L;
  KommanderWidget* widget = p->currentWidget();
  if (!widget)
    parent = widget->parentDialog();
  KPasswordDialog dlg(parent);
  dlg.setPrompt(i18n("Enter a password"));
  if (!dlg.exec())
    return ParseNode(); 
  return dlg.password() ;
}
    
static ParseNode f_inputValue(Parser*, const ParameterList& params)
{
  //FIXME
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

static ParseNode f_message_warning(Parser* p, const ParameterList& params)
{
  QWidget *parent = 0L;
  KommanderWidget* widget = p->currentWidget();
  if (!widget)
    parent = widget->parentDialog();
  int result = 0;
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
// FIXME KMessageBox
  if (button1.isNull())
     result = KMessageBox::warningYesNo(parent, text, caption);
  else if (button3.isNull())
    result = KMessageBox::warningYesNo(parent, text, caption, KGuiItem(button1), KGuiItem(button2));
  else 
    result = KMessageBox::warningYesNoCancel(parent, text, caption, KGuiItem(button1), KGuiItem(button2), KGuiItem(button3));
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

static ParseNode f_message_question(Parser* p, const ParameterList& params)
{
  QWidget *parent = 0L;
  KommanderWidget* widget = p->currentWidget();
  if (!widget)
    parent = widget->parentDialog();
  int result = 0;
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
// FIXME
  if (button1.isNull())
      result = KMessageBox::questionYesNo(parent, text, caption);
  else if (button3.isNull())
    result = KMessageBox::questionYesNo(parent, text, caption, KGuiItem(button1), KGuiItem(button2));
  else 
    result = KMessageBox::questionYesNoCancel(parent, text, caption, KGuiItem(button1), KGuiItem(button2), KGuiItem(button3));
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
    KConfig cfg("kommanderrc");
    return cfg.group(fname).readEntry(params[0].toString(), def);
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
    KConfig cfg("kommanderrc");
    cfg.group(fname).writeEntry(params[0].toString(), params[1].toString());
  }
  return ParseNode();
}



  
void ParserData::registerStandardFunctions()
{
  registerFunction("str_length", Function(&f_stringLength, ValueInt, ValueString));
  registerFunction("str_contains", Function(&f_stringContains, ValueInt, ValueString, ValueString));
  registerFunction("str_compare", Function(&f_stringCompare, ValueInt, ValueString, ValueString));
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
  registerFunction("str_round", Function(&f_stringRound, ValueInt, ValueDouble, ValueInt, 2));
  registerFunction("str_todouble", Function(&f_stringToDouble, ValueString, ValueDouble, 1));
  registerFunction("return", Function(&f_return, ValueNone, ValueString, 1, 1));
  registerFunction("debug", Function(&f_debug, ValueNone, ValueString, 1, 100));
  registerFunction("echo", Function(&f_echo, ValueNone, ValueString, 1, 100));
  registerFunction("file_read", Function(&f_fileRead, ValueString, ValueString, 1, 1));
  registerFunction("file_write", Function(&f_fileWrite, ValueInt, ValueString, ValueString, 2, 100));
  registerFunction("file_append", Function(&f_fileAppend, ValueInt, ValueString, ValueString, 2, 100));
  registerFunction("file_exists", Function(&f_fileExists, ValueString, ValueString, 1, 1));
  registerFunction("internalDcop", Function(&f_internalDcop, ValueString, ValueString, ValueString, 2, 100));
  registerFunction("executeSlot", Function(&f_executeSlot, ValueString, ValueString, ValueString, 2, 100));
  registerFunction("createWidget", Function(&f_createWidget, ValueString, ValueString, ValueString, 3, 100));
  registerFunction("widgetExists", Function(&f_widgetExists, ValueString, ValueString, 1));
  registerFunction("connect", Function(&f_connect, ValueString, ValueString, ValueString, ValueString, 4, 4));
  registerFunction("disconnect", Function(&f_disconnect, ValueString, ValueString, ValueString, ValueString, 4, 4));
  registerFunction("dcop", Function(&f_dcop, ValueString, ValueString, ValueString, 3, 100));
  registerFunction("dcopid", Function(&f_dcopid, ValueString, ValueNone, 0, 0));
  registerFunction("pid", Function(&f_pid, ValueString, ValueNone, 0, 0));
  registerFunction("parentPid", Function(&f_parentPid, ValueString, ValueNone, 0, 0));
  registerFunction("dialog", Function(&f_dialog, ValueString, ValueString, ValueString, 1, 2));
  registerFunction("exec", Function(&f_exec, ValueString, ValueString, ValueString, 1, 2));
  registerFunction("execBackground", Function(&f_execBackground, ValueString, ValueString, ValueString, 1, 2));
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
  registerFunction("array_indexedfromstring", Function(&f_arrayIndexedFromString, ValueNone, ValueString, ValueString, ValueString, 2, 3));
  registerFunction("array_indexedtostring", Function(&f_arrayIndexedToString, ValueNone, ValueString, ValueString, 1, 2));
  registerFunction("array_indexedRemoveElements", Function(&f_arrayIndexedRemoveElements, ValueNone, ValueString, ValueInt, ValueInt, 2 , 3));
  registerFunction("array_indexedInsertElements", Function(&f_arrayIndexedInsertElements, ValueNone, ValueString, ValueInt, ValueString, ValueString, 3, 4));
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

