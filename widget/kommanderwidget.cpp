/***************************************************************************
                          kommanderwidget.cpp - Text widget functionality class 
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
#include <dcopclient.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kprocess.h>

/* QT INCLUDES */
#include <qapplication.h>
#include <qcstring.h>
#include <qdatastream.h>
#include <qobject.h>
#include <qobjectlist.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qvariant.h>


/* UNIX INCLUDES */
#include <unistd.h>

/* OTHER INCLUDES */
#include "myprocess.h"
#include "kommanderwidget.h"




KommanderWidget::KommanderWidget(QObject *a_thisObject)
{
  m_thisObject = a_thisObject;
  registerFunctions();
}

KommanderWidget::~KommanderWidget()
{
}

void KommanderWidget::setAssociatedText(const QStringList& a_associations)
{
  m_associatedText = a_associations;
  while(m_associatedText.count() < (states().count()))
  m_associatedText += QString::null; // sync states and associations
}

QStringList KommanderWidget::associatedText() const
{
  return m_associatedText;
}

void KommanderWidget::setPopulationText(const QString& a_txt)
{
  m_populationText = a_txt;
}

QString KommanderWidget::populationText() const
{
  return m_populationText;
}

QStringList KommanderWidget::states() const
{
  return m_states;
}

QStringList KommanderWidget::displayStates() const
{
  return m_displayStates;
}

void KommanderWidget::setStates(const QStringList& a_states)
{
  m_states = a_states;
}

void KommanderWidget::setDisplayStates(const QStringList& a_displayStates)
{
  m_displayStates = a_displayStates;
}

QString KommanderWidget::evalAssociatedText() const // expands and returns associated text as a string
{
  int index = ( states().findIndex( currentState()) );
  if (index == -1)
  {
    printError(i18n("Invalid state for associated text"));
    return QString::null;
  }
  return evalAssociatedText(m_associatedText[index]);
}

QString KommanderWidget::evalAssociatedText(const QString& a_text) const
{
  QString evalText;
  
  int pos = 0, baseTextLength = a_text.length();
  while (pos < baseTextLength)
  {
    int ident = a_text.find(ESCCHAR, pos);
    if (ident == -1) {
      evalText += a_text.mid(pos);
      break;
    }
    evalText += a_text.mid(pos, ident - pos);
    pos = ident+1;
      
    QString identifier = parseIdentifier(a_text, pos);
    if (identifier.isEmpty()) {
      if (pos < baseTextLength && a_text[pos] == '#') {   // comment 
        int newpos = a_text.find('\n', pos+1);
        if (newpos == -1) 
          newpos = a_text.length();
        if (pos > 1 && a_text[pos-2] == '\n')
          newpos++;
        pos = newpos;
      }
      else
         evalText += ESCCHAR;    // single @
      continue;
    }
    
    if (m_functions.contains(identifier)) {    // @function
      bool ok = true;
      QString arg = parseBrackets(a_text, pos, ok);
      if (!ok)
      {
        printError(i18n("Unmatched parenthesis after \'%1\'").arg(identifier));
        return QString::null;
      }
      const QStringList args = parseArgs(arg, ok);
      if (!ok)
      {
        printError(i18n("Unmatched quotes in argument of \'%1\'").arg(identifier));
        return QString::null;
      }
      if (args.count() < m_functions[identifier].first)
      {
        printError(i18n("Not enough arguments for \'%1\' (%2 instead of %3)")
          .arg(identifier).arg(args.count()).arg(m_functions[identifier].first));
        return QString::null;
      }
      if (args.count() > m_functions[identifier].second)
      {
        printError(i18n("Too many arguments for \'%1\' (%2 instead of %3)")
          .arg(identifier).arg(args.count()).arg(m_functions[identifier].second));
        return QString::null;
      }
      if (identifier == "widgetText")
        evalText += widgetText();
      else if (identifier == "selectedWidgetText")
        evalText += selectedWidgetText();
      else if (identifier == "dcopid")
        evalText += kapp->dcopClient()->appId();
      else if (identifier == "pid")
        evalText += QString().setNum(getpid());
      else if (identifier == "exec")
        evalText += execCommand(args[0]);
      else if (identifier == "dcop")
        evalText += dcopQuery(args);
      else if (identifier == "parentPid")
        evalText += parentPid();
      else if (identifier == "readSetting") 
      {
        KConfig cfg("kommanderrc", true);
        cfg.setGroup(QString(parentDialog()->name()));
        evalText += cfg.readEntry(args[0], args[1]);
      }
      else if (identifier == "writeSetting") 
      {
        KConfig cfg("kommanderrc", false);
        cfg.setGroup(QString(parentDialog()->name()));
        cfg.writeEntry(args[0], args[1]);
      }
      else if (identifier == "env")
      {
        evalText += QString(getenv(args[0].latin1())); 
      }
      else if (identifier == "global")
      {
        evalText += localDcopQuery("global(QString)", args); 
      }
      else if (identifier == "dialog")
      {
        evalText += execCommand(QString("kmdr-executor %1 _PARENTPID=%2 _PARENTDCOPID=kmdr-executor-%3")
           .arg(args[0]).arg(getpid()).arg(getpid())); 
      }
      else if (identifier == "setGlobal")
      {
        localDcopQuery("setGlobal(QString,QString)", args); 
      }
      else if (identifier == "execBegin")
      {
        int f = a_text.find("@execEnd", pos);  
        if (f == -1)
        {
          printError(i18n("Unterminated @execBegin ... @execEnd block"));
          return QString::null;
        } 
        else
        {
          QString shell = args.count() ? args[0] : QString::null;
          evalText += execCommand(evalAssociatedText(a_text.mid(pos, f - pos)), shell);
          pos = f + QString("@execEnd").length();
        }
      }
    }
    else                    // see if this is a widget
    {
      KommanderWidget* pWidget = parseWidget(identifier);
      if (!pWidget) {
        evalText += QChar(ESCCHAR);
        evalText += identifier;
        continue;
      }
      else if (pWidget == this)
      {
        printError(i18n("Infinite loop: @%1 called inside @%2").arg(identifier).arg(identifier));
        return QString::null;
      }
      else
        evalText += pWidget->evalAssociatedText();
    }
  }
          
  return evalText;
}


QString KommanderWidget::dcopQuery(const QStringList& a_query) const
{
  QCString appId = a_query[0].latin1(), object = a_query[1].latin1();
  
  // parse function arguments
  QString function = a_query[2], pTypes;
  function.remove(' ');
  int start = function.find('(');
  bool ok = false;
  if (start != -1)
    pTypes = parseBrackets(function, start, ok);
  if (!ok)
  {
    printError(i18n("Unmatched parenthesis in DCOP call \'%1\'").arg(function));
    return QString::null;
  }
  const QStringList argTypes = parseArgs(pTypes, ok);
  if (!ok || argTypes.count() != a_query.count() - 3)
  {
    printError(i18n("Incorrect arguments in DCOP call \'%1\'").arg(function));
    return QString::null;
  }
  
  QCString replyType;
  QByteArray byteData, byteReply;
  QDataStream byteDataStream(byteData, IO_WriteOnly);
  for (uint i=0 ; i<argTypes.count(); i++) {
    if (argTypes[i] == "int")
      byteDataStream << a_query[i+3].toInt();
    else if (argTypes[i] == "long")
      byteDataStream << a_query[i+3].toLong();
    else if (argTypes[i] == "float")
      byteDataStream << a_query[i+3].toFloat();
    else if (argTypes[i] == "double")
      byteDataStream << a_query[i+3].toDouble();
    else if (argTypes[i] == "bool")
      byteDataStream << (bool)(a_query[i+3] != "false" && a_query[i+3] != "FALSE" && a_query[i] != "0");
    else if (argTypes[i] == "QStringList")
      if (a_query[i+3].find('\n') != -1)
        byteDataStream << QStringList::split("\n", a_query[i+3], true);
      else
        byteDataStream << QStringList::split("\\n", a_query[i+3], true);
    else 
      byteDataStream << a_query[i+3];
  }

  DCOPClient *cl = KApplication::dcopClient();
  if (!cl || !cl->call(appId, object, function.latin1(), byteData, replyType, byteReply))
  {
    printError(i18n("Tried to perform DCOP query, but failed"));
    return QString::null;
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
    return QString::number(i);
  }
  else if(replyType != "void")
  {
    printError(i18n("DCOP return type %1 is not yet implemented.").arg(replyType.data()));
  }

  return QString::null;
}

QString KommanderWidget::localDcopQuery(const QString function, const QStringList& args) const
{
  QStringList pArgs;
  pArgs.append(kapp->dcopClient()->appId());
  pArgs.append("KommanderIf");
  pArgs.append(function);
  for (int i=0; i<args.count(); i++)
    pArgs.append(args[i]);
  return dcopQuery(pArgs);
}
  

QString KommanderWidget::execCommand(const QString& a_command, const QString& a_shell) const
{
    MyProcess proc(this);
    QString text = proc.run(a_command.local8Bit(), a_shell.latin1());
//FIXME check if exec was successful
    return text;
}

void KommanderWidget::printError(const QString& a_error, const QString& a_className) const
{
  kdError() << i18n("In widget %1:\n\t%2\n").arg( a_className.isNull() ? 
    QString(m_thisObject->name()) : a_className ).arg(a_error);
}

// Get parent pid
QString KommanderWidget::parentPid() const
{
  QStringList simpleArg;
  simpleArg.append("_PARENTPID");
  QString parentPid = localDcopQuery("global(QString)", simpleArg); 
  if (!parentPid.isEmpty())
    return parentPid;
  else
    return QString().setNum(getppid());
} 





QString KommanderWidget::parseIdentifier(const QString& s, int& from) const
{
  uint start = from;
  while (start < s.length() && s[start].isSpace())
    start++;
  uint end = start; 
  while (end < s.length() && (s[end].isLetterOrNumber() || s[end] == '_'))
    end++;
  from = end;
  return s.mid(start, end-start);
}

QString KommanderWidget::parseBrackets(const QString& s, int& from, bool& ok) const
{
  ok = true;
  uint start = from;
  while (start < s.length() && s[start].isSpace())
    start++;
  if (start == s.length() || s[start] != '(')
    return QString::null;
  bool quoteSingle = false, quoteDouble = false;
  for (uint end = start+1; end < s.length(); end++) 
  {
    if (!quoteDouble && s[end] == '\'' && s[end-1] != '\\')
      quoteSingle = not quoteSingle;
    else if (!quoteSingle && s[end] == '\"' && s[end-1] != '\\')
      quoteDouble = not quoteDouble;
    else if (!quoteDouble && !quoteSingle && s[end] == ')') 
    {
      from = end + 1;
      return s.mid(start+1, end-start-1);
    }
  }
  ok = false;
  return QString::null;
}

QStringList KommanderWidget::parseArgs(const QString& s, bool &ok) const
{
  QStringList argList;
  bool quoteDouble = false, quoteSingle = false;
  uint i, start = 0;
  for (i = 0; i < s.length(); i++) 
  {
    if (!quoteDouble && s[i] == '\'' && s[i-1] != '\\')
      quoteSingle = not quoteSingle;
    else if (!quoteSingle && s[i] == '\"' && s[i-1] != '\\')
      quoteDouble = not quoteDouble;
    else if (!quoteDouble && !quoteSingle && s[i] == ',')
    {
      QString arg = s.mid(start, i - start).stripWhiteSpace();
      if (!arg.isEmpty())
        argList.append(evalAssociatedText(parseQuotes(arg)));
      start = i+1;
    }
  }
  if (!quoteDouble && !quoteSingle) 
  {
    QString arg = s.mid(start, s.length() - start + 1).stripWhiteSpace();
    if (!arg.isEmpty())
      argList.append(evalAssociatedText(parseQuotes(arg)));
  }
  ok = !quoteDouble && !quoteSingle;
  
  return argList;
}

QString KommanderWidget::parseQuotes(const QString& s) const
{
  if (s[0] == s[s.length()-1] && (s[0] == '\'' || s[0] == '\"'))
    return s.mid(1, s.length()-2);
  else return s;
}


KommanderWidget* KommanderWidget::parseWidget(const QString& name) const
{
  QObject *childObj = parentDialog()->child(name.latin1());
  return dynamic_cast <KommanderWidget *>(childObj);
}

QObject* KommanderWidget::parentDialog() const
{
  QObject *superParent = m_thisObject;
  while (superParent->parent())
  {
    superParent = superParent->parent();
    if (superParent->inherits("Dialog"))
      break;
  }
  return superParent;
}

void KommanderWidget::registerFunctions()
{
  if (m_functions.count())
    return;
  
  /* functions without arguments */
  registerFunction("widgetText");
  registerFunction("selectedWidgetText");
  registerFunction("pid");
  registerFunction("dcopid");
  registerFunction("parentPid");
  registerFunction("execBegin", 0, 1);
  /* functions with one argument */
  registerFunction("exec", 1);
  registerFunction("global", 1);
  registerFunction("env", 1);
  registerFunction("dialog", 1);
  /* functions with two arguments */
  registerFunction("readSetting", 2);  
  registerFunction("writeSetting", 2);
  registerFunction("setGlobal", 2);
  /* functions with four arguments */
  registerFunction("dcop", 4, 10);
}

void KommanderWidget::registerFunction(const QString& name, uint minarg, uint maxarg)
{
  if (maxarg < minarg)
    maxarg = minarg;
  m_functions.insert(name, QPair<uint, uint>(minarg, maxarg));
}


QMap<QString, QPair<uint, uint> > KommanderWidget::m_functions;

bool KommanderWidget::inEditor = false;