/**********************************************************************
 This file is based on Qt Designer, Copyright (C) 2000 Trolltech AS. �All rights reserved.

 This file may be distributed and/or modified under the terms of the
 GNU General Public License version 2 as published by the Free Software
 Foundation and appearing in the file LICENSE.GPL included in the
 packaging of this file.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

 See http://www.trolltech.com/gpl/ for GPL licensing information.

 Modified for Kommander:
  (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
  (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>

**********************************************************************/

#ifndef FORMFILE_H
#define FORMFILE_H

#include <qobject.h>
#include "timestamp.h"
#include "metadatabase.h"

class FormWindow;

class FormFile : public QObject
{
    Q_OBJECT

public:
    FormFile(const QString &fn, bool temp);
    ~FormFile();

  void setFormWindow( FormWindow *f );
  void setFileName( const QString &fn );
  void setModified(bool m);
  FormWindow *formWindow() const;
  QString fileName() const;
  QString absFileName() const;
  bool save( bool withMsgBox = TRUE );
  bool saveAs();
  bool close();
  bool closeEvent();
  bool isModified();
  void showFormWindow();
  static QString createUnnamedFileName();
  QString formName() const;
  bool hasTempFileName() const {return fileNameTemp;}

signals:
  void addedFormFile(FormFile*);
  void removedFormFile(FormFile*);
  void somethingChanged(FormFile*);

private slots:
    void formWindowChangedSomehow();
  void init();

private:
    bool isFormWindowModified() const;
    void setFormWindowModified( bool m );

private:
    QString filename;
    bool fileNameTemp;
    FormWindow *fw;
    QString cachedFormName;

};

#endif
