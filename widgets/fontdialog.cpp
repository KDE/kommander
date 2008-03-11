//
// C++ Implementation: FontDialog
//
// Description: 
//
//
// Author: Andras Mantia <amantia@kdewebdev.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "fontdialog.h"

#include "kommanderplugin.h"
#include "specials.h"

#include <kfontdialog.h>
#include <kiconloader.h>
#include <klocale.h>
//Added by qt3to4:
#include <QLabel>
#include <QFrame>

enum Functions {
  FirstFunction = 139,
  SetFont,
  Family,
  PointSize,
  Bold,
  Italic,
  LastFunction
};

FontDialog::FontDialog(QWidget *parent, const char *name)
 : QLabel(parent, name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  if (KommanderWidget::inEditor)
  {
    setPixmap(KIconLoader::global()->loadIcon("kfontcombo", KIconLoader::NoGroup, KIconLoader::SizeMedium));
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(1);
    setFixedSize(pixmap()->size());
  }
  else
    setHidden(true);
  KommanderPlugin::setDefaultGroup(Group::DBUS);
  KommanderPlugin::registerFunction(SetFont, "setFont(QString widget, QString family, int pointSize, bool bold, bool italic)",
         i18n("Sets the default font for the dialog, by specifying the family, the size and other style options."), 2, 5);
  KommanderPlugin::registerFunction(Family, "family(QString widget)",
         i18n("Returns the font family."), 1);
  KommanderPlugin::registerFunction(PointSize, "pointSize(QString widget)",
         i18n("Returns the font size in point."), 1);
  KommanderPlugin::registerFunction(Bold, "bold(QString widget)",
         i18n("Returns true, if the font is bold."), 1);
  KommanderPlugin::registerFunction(Italic, "italic(QString widget)",
         i18n("Returns true, if the font is italic."), 1);
}

FontDialog::~FontDialog()
{
}

QString FontDialog::currentState() const
{
  return QString("default");
}

bool FontDialog::isKommanderWidget() const
{
  return true;
}

QStringList FontDialog::associatedText() const
{
  return KommanderWidget::associatedText();
}

void FontDialog::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void FontDialog::setWidgetText(const QString& a_text)
{
  KommanderWidget::setAssociatedText(QStringList(a_text));
}

void FontDialog::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString FontDialog::populationText() const
{
  return KommanderWidget::populationText();
}

void FontDialog::populate()
{
  setAssociatedText(QStringList(KommanderWidget::evalAssociatedText( populationText())));
}

bool FontDialog::isFunctionSupported(int f)
{
  return (f > FirstFunction && f < LastFunction) || f == DBUS::execute;
}

QString FontDialog::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case SetFont:
    {
      m_font.setFamily(args[0]);
      if (args[1].isEmpty())
        m_font.setPointSize(12);
      else
        m_font.setPointSize(args[1].toInt());
      m_font.setBold(args[2] == "1" || args[2].upper() == "TRUE");
      m_font.setItalic(args[3] == "1" || args[3].upper() == "TRUE");
      break;
    }
    case Family:
    {
      return m_font.family();
      break;
    }
    case PointSize:
    {
      return QString::number(m_font.pointSize());
      break;
    }
    case Bold:
    {
      return m_font.bold() ? "1" : "0";
    }
    case Italic:
    {
      return m_font.italic() ? "1" : "0";
    }
    case DBUS::execute:
    {
      int result = KFontDialog::getFont( m_font );
      if ( result == KFontDialog::Accepted )
      {
        return m_font.toString();
      } 
      break;
    }
    default:
      return KommanderWidget::handleDBUS(function, args);
  }
  return QString();
}



#include "fontdialog.moc"
