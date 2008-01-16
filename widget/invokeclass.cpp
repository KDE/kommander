#include "invokeclass.h"

InvokeClass::InvokeClass(QObject *parent):QObject(parent)
{
}

void InvokeClass::invokeSlot(QObject *object, const QString& slot)
{
  QString slotName = QString::number(QSLOT_CODE) + slot;  
  connect(this, SIGNAL(invoke()), object, slotName.ascii());
  emit invoke();
  disconnect(this, SIGNAL(invoke()), object, slotName.ascii()); 
}

#include "invokeclass.moc"
