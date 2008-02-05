%{H_TEMPLATE}
#ifndef %{APPNAMEUC}_WIDGET_H
#define %{APPNAMEUC}_WIDGET_H

#include <qwidget.h>
#include <qstring.h>

#include <kparts/part.h>

#include <kommanderwidget.h>

class QStringList;

//replace QWidget with the widget you want to derive from
class %{APPNAME}: public QWidget, public KommanderWidget
{
  Q_OBJECT

  Q_PROPERTY(QString populationText READ populationText WRITE setPopulationText DESIGNABLE false)
  Q_PROPERTY(QStringList associations READ associatedText WRITE setAssociatedText DESIGNABLE false)
  Q_PROPERTY(bool KommanderWidget READ isKommanderWidget)

public:  
  %{APPNAME}(QWidget *parent, const char* name);
  ~%{APPNAME}();

  virtual bool isKommanderWidget() const;
  virtual void setAssociatedText(const QStringList&);
  virtual QStringList associatedText() const;
  virtual QString currentState() const;
  virtual QString populationText() const;
  virtual void setPopulationText(const QString&);
  virtual void populate();

  virtual QString handleDCOP(int function, const QStringList& args);
  virtual bool isFunctionSupported(int function);

signals:
  void contextMenuRequested(int xpos, int ypos);

protected:
  void contextMenuEvent( QContextMenuEvent * e );
};


#endif
