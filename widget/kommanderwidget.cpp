 /***************************************************************************
                    kommanderwidget.cpp - Text widget core functionality 
                             -------------------
    copyright          : (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
                         (C) 2004      Michal Rudolf <mrudolf@kdewebdwev.org>
    
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
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>

/* QT INCLUDES */
#include <qapplication.h>
#include <qcstring.h>
#include <qdatastream.h>
#include <qfileinfo.h>
#include <qobject.h>
#include <qobjectlist.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qvariant.h>


/* UNIX INCLUDES */
#include <unistd.h>
#include <stdlib.h>

/* OTHER INCLUDES */
#include "myprocess.h"
#include "kommanderwidget.h"
#include "specials.h"
#include "specialinformation.h"


KommanderWidget::KommanderWidget(QObject *a_thisObject)
{
  m_thisObject = a_thisObject;
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

bool KommanderWidget::hasAssociatedText()
{
  int index = (states().findIndex( currentState()));
  if (index == -1 || m_associatedText[index].isEmpty())
    return false;
  return true;
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

QString KommanderWidget::evalAssociatedText() // expands and returns associated text as a string
{
  int index = ( states().findIndex( currentState()) );
  if (index == -1)
  {
    printError(i18n("Invalid state for associated text."));
    return QString::null;
  }
  return evalAssociatedText(m_associatedText[index]);
}

QString KommanderWidget::evalAssociatedText(const QString& a_text)
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
    
    /* escaped @ */
    if (pos < baseTextLength-1 && a_text[pos] == ESCCHAR)
    {
      evalText += ESCCHAR;
      pos++;
      continue;
    }
    
    QString identifier = parseIdentifier(a_text, pos);
    /* comment */
    if (identifier.isEmpty()) 
    {
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
    
    bool ok = true;
    QStringList args;
    
    /* Standard, non-prefixed special */
    if (SpecialInformation::function(Group::Kommander, identifier) != -1) 
    {    
      args = parseFunction(QString::null, identifier, a_text, pos, ok);
      if (!ok)
        return QString::null;
      else if (identifier == "execBegin")
        evalText += evalExecBlock(args, a_text, pos);  
      else
        evalText += evalFunction(identifier, args);
      
        
    }
    /* Widget special */
    else if (parseWidget(identifier))
      evalText += evalWidgetFunction(identifier, a_text, pos);
    else if (a_text[pos] == '.')
    {
      pos++;
      QString function = parseIdentifier(a_text, pos);
      args = parseFunction(identifier, function, a_text, pos, ok);
      if (!ok)
        return QString::null;
      switch (SpecialInformation::group(identifier))
      {
        case Group::Array:
          evalText += evalArrayFunction(function, args);
          break;
        case Group::String:
          evalText += evalStringFunction(function, args);
        case Group::File:
          evalText += evalFileFunction(function, args);
          break;
        default:
          return QString::null;
      }
    }
    else
    {
      printError(i18n("Unknown special: \'%1\'.").arg(identifier));
      return QString::null;
    }
  }
          
  return evalText;
}


QString KommanderWidget::DCOPQuery(const QStringList& a_query)
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
    printError(i18n("Unmatched parenthesis in DCOP call \'%1\'.").arg(function));
    return QString::null;
  }
  const QStringList argTypes = parseArgs(pTypes, ok);
  if (!ok || argTypes.count() != a_query.count() - 3)
  {
    printError(i18n("Incorrect arguments in DCOP call \'%1\'.").arg(function));
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
    printError(i18n("Tried to perform DCOP query, but failed."));
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

QString KommanderWidget::localDCOPQuery(const QString function, const QStringList& args)
{
  QStringList pArgs;
  pArgs.append(kapp->dcopClient()->appId());
  pArgs.append("KommanderIf");
  pArgs.append(function);
  for (uint i=0; i<args.count(); i++)
    pArgs.append(args[i]);
  return DCOPQuery(pArgs);
}
  
QString KommanderWidget::localDCOPQuery(const QString function, const QString& arg1, 
     const QString& arg2, const QString& arg3, const QString& arg4)
{
  QStringList pArgs;
  pArgs.append(kapp->dcopClient()->appId());
  pArgs.append("KommanderIf");
  pArgs.append(function);
  pArgs.append(arg1);
  pArgs.append(arg2);
  if (arg3 != QString::null)
    pArgs.append(arg3);
  if (arg4 != QString::null)
    pArgs.append(arg4);
  return DCOPQuery(pArgs);
}


QString KommanderWidget::execCommand(const QString& a_command, const QString& a_shell) const
{
  MyProcess proc(this);
  QString text = proc.run(a_command.local8Bit(), a_shell.latin1());
//FIXME check if exec was successful
  return text;
}

QString KommanderWidget::runDialog(const QString& a_dialog, const QString& a_params)
{
  QString pFileName = localDCOPQuery("global(QString)", "_KDDIR") + QString("/") + a_dialog;
  QFileInfo pDialogFile(pFileName);
  if (!pDialogFile.exists()) 
  {
    pFileName = a_dialog;
    pDialogFile.setFile(pFileName);
    if (!pDialogFile.exists())
      return QString::null;
  }
  QString cmd = QString("kmdr-executor %1 %2 _PARENTPID=%3 _PARENTDCOPID=kmdr-executor-%4")
    .arg(pFileName).arg(a_params).arg(getpid()).arg(getpid());
  return execCommand(cmd);
}


void KommanderWidget::printError(const QString& a_error) const
{
  if (showErrors) 
  {
    KMessageBox::error(0, i18n("<qt>Error in widget <b>%1</b>:<p><i>%2</i></qt>")
       .arg(QString(m_thisObject->name())).arg(a_error)); 
  }
  else 
  {
    kdError() << i18n("Error in widget %1:\n  %2\n").arg(m_thisObject->name()).arg(a_error);
  }
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
  int brackets = 1;
  for (uint end = start+1; end < s.length(); end++) 
  {
    if (!quoteDouble && s[end] == '\'' && s[end-1] != '\\')
      quoteSingle = !quoteSingle;
    else if (!quoteSingle && s[end] == '\"' && s[end-1] != '\\')
      quoteDouble = !quoteDouble;
    else if (!quoteDouble && !quoteSingle && s[end] == '(') 
      brackets++;
    else if (!quoteDouble && !quoteSingle && s[end] == ')') 
    {
      brackets--;
      if (!brackets) {
        from = end + 1;
        return s.mid(start+1, end-start-1);
      }
    }
  }
  ok = false;
  return QString::null;
}


QStringList KommanderWidget::parseArgs(const QString& s, bool &ok)
{
  QStringList argList;
  bool quoteDouble = false, quoteSingle = false;
  uint i, start = 0, brackets=0;
  for (i = 0; i < s.length(); i++) 
  {
    /* Handle brackets */
    if (s[i] == '(' && !quoteSingle && !quoteDouble)
      brackets++;
    else if (s[i] == ')' && !quoteSingle && !quoteDouble)
      brackets--;
    /* Ignore everything in brackets */
    else if (!brackets) 
    {
      if (s[i] == '\'' && s[i-1] != '\\' && !quoteDouble)
         quoteSingle = !quoteSingle;
      else if (s[i] == '\"' && s[i-1] != '\\' && !quoteSingle)
         quoteDouble = !quoteDouble;
      else if (s[i] == ',' && !quoteDouble && !quoteSingle)
      {
        QString arg = s.mid(start, i - start).stripWhiteSpace();
        if (!arg.isEmpty())
          argList.append(evalAssociatedText(parseQuotes(arg)));
        start = i+1;
      }  
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

QStringList KommanderWidget::parseFunction(const QString group, const QString& function, 
    const QString& s, int& from, bool& ok)
{
  ok = true;
  QString arg = parseBrackets(s, from, ok);
  if (!ok)
  {
    printError(i18n("Unmatched parenthesis after \'%1\'.").arg(function));
    return QString::null;
  }
  const QStringList args = parseArgs(arg, ok);
  int gname = SpecialInformation::group(group);
  int fname = SpecialInformation::function(gname, function);
  
  if (!ok)
    printError(i18n("Unmatched quotes in argument of \'%1\'.").arg(function));
  else if (gname == -1)
    printError(i18n("Unknown function group: \'%1\'.").arg(group));
  else if (fname == -1)
    printError(i18n("Unknown function: \'%1\' in group '%2'.").arg(function).arg(group));
  else if ((int)args.count() < SpecialInformation::minArg(gname, fname))
    printError(i18n("Not enough arguments for \'%1\' (%2 instead of %3).")
        .arg(function).arg(args.count()).arg(SpecialInformation::minArg(gname, fname)));
  else if ((int)args.count() > SpecialInformation::maxArg(gname, fname))
    printError(i18n("Too many arguments for \'%1\' (%2 instead of %3).")
      .arg(function).arg(args.count()).arg(SpecialInformation::maxArg(gname, fname)));
  return args;
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




QString KommanderWidget::global(const QString& variableName)
{
  if (m_globals.contains(variableName))
    return m_globals[variableName];
  else
    return QString::null;
}

void KommanderWidget::setGlobal(const QString& variableName, const QString& value)
{
  m_globals.insert(variableName, value); 
}  

QString KommanderWidget::handleDCOP(const int, const QStringList&)
{
  return QString::null;
}


bool KommanderWidget::inEditor = false;
bool KommanderWidget::showErrors = true;
QMap<QString, QString> KommanderWidget::m_globals;
QMap<QString, QMap<QString, QString> > KommanderWidget::m_arrays;
