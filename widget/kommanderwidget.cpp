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
#include <kapplication.h>
#include <dcopclient.h>
#include <kprocess.h>
#include <kconfig.h>
#include <klocale.h>
#include <kdebug.h>

/* QT INCLUDES */
#include <qvariant.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qcstring.h>
#include <qdatastream.h>
#include <qvaluelist.h>
#include <qstring.h>
#include <qapplication.h>
#include <qobject.h>
#include <qobjectlist.h>

/* UNIX INCLUDES */
#include <unistd.h>

/* OTHER INCLUDES */
#include "myprocess.h"
#include "kommanderwidget.h"

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
  QString baseText = a_text;
  int pos = 0, baseTextLength = baseText.length();
  while (pos < baseTextLength)  //expand identifiers
  {
    while (baseText[pos] != ESCCHAR && pos < baseTextLength)
      evalText += baseText[pos++];
    if (pos < baseTextLength)
    {
      QString identifier;

      for (++pos; pos < baseTextLength && (baseText[pos].isLetterOrNumber() 
         || baseText[pos] == '_'); ++pos)
        identifier += baseText[pos];

      if (identifier.isEmpty())
      {
        evalText += ESCCHAR;
        ++pos;
      } else
      {
        if (identifier == "widgetText")
          evalText += widgetText();
        else if (identifier == "dcopid")
          evalText += kapp->dcopClient()->appId();
        else if (identifier == "pid")
          evalText += QString().setNum(getpid());
        else if (identifier == "selectedWidgetText")
          evalText += selectedWidgetText();
        else                    // see if this is an identifier
        {
          QObject *superParent = m_thisObject;
          while (superParent->parent())
          {
            superParent = superParent->parent();
            if (superParent->inherits("Dialog"))
              break;
          }

          //qDebug("superParent is %s", superParent->name()); 
          QObject *childObj = superParent->child(identifier.latin1());
          KommanderWidget *childTextObj = dynamic_cast < KommanderWidget * >(childObj);
          if (childTextObj)
          {
            evalText += childTextObj->evalAssociatedText();
          } else if (identifier == "dcop" || identifier == "exec" ||
                     identifier == "readSetting"
                     || identifier == "writeSetting")
          {
            while (baseText[pos].isSpace() && pos < baseTextLength)
              ++pos;

            if (baseText[pos++] != '(')
            {
              printError(i18n("Expected \'(\' after \'%1\'").arg(identifier));
              return QString::null;
            }

            int braceCount = 1;
            int startpos = pos;
            bool inQuotes = false;
            while (pos < baseTextLength && braceCount)
            {
              if ((baseText[pos] == ')' || baseText[pos] == '('))
              {
                if (!inQuotes)  // this brace counts
                {
                  if (baseText[pos] == '(')
                    ++braceCount;
                  else if (baseText[pos] == ')')
                    --braceCount;
                }
              } else if (baseText[pos] == '\"')
              {
                if (pos <= 0 || baseText[pos - 1] != '\\')
                  inQuotes = !inQuotes;
              }
              ++pos;
            }
            if (braceCount)
            {
              printError(i18n("Unmatched parenthesis after \'%1\'").arg(identifier));
              return QString::null;
            }
            if (inQuotes)
            {
              printError(i18n("Unmatched quotes in argument of \'%1\'").arg(identifier));
              return QString::null;
            }
            QString arg = evalAssociatedText(baseText.mid(startpos, pos - startpos - 1));
            if (identifier == "dcop")
              evalText += dcopQuery(arg);
            else if (identifier == "exec")
            {
              bool ok;
              QStringList args = parseArgs(arg, ok);
              if (!ok || args.count() != 1)
              {
                printError(i18n("Error parsing arguments to @exec"));
                return QString::null;
              }
              evalText += execCommand(args[0]);
            } else
            {
              KConfig cfg("kommanderrc", identifier == "readSetting");
              cfg.setGroup(QString(superParent->name()));
              bool ok;
              QStringList args = parseArgs(arg, ok);
              if (!ok || (args.count() != 2 && (args.count() != 1 && identifier == "readSetting")))
              {
                printError(i18n("Error parsing arguments to @%1").arg(identifier));
                return QString::null;
              }
              if (identifier == "readSetting")
                evalText +=
                  cfg.readEntry(args[0],
                                (args.count() > 1 ? args[1] : QString::null));
              else
                cfg.writeEntry(args[0], args[1]);
            }
          } else if (identifier == "execBegin")
          {
            // execute everything until execEnd
            int f = baseText.find("@execEnd");
            QString execBlock;
            if (f == -1)
            {
              printError(i18n("Unterminated @execBegin ... @execEnd block"));
              execBlock = baseText.mid(pos);
            } else
            {
              execBlock = baseText.mid(pos, f - pos);
            }
            //qDebug("execBlock is %s", execBlock.latin1());
            pos += execBlock.length();
            if (f != -1)
              //terminated properly so add on the terminator length
              pos += QString("@execEnd").length();
            execBlock = evalAssociatedText(execBlock);
            //qDebug("execBlock is %s", execBlock.latin1());
            evalText += execCommand(execBlock);
          } else
          {
            int termIndex = baseText.find(identifier, pos);
            if (termIndex == -1)
            {
              printError(i18n("Unterminated text block \'%1\'").arg(identifier));
              return QString::null;
            }

            evalText += baseText.mid(pos, termIndex - pos - 1);
            pos = termIndex + identifier.length();
          }

        }
      }
    }
  }
  return evalText;
}


QStringList KommanderWidget::parseArgs( const QString &args, bool &ok ) const
{
    QString argStr = args.stripWhiteSpace();
    QStringList argv;
    ok = TRUE;
    bool inQuotes = FALSE;
    QString realArg;
    bool wasInQuotes = FALSE;//have we been inside quotes for thsi argument?
    for( int i = 0 ; i < (int)argStr.length() ; ++i )
    {
	if( argStr[i] == '\"' && (i == 0 || argStr[i-1] != '\\' ) )
	{
	    inQuotes = !inQuotes;
	    if( inQuotes )
		wasInQuotes = TRUE;
	}
	else if( !inQuotes && argStr[i] == ',' )
	{
	    if( realArg.isEmpty() && !wasInQuotes )
	    {
		ok = FALSE;
		break;
	    }
	    argv += realArg;
	    realArg = QString::null;
	    wasInQuotes = FALSE;
	}
	else if( !inQuotes && !argStr[i].isSpace() )
	{
	    ok = FALSE;
	    break;
	}
	else if( inQuotes )
	{
	    realArg += argStr[i];
	}
    }

    //realArg should not be empty when haven't been in quotes
    if( inQuotes || (realArg.isEmpty() && !wasInQuotes) )
	ok = FALSE;
    argv += realArg;
    /*
    for( int i = 0 ; i < argv.count() ; ++i )
	qDebug("arg[%d] = %s", i, argv[i].latin1());
	*/
    return argv;
}

QString KommanderWidget::dcopQuery(const QString& a_query) const
{
  int argc = 0;
  bool ok;
  QStringList argv = parseArgs(a_query, ok);
  argc = argv.count();
  if( !ok || argc != 4)
  {
    printError(i18n("Error in arguments to DCOP query; Unmatched quotes or too few arguments"));
    return QString::null;
  }
  QCString appId = argv[0].latin1(), object = argv[1].latin1(), 
     function = argv[2].latin1(), data = argv[3].latin1();

  QString evalText;
  QCString replyType;
  QByteArray byteData, byteReply;
  QDataStream byteDataStream(byteData, IO_ReadWrite);
  byteDataStream << data;

  DCOPClient *cl = KApplication::dcopClient();
  if(!cl || !cl->call(appId, object, function, byteData, replyType, byteReply))
  {
    printError(i18n("Tried to perform DCOP query, but failed"));
    return QString::null;
  }

  QDataStream byteReplyStream(byteReply, IO_ReadOnly);
  if(replyType == "QString")
  {
    QString text;
    byteReplyStream >> text;
    evalText += text;
  }
  else if(replyType == "int")
  {
    int i;
    byteReplyStream >> i;
    QVariant var(i);
    evalText += var.toString();
  }
  else if(replyType != "void")
  {
    printError(i18n("DCOP return type %1 is not yet implemented.").arg(replyType.data()));
  }

  return evalText;
}

QString KommanderWidget::execCommand(const QString& a_command) const
{
    MyProcess proc(this);
    QString text = proc.run(a_command);
//FIXME check if exec was successful
    return text;
}

void KommanderWidget::printError(const QString& a_error, const QString& a_className) const
{
  kdError() << i18n("In widget %1:\n\t%2\n").arg( a_className.isNull() ? 
    QString(m_thisObject->name()) : a_className ).arg(a_error);
}
