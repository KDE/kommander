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

#include <stdlib.h> 

#include <qfile.h>
#include <qtextstream.h>
 
#include <dcopclient.h>
#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>

#include "kommanderwidget.h"
#include "kommanderwindow.h"
#include "specials.h"
 
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
      return QString::null;
    case Kommander::exec:
      return execCommand(args[0]);
    case Kommander::dcop:
      return DCOPQuery(args);
    case Kommander::parentPid:
      return global("_PARENTPID").isEmpty() ? QString().setNum(getppid()) : global("_PARENTPID");
    case Kommander::env:
      return QString(getenv(args[0].latin1())); 
    case Kommander::global:
      return global(args[0]);
    case Kommander::setGlobal:
      setGlobal(args[0], args[1]); 
      return QString::null;
    case Kommander::readSetting:
    {
      KommanderWindow* window = dynamic_cast<KommanderWindow*>(parentDialog());
      if (window)
      {
        KConfig cfg("kommanderrc", true);
        cfg.setGroup(QString(window->fileName()));
        return cfg.readEntry(args[0], args[1]);
      }
      return QString::null;
    }
    case Kommander::writeSetting:
    {
      KommanderWindow* window = dynamic_cast<KommanderWindow*>(parentDialog());
      if (window)
      {
        KConfig cfg("kommanderrc", false);
        cfg.setGroup(QString(window->fileName()));
        cfg.writeEntry(args[0], args[1]);
      }
      return QString::null;
    }
    case Kommander::dialog:
      if (args.count() > 1)
        return runDialog(args[0], args[1]); 
      else
        return runDialog(args[0]); 
    default:
      return QString::null;
  }
}


QString KommanderWidget::evalExecBlock(const QStringList& args, const QString& s, int& pos) 
{
  int f = s.find("@execEnd", pos);  
  if (f == -1)
  {
    printError(i18n("Unterminated @execBegin ... @execEnd block."));
    return QString::null;
  } 
  else
  {
    QString shell = args.count() ? args[0] : QString::null;
    int start = pos;
    pos = f + QString("@execBegin").length() -1;
    return execCommand(evalAssociatedText(s.mid(start, f - start)), shell);
  }
}
 



QString KommanderWidget::evalArrayFunction(const QString& function, const QStringList& args) const
{
  int fname = SpecialInformation::function(Group::Array, function);
  if (fname == Array::setValue)
    m_arrays[args[0]][args[1]] = args[2];
  else if (fname == Array::fromString)
  {
    QStringList lines = QStringList::split("\n", args[1]);
    for (uint i=0; i<lines.count(); i++)
    {
        QString key = lines[i].section('\t', 0, 0).stripWhiteSpace();
        if (!key.isEmpty())
          m_arrays[args[0]][key] = lines[i].section('\t', 1);
    }
  }
  else if (!m_arrays.contains(args[0]))
    return QString::null;
  else switch (fname) {
    case Array::value:
      return m_arrays[args[0]].contains(args[1]) ? m_arrays[args[0]][args[1]] : QString::null;
    case Array::keys:
      return QStringList(m_arrays[args[0]].keys()).join("\n");
    case Array::values:
      return QStringList(m_arrays[args[0]].values()).join("\n");
    case Array::clear:
      m_arrays[args[0]].clear();
      return QString::null;
    case Array::remove:
      m_arrays[args[0]].remove(args[1]);
      return QString::null;
    case Array::count:
      return QString::number(m_arrays[args[0]].count());
    case Array::toString:
    {
      QStringList keys = m_arrays[args[0]].keys();
      QStringList values = m_arrays[args[0]].values();
      QString array;
      for (uint i=0; i<keys.count(); i++)
          array += QString("%1\t%2\n").arg(keys[i]).arg(values[i]);
      return array;
    }
    default: 
      return QString::null;
  }
  return QString::null;
}

QString KommanderWidget::evalFileFunction(const QString& function, const QStringList& args) const
{
  int fname = SpecialInformation::function(Group::File, function);
  QFile file(args[0]);
  
  if (fname == File::read && file.open(IO_ReadOnly))
  {
    QTextStream text(&file);
    return text.read();
  }
  else if (fname == File::write && file.open(IO_WriteOnly))
  {
    QTextStream text(&file);
    text << args[1];
  }
  else if (fname == File::append && file.open(IO_Append))
  {
    QTextStream text(&file);
    text << args[1];
  }
  return QString::null;
}




QString KommanderWidget::evalWidgetFunction(const QString& identifier, const QString& s, int& pos)
{
  KommanderWidget* pWidget = parseWidget(identifier);
  if (!pWidget) 
  {
    printError(i18n("Unknown widget: @%1.").arg(identifier));
    return QString::null;
  }
  if (pWidget == this)
  {
    printError(i18n("Infinite loop: @%1 called inside @%2.").arg(identifier).arg(identifier));
    return QString::null;
  }
  if (s[pos] == '.')
  {
    pos++;
    bool ok = true;
    QString function = parseIdentifier(s, pos);
    QString prototype = SpecialInformation::prototype(Group::DCOP,
      SpecialInformation::function(Group::DCOP, function));
    if (prototype.isNull())
    {
      printError(i18n("Unknown DCOP function: '%1'.").arg(function));
      return QString::null;
    }
    QString brackets = parseBrackets(s, pos, ok);
    if (!ok)
    {
      printError(i18n("Unmatched parenthesis after \'@%1.%2\'.").arg(identifier)
          .arg(function));
      return QString::null;
    }
    QStringList args;
    args.append(identifier);
    args += parseArgs(brackets, ok);
    if (!ok)
    {
      printError(i18n("Unmatched quotes in argument of \'@%1.%2\'.").arg(identifier)
            .arg(function));
        return QString::null;
      }
      return localDCOPQuery(prototype, args);
  }
  else if (!pWidget->hasAssociatedText())
  {
    printError(i18n("Script for @%1 is empty.").arg(identifier));
    return QString::null;
  }
  return pWidget->evalAssociatedText();
}


QString KommanderWidget::evalStringFunction(const QString& function, const QStringList& args) const
{
  switch (SpecialInformation::function(Group::String, function)) {
    case String::length:
      return QString::number(args[0].length());
    case String::contains:
      return QString::number(args[0].contains(args[1]));
    case String::find:
      return QString::number(args[0].find(args[1]));
    case String::left:
      return args[0].left(args[1].toInt());
    case String::right:
      return args[0].right(args[1].toInt());
    case String::mid:
      return args[0].mid(args[1].toInt(), args[2].toInt());
    case String::remove:
      return QString(args[0]).remove(args[1]);
    case String::replace:
      return QString(args[0]).replace(args[1], args[2]);
    case String::lower:
      return args[0].lower();
    case String::upper:
      return args[0].upper();
    case String::isEmpty:
      return QString::number(args[0].isEmpty());
    case String::compare:
    {
      int compare = args[0].compare(args[1]);
      return compare < 0 ? "-1" : (compare == 0 ? "0" : "1");
    }
    case String::isNumber:
    {
      bool ok;
      args[0].toInt(&ok);
      return QString::number(ok);
    }
    default:
      return QString::null;
  }
}

