//
// C++ Implementation: colordialog
//
// Description: 
//
//
// Author: Andras Mantia <amantia@kdewebdev.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "colordialog.h"

#include "kommanderplugin.h"
#include "specials.h"

#include <kcolordialog.h>
#include <kiconloader.h>
#include <klocale.h>

enum Functions {
  FirstFunction = 139,
  GetColor,
  LastFunction
};

ColorDialog::ColorDialog(QWidget *parent, const char *name)
 : QLabel(parent, name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  if (KommanderWidget::inEditor)
  {
    setPixmap(KGlobal::iconLoader()->loadIcon("khsselector", KIcon::NoGroup, KIcon::SizeMedium));
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(1);
    setFixedSize(pixmap()->size());
  }
  else
    setHidden(true);
  KommanderPlugin::setDefaultGroup(Group::DCOP);
  KommanderPlugin::registerFunction(GetColor, "getColor(QString widget, QString defaultColor)",
         i18n("Show a color picker dialog, defaulting to the defaultColor in #RRGGBB format. If the dialog is accepted, the selected color is returned in #RRGGBB format. If it is not accepted, an empty string is returned."), 2);
}

ColorDialog::~ColorDialog()
{
}

QString ColorDialog::currentState() const
{
  return QString("default");
}

bool ColorDialog::isKommanderWidget() const
{
  return true;
}

QStringList ColorDialog::associatedText() const
{
  return KommanderWidget::associatedText();
}

void ColorDialog::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void ColorDialog::setWidgetText(const QString& a_text)
{
  KommanderWidget::setAssociatedText(a_text);
}

void ColorDialog::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString ColorDialog::populationText() const
{
  return KommanderWidget::populationText();
}

void ColorDialog::populate()
{
  setAssociatedText(KommanderWidget::evalAssociatedText( populationText()));
}

bool ColorDialog::isFunctionSupported(int f)
{
  return (f > FirstFunction && f < LastFunction);
}

QString ColorDialog::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case GetColor:
    {
      QColor color;
      int result = KColorDialog::getColor(color, args[0]);
      if (result == KColorDialog::Accepted )
      {
        return color.name();
      }      
      break;
    }
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}



#include "colordialog.moc"
