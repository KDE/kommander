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

#ifndef INVOKECLASS_H
#define INVOKECLASS_H

#include <qobject.h>

class InvokeClass : public QObject {
Q_OBJECT
public:
  InvokeClass(QObject *parent);
  void invokeSlot(QObject *object,  const QString& slot);

signals:
  void  invoke();
};

#endif
