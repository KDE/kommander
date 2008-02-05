%{CPP_TEMPLATE}
#include "%{APPNAMELC}.h"

#include <kommanderplugin.h>
#include <specials.h>

#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>


enum Functions {
  FirstFunction = 11001, //CHANGE THIS NUMBE TO AN UNIQUE ONE!!!
  Function1,
  Function2,
  LastFunction
};

%{APPNAME}::%{APPNAME}(QWidget *parent, const char *name)
  : QWidget(parent, name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);

//enable the below code to show a different widget in editor
/*
  if (KommanderWidget::inEditor)
  {
    setPixmap(KGlobal::iconLoader()->loadIcon("%{APPNAMELC}", KIcon::NoGroup, KIcon::SizeMedium));
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(1);
    setAlignment(Qt::AlignCenter);
  }
  else
    setHidden(true);
*/

  KommanderPlugin::setDefaultGroup(Group::DCOP);

//CHANGE THE BELOW LINES TO MATCH YOUR FUNCTIONS NAMES AND SIGNATURE
  KommanderPlugin::registerFunction(Function1, "function1(QString widget, QString arg1, int arg2)",  i18n("Call function1 with two arguments, second is optional."), 2, 3);
  KommanderPlugin::registerFunction(Function2, "function2(QString widget)",  i18n("Get a QString as a result of function2."), 1);  

}

%{APPNAME}::~%{APPNAME}()
{
}

QString %{APPNAME}::currentState() const
{
  return QString("default");
}

bool %{APPNAME}::isKommanderWidget() const
{
  return true;
}

QStringList %{APPNAME}::associatedText() const
{
  return KommanderWidget::associatedText();
}

void %{APPNAME}::setAssociatedText(const QStringList& a_atext)
{
  KommanderWidget::setAssociatedText(a_atext);
}

void %{APPNAME}::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString %{APPNAME}::populationText() const
{
  return KommanderWidget::populationText();
}

void %{APPNAME}::populate()
{
  KommanderWidget::evalAssociatedText(populationText());
}

void %{APPNAME}::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}


bool %{APPNAME}::isFunctionSupported(int f)
{
  return (f >= FirstFunction && f <= LastFunction); //see specials.h for other DCOP functions you might want to support
}

QString %{APPNAME}::handleDCOP(int function, const QStringList& args)
{
  switch (function)
  {
    case Function1:
      //do something for Function1, like handleFunction1(arg[0], arg[1].toInt());
      break;
    case Function2:
      //do something for Function2, like return handleFunction2();
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString::null;
}

#include "%{APPNAMELC}.moc"
