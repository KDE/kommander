//
// C++ Interface: KmdrMainWindow
//
// Description: 
//
//
// Author: Andras Mantia <amantia@kdewebdev.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KmdrMainWindow_H
#define KmdrMainWindow_H

#include <kmainwindow.h>

/**
	@author Andras Mantia <amantia@kdewebdev.org>
*/
class KmdrMainWindow : public KMainWindow
{
Q_OBJECT
public:
    KmdrMainWindow(QWidget *parent = 0, const char *name = 0, WFlags f = WType_TopLevel | WDestructiveClose);

    ~KmdrMainWindow();

protected:
    virtual bool queryClose();

signals:
    void initialize();
    void destroy();

};

#endif
