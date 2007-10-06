/***************************************************************************
                    kommanderfunctions.cpp - Text widget core functionality 
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

#include <iostream>
#include <stdlib.h> 

#include <qfile.h>
#include <qregexp.h>
#include <qtextstream.h>
 
#include <dcopclient.h>
#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>

#include "kommanderwidget.h"
#include "specials.h"
#include "expression.h"
#include "parser.h"

QString KommanderWidget::evalFunction(const QString& function, const QStringList& args)
{ 
  switch (SpecialInformation::function(Group::Kommander, function)) {
    case Kommander::widgetText:
      return handleDCOP(DCOP::text);
    case Kommander::selectedWidgetText:
      return handleDCOP(DCOP::selection);
    case Kommander::dcopid:
      return kapp->dcopClient()->appId();
    case Kommander::pid:
      return QString().setNum(getpid());
    case Kommander::null:
      return QString();
    case Kommander::comment:
      return QString("#");
    case Kommander::exec:
      return execCommand(args[0]);
    case Kommander::dcop:
      return DCOPQuery(args);
    case Kommander::parentPid:
      return global("_PARENTPID").isEmpty() ? QString().setNum(getppid()) : global("_PARENTPID");
    case Kommander::env:
      return QString(getenv(args[0].latin1())); 
    case Kommander::i18n:
      return KGlobal::locale()->translate(args[0]);
    case Kommander::global:
      return global(args[0]);
    case Kommander::setGlobal:
      setGlobal(args[0], args[1]); 
      return QString();
    case Kommander::debug:
      qDebug("%s", args[0].latin1());
      return QString::null;
    case Kommander::echo:
      for (uint i=0; i<args.count(); i++)
         std::cout << args[i].latin1();
      fflush(stdout);
      return QString::null;
    case Kommander::readSetting:
    {
      QString fname = fileName();
      if (!fname.isEmpty())
      {
        KConfig cfg("kommanderrc", true);
        cfg.setGroup(fname);
        return cfg.readEntry(args[0], args[1]);
      }
      return QString::null;
    }
    case Kommander::writeSetting:
    {
      QString fname = fileName();
      if (!fname.isEmpty())
      {
        KConfig cfg("kommanderrc", false);
        cfg.setGroup(fname);
        cfg.writeEntry(args[0], args[1]);
      }
      return QString::null;
    }
    case Kommander::dialog:
      if (args.count() > 1)
        return runDialog(args[0], args[1]); 
      else
        return runDialog(args[0]); 
    case Kommander::expr:
    {
      Expression expr(args[0]);
      bool ok;
      QVariant value = expr.value(&ok);
      return ok ? value.toString() : QString();
    }
    default:
      return QString();
  }
}


QString KommanderWidget::evalExecBlock(const QStringList& args, const QString& s, int& pos) 
{
  int f = s.find("@execEnd", pos);  
  if (f == -1)
  {
    printError(i18n("Unterminated @execBegin ... @execEnd block."));
    return QString();
  } 
  else
  {
    QString shell = args.count() ? args[0] : QString();
    int start = pos;
    pos = f + QString("@execEnd").length()+1;
    return execCommand(evalAssociatedText(s.mid(start, f - start)), shell);
  }
}
 
QString KommanderWidget::evalForEachBlock(const QStringList& args, const QString& s, int& pos) 
{
  int f = s.find("@end", pos);  
//FIXME: better detection of block boundaries  
  if (f == -1)
  {
    printError(i18n("Unterminated @forEach ... @end block."));
    return QString();
  }
  else
  {
    int start = pos;
    pos = f + QString("@end").length()+1;
    QString var = args[0];
    QStringList loop = QStringList::split("\n", args[1]);
    QString output;
    QString block = substituteVariable(s.mid(start, f - start), QString("%1_count").arg(var),
      QString::number(loop.count()));
    QString varidx = QString("%1_index").arg(var);
    for (uint i=0; i<loop.count(); i++)
      output += evalAssociatedText(substituteVariable(substituteVariable(block, varidx, QString::number(i+1)),
        var, loop[i]));
    return output;
  }
}

QString KommanderWidget::evalForBlock(const QStringList& args, const QString& s, int& pos) 
{
  int f = s.find("@end", pos);  
//FIXME: better detection of block boundaries  
  if (f == -1)
  {
    printError(i18n("Unterminated @forEach ... @end block."));
    return QString();
  } 
  else
  {
    int start = pos;
    pos = f + QString("@end").length()+1;
    QString block = s.mid(start, f - start);
    QString variable = args[0];

    Expression expr;
    int loopstart = expr.value(args[1]).toInt();
    int loopend = expr.value(args[2]).toInt();
    int loopstep = 1;
    if (args.count() > 3)
    {
      loopstep = expr.value(args[3]).toInt();  
      if (!loopstep)
        loopstep = 1;
    }

    QString output;
    for (int i=loopstart; i<=loopend; i+=loopstep)
    {
      output += evalAssociatedText(substituteVariable(block, variable, QString::number(i)));
    }
    return output;
  }
}

QString KommanderWidget::evalIfBlock(const QStringList& args, const QString& s, int& pos) 
{
  int f = s.find("@endif", pos);
//FIXME: better detection of block boundaries; add error message
  if (f == -1)
  {
    pos = s.length()+1;
    printError(i18n("Unterminated @if ... @endif block."));
    return QString();
  }
  else
  {
    QString block = s.mid(pos, f - pos);
    pos = f + QString("@endif").length()+1;
    Expression expr;
    if (expr.isTrue(args[0]))
      return evalAssociatedText(block);
    return QString();
  }
}

QString KommanderWidget::evalSwitchBlock(const QStringList& args, const QString& s, int& pos) 
{
  int f = s.find("@end", pos);
//FIXME: better detection of block boundaries; add error message
  if (f == -1)
  {
    printError(i18n("Unterminated @switch ... @end block."));
    return QString();
  }
  else
  {
    QString block = s.mid(pos, f - pos);
    pos = f + QString("@end").length()+1;
    f = parseBlockBoundary(block, 0, "@case");
    bool finished = f == -1;
    while (!finished)
    {
      f += 5;
      int end = parseBlockBoundary(block, f, "@case");
      if (end == -1) 
      {
        end = block.length();
        finished = true;
      }
      bool ok;
      QString value = parseBrackets(block, f, ok);
      if (!ok) 
        break;
      if (value == args[0] || value == "*")
        return evalAssociatedText(block.mid(f, end-f));
      f = end;
    }
    return QString();
  }
}



QString KommanderWidget::evalArrayFunction(const QString& function, const QStringList& args)
{
  Parser parser(internalParserData());
  int fname = SpecialInformation::function(Group::Array, function);
  QString array = args[0].startsWith("_") ? args[0] : QString("_")+ args[0];

  if (fname == Array::setValue)
    parser.setArray(array, args[1], args[2]);
  else if (fname == Array::fromString)
  {
    parser.unsetArray(array);
    QStringList lines = QStringList::split("\n", args[1]);
    for (QStringList::Iterator it = lines.begin(); it != lines.end(); ++it)
    {
      QString key = (*it).section('\t', 0, 0).stripWhiteSpace();
      if (!key.isEmpty())
        parser.setArray(array, key, (*it).section('\t', 1));
    }
  }
  else if (!parser.isArray(array))
    return QString();
  else switch (fname) {
    case Array::value:
      return parser.arrayValue(array, args[1]).toString();
    case Array::keys:
    {
      const QMap<QString, ParseNode> map = parser.array(array);
      QStringList keys;
      for (QMap<QString, ParseNode>::ConstIterator it = map.begin(); it != map.end(); ++it)
        keys.append(it.key());
      return keys.join("\n");
    }
    case Array::values:
    {
      const QMap<QString, ParseNode> map = parser.array(array);
      QStringList values;
      for (QMap<QString, ParseNode>::ConstIterator it = map.begin(); it != map.end(); ++it)
        values.append(it.data().toString());
      return values.join("\n");
    }
    case Array::clear:
      parser.unsetArray(array);
      return QString();
    case Array::remove:
      parser.unsetArray(array, args[1]);
      return QString();
    case Array::count:
      return QString::number(parser.array(array).count());
    case Array::toString:
    {
      const QMap<QString, ParseNode> map = parser.array(array);
      QString arraystring;
      for (QMap<QString, ParseNode>::ConstIterator it = map.begin(); it != map.end(); ++it)
        arraystring += QString("%1\t%2\n").arg(it.key()).arg(it.data().toString());
      return arraystring;
    }
    default:
      return QString();
  }
  return QString();
}


QString KommanderWidget::evalWidgetFunction(const QString& identifier, const QString& s, int& pos)
{
  KommanderWidget* pWidget = parseWidget(identifier);
  if (!pWidget) 
  {
    printError(i18n("Unknown widget: @%1.").arg(identifier));
    return QString();
  }
  if (s[pos] == '.')
  {
    pos++;
    bool ok = true;
    QString function = parseIdentifier(s, pos);
    QStringList args = parseFunction("DCOP", function, s, pos, ok);
    if (!ok)
      return QString();
    args.prepend(pWidget->widgetName());
    QString prototype = SpecialInformation::prototype(Group::DCOP,
      SpecialInformation::function(Group::DCOP, function));
    return localDCOPQuery(prototype, args);
  }
  else if (pWidget == this)
  {
    printError(i18n("Infinite loop: @%1 called inside @%2.").arg(pWidget->widgetName())
        .arg(pWidget->widgetName()));
    return QString();
  }
  else if (!pWidget->hasAssociatedText())
  {
    printError(i18n("Script for @%1 is empty.").arg(pWidget->widgetName()));
    return QString();
  }
  return pWidget->evalAssociatedText();
}

