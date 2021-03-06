//
// C++ Interface: invokeclass
//
// Description: 
//
//
// Author: Andras Mantia <amantia@kde.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "invokeclass.h"

#include <qcolor.h>
#include <qregexp.h>

InvokeClass::InvokeClass(QObject *parent):QObject(parent)
{
  m_acceptedSlots = acceptedSlots();
}

void InvokeClass::invokeSlot(QObject *object, const QString& slot, QStringList args)
{
  QString invokeName = slot;
  invokeName = invokeName.mid(invokeName.indexOf('('));
  invokeName.prepend(QString::number(QSIGNAL_CODE) + "invoke");
  QString slotName = QString::number(QSLOT_CODE) + slot;  
  connect(this, invokeName.toAscii(), object, slotName.toAscii());

  if (args.count() == 0)  
    emit invoke();
  else
  {
    int opening = slot.indexOf('(');
    int closing = slot.indexOf(')', opening);
    QString slotArgStr = slot.mid(opening + 1, closing - opening - 1);
    uint argNum = slotArgStr.count(',') + 1;
    for (uint i = args.count(); i < argNum; i++)
      args << "";
  //poor man's invokeMetaObject
    if (slotArgStr == m_acceptedSlots[0])
      emit invoke(args[0]);
    else if (slotArgStr == m_acceptedSlots[1])
      emit invoke(args[0], args[1]);
    else if (slotArgStr == m_acceptedSlots[2])
      emit invoke(args[0].toUpper()=="TRUE" || args[0] =="1"? true : false);
    else if (slotArgStr == m_acceptedSlots[3])
      emit invoke(args[0].toInt());
    else if (slotArgStr == m_acceptedSlots[4])
      emit invoke(args[0].toInt(), args[1].toInt());
    else if (slotArgStr == m_acceptedSlots[5])
      emit invoke(args[0].toInt(), args[1].toInt(), args[2].toInt());
    else if (slotArgStr == m_acceptedSlots[6])
      emit invoke(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt());
    else if (slotArgStr == m_acceptedSlots[7])
      emit invoke(QColor(args[0]));
  }
    
  disconnect(this, invokeName.toAscii(), object, slotName.toAscii());
}

#include "invokeclass.moc"
