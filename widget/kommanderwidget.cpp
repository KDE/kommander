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
#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <k3process.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qdatastream.h>
#include <qfileinfo.h>
#include <qobject.h>
#include <qobject.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlist.h>
#include <qvariant.h>
//Added by qt3to4:
//#include <Q3MemArray>


/* UNIX INCLUDES */
#include <unistd.h>
#include <stdlib.h>

/* OTHER INCLUDES */
#include "myprocess.h"
#include "kommanderwidget.h"
#include "specials.h"
#include "specialinformation.h"
#include "parser.h"
#include "parserdata.h"
#include "kommanderwindow.h"

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
  m_associatedText += QString(); // sync states and associations
}

QStringList KommanderWidget::associatedText() const
{
  return m_associatedText;
}

bool KommanderWidget::hasAssociatedText()
{
  int index = states().indexOf(currentState());
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
  int index = ( states().indexOf( currentState()) );
  if (index == -1)
  {
    printError(i18n("Invalid state for associated text."));
    return QString();
  }
  return evalAssociatedText(m_associatedText[index]);
}

QString KommanderWidget::evalAssociatedText(const QString& a_text)
{
  /* New internal parser is used if global flag is set */
  if ((KommanderWidget::useInternalParser && !a_text.startsWith("#!")) || a_text.startsWith("#!kommander"))
  {
    Parser p(internalParserData());
    p.setString(a_text);
    p.setWidget(this);
    if (!p.parse())
    {
      // FIXME add widget's name to KommanderWidget class      
      printError(i18n("Line %1: %2.\n", p.errorLine()+1, p.errorMessage()));
    }
    return QString();
  }
  
  /* Old macro-only parser is implemented below  */
  
  QString evalText;
  
  int pos = 0, baseTextLength = a_text.length();
  while (pos < baseTextLength)
  {
    int ident = a_text.indexOf(ESCCHAR, pos);
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
        int newpos = a_text.indexOf('\n', pos+1);
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
      args = parseFunction("Kommander", identifier, a_text, pos, ok);
      if (!ok)
        return QString();
      else if (identifier == "execBegin")
        evalText += evalExecBlock(args, a_text, pos);  
      else if (identifier == "forEach")
        evalText += evalForEachBlock(args, a_text, pos);  
      else if (identifier == "for")
        evalText += evalForBlock(args, a_text, pos);  
      else if (identifier == "switch")
        evalText += evalSwitchBlock(args, a_text, pos);  
      else if (identifier == "if")
        evalText += evalIfBlock(args, a_text, pos);  
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
        return QString();
      switch (SpecialInformation::group(identifier))
      {
        case Group::Array:
          evalText += evalArrayFunction(function, args);
          break;
        case Group::String:
          evalText += Parser::function(internalParserData(), "str_" + function, args);
          break;
        case Group::File:
          evalText += Parser::function(internalParserData(), "file_" + function, args);
          break;
        case Group::Message:
          evalText += Parser::function(internalParserData(), "message_" + function, args);
          break;
        case Group::Input:
          evalText += Parser::function(internalParserData(), "input_" + function, args);
          break;
        default:
          return QString();
      }
    }
    else
    {
      printError(i18n("Unknown special: \'%1\'.", identifier));
      return QString();
    }
  }
          
  return evalText;
}

// FIX ME DCOP to dbus
QString KommanderWidget::DBUSQuery(const QStringList& a_query)
{
  return QString();
  QString appId = a_query[0].toLatin1(), object = a_query[1].toLatin1();
  
  // parse function arguments
  QString function = a_query[2], pTypes;
  function.remove(' ');
  int start = function.indexOf('(');
  bool ok = false;
  if (start != -1)
    pTypes = parseBrackets(function, start, ok);
  if (!ok)
  {
    printError(i18n("Unmatched parenthesis in D-Bus call \'%1\'.", a_query[2]));
    return QString();
  }
  const QStringList argTypes = parseArgs(pTypes, ok);
  if (!ok || argTypes.count() != a_query.count() - 3)
  {
    printError(i18n("Incorrect arguments in D-Bus call \'%1\'.", a_query[2]));
    return QString();
  }
  
  /*Q3CString replyType;
  QByteArray byteData, byteReply;
  QDataStream byteDataStream(byteData, QIODevice::WriteOnly);
  for (int i=0 ; i<argTypes.count(); i++) {
    if (argTypes[i] == "int")
      byteDataStream << a_query[i+3].toInt();
    else if (argTypes[i] == "long")
      byteDataStream << a_query[i+3].toLong();
    else if (argTypes[i] == "float")
      byteDataStream << a_query[i+3].toFloat();
    else if (argTypes[i] == "double")
      byteDataStream << a_query[i+3].toDouble();
    else if (argTypes[i] == "bool")
      byteDataStream << (bool)(a_query[i+3] != "false" && a_query[i+3] != "false" && a_query[i+3] != "0");
    else if (argTypes[i] == "QStringList")
      if (a_query[i+3].indexOf('\n') != -1)
        byteDataStream << QStringList::split("\n", a_query[i+3], true);
      else
        byteDataStream << QStringList::split("\\n", a_query[i+3], true);
    else 
      byteDataStream << a_query[i+3];
  }

  DBUSClient *cl = KApplication::dcopClient();
  if (!cl || !cl->call(appId, object, function.toLatin1(), byteData, replyType, byteReply))
  {
    printError(i18n("Tried to perform D-Bus query, but failed."));
    return QString(); 
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
    return QString::number(i);
  }
  else if(replyType == "bool")
  {
    bool b;
    byteReplyStream >> b;
    return QString::number(b);
  }
  else if (replyType == "QStringList")
  {
    QStringList text;
    byteReplyStream >> text;
    return text.join("\n");
  }
  else if(replyType != "void")
  {
    printError(i18n("D-Bus return type %1 is not yet implemented.", replyType.data()));
  }
*/
  return QString();
}

//FIXME DCOP to DBUS conversion
QString KommanderWidget::localDBUSQuery(const QString function, const QStringList& args)
{
  return QString();
  QStringList pArgs;
  //pArgs.append(kapp->dcopClient()->appId());
  pArgs.append("KommanderIf");
  pArgs.append(function);
  for (int i=0; i<args.count(); i++)
    pArgs.append(args[i]);
  return DBUSQuery(pArgs);
}
//FIXME DCOP to DBUS
QString KommanderWidget::localDBUSQuery(const QString function, const QString& arg1, 
     const QString& arg2, const QString& arg3, const QString& arg4)
{
  QStringList pArgs;
  //pArgs.append(kapp->dcopClient()->appId());
  pArgs.append("KommanderIf");
  pArgs.append(function);
  pArgs.append(arg1);
  pArgs.append(arg2);
  if (!arg3.isNull())
    pArgs.append(arg3);
  if (!arg4.isNull())
    pArgs.append(arg4);
  return DBUSQuery(pArgs);
}


QString KommanderWidget::execCommand(const QString& a_command, const QString& a_shell) const
{
  MyProcess proc(this);
  QString text = proc.run(a_command.toLocal8Bit(), a_shell.toLatin1());
//FIXME check if exec was successful
  return text;
}

QString KommanderWidget::runDialog(const QString& a_dialog, const QString& a_params)
{
  //FIXME dcop to dbus conversion
  /*QString pFileName =  localDBUSQuery("DBUSal(QString)", "_KDDIR") +  QString("/") + a_dialog;
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
  return execCommand(cmd);
*/
  return QString();
 
}


void KommanderWidget::printError(const QString& a_error) const
{
  if (showErrors) 
  {
    //FIXME create a warningYesNoCancel
 /*   KDialog* dialog = new KDialog("Error", KDialog::Yes | KDialog::No | KDialog::Cancel,
                KDialog::Yes, KDialog::No, 0, 0, true, false, 
                i18n("Continue"), i18n("Continue && Ignore Next Errors"), i18n("Stop"));
    switch (KMessageBox::createKMessageBox(dialog, QMessageBox::Warning, 
                i18n("<qt>Error in widget <b>%1</b>:<p><i>%2</i></qt>", QString(m_thisObject->name()),
                     a_error), QStringList(), QString::null, 0, 0))	//krazy:exclude=nullstrassign for old broken gcc
    {
      case KDialog::No:
        showErrors = false;
      case KDialog::Yes:
        break;
      case KDialog::Cancel:
        if (parentDialog()->inherits("QDialog"))
        {
          parentDialog()->close();
          exit(-1);
        }
        else if (parentDialog()->inherits("QMainWindow"))
          kapp->quit();
    }
    */
  }
  else 
  {
    kError() << i18n("Error in widget %1:\n  %2\n", m_thisObject->objectName(), a_error);
  }
}



QString KommanderWidget::parseIdentifier(const QString& s, int& from) const
{
  int start = from;
  while (start < s.length() && s[start].isSpace())
    start++;
  int end = start; 
  while (end < s.length() && (s[end].isLetterOrNumber() || s[end] == '_'))
    end++;
  from = end;
  return s.mid(start, end-start);
}

QString KommanderWidget::parseBrackets(const QString& s, int& from, bool& ok) const
{
  ok = true;
  int start = from;
  while (start < s.length() && s[start].isSpace())
    start++;
  if (start == s.length() || s[start] != '(')
    return QString();
  bool quoteSingle = false, quoteDouble = false;
  int brackets = 1;
  for (int end = start+1; end < s.length(); end++) 
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
  return QString();
}


QStringList KommanderWidget::parseArgs(const QString& s, bool &ok)
{
  QStringList argList;
  bool quoteDouble = false, quoteSingle = false;
  int i, start = 0, brackets=0;
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
        QString arg = s.mid(start, i - start).trimmed();
        if (!arg.isEmpty())
          argList.append(evalAssociatedText(parseQuotes(arg)));
        start = i+1;
      }  
    }
  }
  if (!quoteDouble && !quoteSingle) 
  {
    QString arg = s.mid(start, s.length() - start + 1).trimmed();
    if (!arg.isEmpty())
      argList.append(evalAssociatedText(parseQuotes(arg)));
  }
  ok = !quoteDouble && !quoteSingle;
  
  return argList;
}

QString KommanderWidget::parseQuotes(const QString& s) const
{
  if (s[0] == s[s.length()-1] && (s[0] == '\'' || s[0] == '\"'))
  {
    QString buf;
    int start = 0;
    int end = s.length() - 1;
    for (int i=1; i<end; i++)
      if (s[i] == '\\')
      {
        if (s[i+1] == 't')
          buf += '\t';
        else if (s[i+1] == 'n')
          buf += '\n';
        else if (s[i+1] == '\\')
          buf += '\\';
        else 
        {
          buf += s[i];
          i--;
        } 
        i++;
      }
      else
        buf += s[i];
    return QString(buf);
    //return s.mid(1, s.length()-2);
  }
  else return s;
}

bool KommanderWidget::isWidget(const QString& a_name) const
{
  return parseWidget(a_name);  
}

KommanderWidget* KommanderWidget::widgetByName(const QString& a_name) const
{
  return parseWidget(a_name);
}


KommanderWidget* KommanderWidget::parseWidget(const QString& widgetName) const
{
  if (QString(parentDialog()->objectName()) == widgetName) 
    return dynamic_cast <KommanderWidget*>(parentDialog());
  QWidget* childObj = parentDialog()->findChild<QWidget *>(widgetName.toLatin1());
  return dynamic_cast <KommanderWidget*>(childObj);
}

QStringList KommanderWidget::parseFunction(const QString group, const QString& function, 
    const QString& s, int& from, bool& ok)
{
  ok = true;
  bool success = false;
  QString arg = parseBrackets(s, from, ok);
  if (!ok)
  {
    printError(i18n("Unmatched parenthesis after \'%1\'.", function));
    return QStringList();
  }
  const QStringList args = parseArgs(arg, ok);
  int gname = SpecialInformation::group(group);
  int fname = SpecialInformation::function(gname, function);
  bool extraArg = gname == Group::DBUS;
  
  if (!ok)
    printError(i18n("Unmatched quotes in argument of \'%1\'.", function));
  else if (gname == -1)
    printError(i18n("Unknown function group: \'%1\'.", group));
  else if (fname == -1 && !extraArg)
    printError(i18n("Unknown function: \'%1\' in group '%2'.", function, group));
  else if (fname == -1 && extraArg)
    printError(i18n("Unknown widget function: \'%1\'.", function));
  else if ((int)args.count() + extraArg < SpecialInformation::minArg(gname, fname))
    printError(i18n("Not enough arguments for \'%1\' (%2 instead of %3).<p>"
       "Correct syntax is: %4",
         function, args.count() + extraArg, SpecialInformation::minArg(gname, fname),
         SpecialInformation::prototype(gname, fname, SpecialFunction::ShowArgumentNames)));
  else if ((int)args.count() + extraArg > SpecialInformation::maxArg(gname, fname))
    printError(i18n("Too many arguments for \'%1\' (%2 instead of %3).<p>"
       "Correct syntax is: %4",
       function, args.count() + extraArg, SpecialInformation::maxArg(gname, fname),
       SpecialInformation::prototype(gname, fname, SpecialFunction::ShowArgumentNames)));
  else 
    success = true;
  ok = success;
  return args;
}

int KommanderWidget::parseBlockBoundary(const QString& s, int from, const QStringList& args) const
{
  int shortest = -1;
  for (int i=0; i<args.count(); i++)
  {
    int match = s.indexOf(args[i], from);
    if (shortest > match || shortest == -1) 
      shortest = match;
  }
  return shortest;
}

int KommanderWidget::parseBlockBoundary(const QString& s, int from, const QString& arg) const
{
  int shortest = -1;
  shortest = s.indexOf(arg, from);
  return shortest;
}


QString KommanderWidget::substituteVariable(QString text, QString variable, QString value) const
{
  QString var = QString("@%1").arg(variable);
  QString newtext;
  int newpos, pos = 0;
  while (true)
  {
    newpos = text.indexOf(var, pos);
    if (newpos != -1)
    {
      newtext += text.mid(pos, newpos-pos);
      newtext += value;
      pos = newpos + var.length();
    } else
    {
      newtext += text.mid(pos);
      break;
    }
  }
  return newtext;
}



QWidget* KommanderWidget::parentDialog() const
{
  QObject *superParent = m_thisObject;
  while (superParent->parent())
  {
    superParent = superParent->parent();
    if (superParent->inherits("QDialog") || superParent->inherits("QMainWindow"))
      break;
  }
  return (QWidget*)superParent;
}




QString KommanderWidget::global(const QString& variableName)
{
  if (m_globals.contains(variableName))
    return m_globals[variableName];
  else
    return QString();
}

void KommanderWidget::setGlobal(const QString& variableName, const QString& value)
{
  m_globals.insert(variableName, value); 
}  
// FIXME dcop to dbusReplacement necessary
QString KommanderWidget::handleDBUS(const int function, const QStringList& args)
{
  QWidget* current = dynamic_cast<QWidget*>(m_thisObject);
  if (!current) 
    return QString();
  switch(function) {
    case DBUS::setEnabled:
      current->setEnabled(args[0] != "false" && args[0] != "0");
      break;
    case DBUS::setVisible:
      current->setVisible(args[0] != "false" && args[0] != "0");
      break;
    case DBUS::type:
      return current->metaObject()->className();      
    case DBUS::children:
    {
      QStringList matching;
      //FIXME Recursive search before: args.count() == 0 || args[0] != "false"
      QList<QWidget *> widgets = current->findChildren<QWidget *>( );
      QListIterator<QWidget *> it(widgets);
      while(it.hasNext())
      { 
        QWidget * w =  it.next();
        if (!w->objectName().isNull() && (dynamic_cast<KommanderWidget*>(w)))
            matching.append(w->objectName());
         
      }
      return matching.join("\n");  
    }  
  }
  return QString();
}


bool KommanderWidget::isFunctionSupported(int f)
{
  return f == DBUS::setEnabled || f == DBUS::setVisible || f == DBUS::children || f == DBUS::type;
}

bool KommanderWidget::isCommonFunction(int f)
{
  return f == DBUS::setEnabled || f == DBUS::setVisible || f == DBUS::children || f == DBUS::type;
}

ParserData* KommanderWidget::internalParserData()
{
  return m_parserData;
}

QString KommanderWidget::fileName()
{
  KommanderWindow* window = dynamic_cast<KommanderWindow*>(parentDialog());
  if (window)
    return QString(window->fileName());
  else
    return QString();
}  


bool KommanderWidget::inEditor = false;
bool KommanderWidget::showErrors = true;
bool KommanderWidget::useInternalParser = false;
QMap<QString, QString> KommanderWidget::m_globals;
QMap<QString, QMap<QString, QString> > KommanderWidget::m_arrays;
ParserData* KommanderWidget::m_parserData = new ParserData;

