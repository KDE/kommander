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
 
#include <dcopclient.h>
#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>

#include "dcopinformation.h"
#include "kommanderwidget.h"
 
QString KommanderWidget::evalFunction(const QString& function, const QStringList& args) const
{ 
  if (function == "widgetText")
    return widgetText();
  else if (function == "selectedWidgetText")
    return selectedWidgetText();
  else if (function == "dcopid")
    return kapp->dcopClient()->appId();
  else if (function == "pid")
    return QString().setNum(getpid());
  else if (function == "exec")
    return execCommand(args[0]);
  else if (function == "dcop")
    return dcopQuery(args);
  else if (function == "parentPid")
    return global("_PARENTPID").isEmpty() ? QString().setNum(getppid()) : global("_PARENTPID");
  else if (function == "env")
    return QString(getenv(args[0].latin1())); 
  else if (function == "global")
    return global(args[0]);
  else if (function == "setGlobal")
    setGlobal(args[0], args[1]); 
  else if (function == "readSetting") 
  {
    KConfig cfg("kommanderrc", true);
    cfg.setGroup(QString(parentDialog()->name()));
    return cfg.readEntry(args[0], args[1]);
  }
  else if (function == "writeSetting") 
  {
    KConfig cfg("kommanderrc", false);
    cfg.setGroup(QString(parentDialog()->name()));
    cfg.writeEntry(args[0], args[1]);
  }
  else if (function == "dialog")
  {
    if (args.count() > 1)
      return runDialog(args[0], args[1]); 
    else
      return runDialog(args[0]); 
  }
  return QString::null;
}


QString KommanderWidget::evalExecBlock(const QStringList& args, const QString& s, int& pos) const
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
  if (function == "setValue")
    m_arrays[args[0]][args[1]] = args[2];
  else if (!m_arrays.contains(args[0]))
    return QString::null;
  else if (function == "value")
    return m_arrays[args[0]].contains(args[1]) ? m_arrays[args[0]][args[1]] : QString::null;
  else if (function == "keys")
    return QStringList(m_arrays[args[0]].keys()).join("\n");
  else if (function == "values")
    return QStringList(m_arrays[args[0]].values()).join("\n");
  else if (function == "clear") 
    m_arrays[args[0]].clear();
  else if (function == "remove")
    m_arrays[args[0]].remove(args[1]);
  return QString::null;
}


QString KommanderWidget::evalWidgetFunction(const QString& identifier, const QString& s, int& pos) const
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
    QString prototype = DCOPInformation::prototype(function);
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
      return localDcopQuery(prototype, args);
  }
  else
    return pWidget->evalAssociatedText();
}


QString KommanderWidget::evalStringFunction(const QString& function, const QStringList& args) const
{
  if (function == "length")
    return QString::number(args[0].length());
  else if (function == "contains")
    return QString::number(args[0].contains(args[1]));
  else if (function == "left")
    return args[0].left(args[1].toInt());
  else if (function == "right")
    return args[0].right(args[1].toInt());
  else if (function == "mid")
    return args[0].mid(args[1].toInt(), args[2].toInt());
  else if (function == "remove")
    return QString(args[0]).remove(args[1]);
  else if (function == "replace")
    return QString(args[0]).replace(args[1], args[2]);
  else if (function == "lower")
    return args[0].lower();
  else if (function == "upper")
    return args[0].upper();
  else if (function == "compare")
    return QString::number(args[0].compare(args[1]));
  else if (function == "isEmpty")
    return QString::number(args[0].isEmpty());
  else if (function == "isNumber")
  {
    bool ok;
    args[0].toInt(&ok);
    return QString::number(ok);
  }
  return QString::null;
}

