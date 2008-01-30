/***************************************************************************
    begin                : Wed Jan 30 2008
    copyright            : (C) 2008, Andras Mantia <amantia@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#ifndef KOMMANDERPART_H
#define KOMMANDERPART_H

#include <kparts/part.h>
#include <kparts/factory.h>

class Instance;
class QGridLayout;

class KommanderPart : public KParts::ReadOnlyPart
{
    Q_OBJECT
public:
    KommanderPart(QWidget *parentWidget, const char *widgetName,
                  QObject *parent, const char *name, const QStringList &args);

    virtual ~KommanderPart();

    static KAboutData* createAboutData();

protected:
    virtual bool openFile();

protected slots:
    void slotRun();

private:
    Instance *m_instance;
    QWidget *m_widget;
    QGridLayout *m_layout;
};


#endif // KOMMANDERPART_H
