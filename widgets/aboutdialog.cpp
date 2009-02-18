//
// C++ Implementation: aboutdialog
//
// Description: 
//
//
// Author: Andras Mantia <amantia@kdewebdev.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <qfile.h>
#include "aboutdialog.h"

#include "kommanderplugin.h"
#include "specials.h"

#include <kaboutdata.h>
#include <kaboutapplication.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include <qtextstream.h>

enum Functions {
  FirstFunction = 159,
  Initialize,
  AddAuthor,
  AddTranslator,
  SetDescription,
  SetHomepage,
  SetBugAddress,
  SetLicense,
  Version,
  LastFunction
};

AboutDialog::AboutDialog(QWidget *parent, const char *name)
 : QLabel(parent, name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  if (KommanderWidget::inEditor)
  {
    setPixmap(KGlobal::iconLoader()->loadIcon("kommander", KIcon::NoGroup, KIcon::SizeMedium));
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(1);
    setFixedSize(pixmap()->size());
  }
  else
    setHidden(true);

  m_aboutData = 0L; 
  KommanderPlugin::setDefaultGroup(Group::DCOP);
  KommanderPlugin::registerFunction(Initialize, "initialize(QString widget, QString appName, QString icon, QString version, QString copyright)",
         i18n("Sets information about the application. This is the first method that must me called, any addition to the dialog done before initialization will be ignored."), 5);
  KommanderPlugin::registerFunction(AddAuthor, "addAuthor(QString widget, QString author, QString task, QString email, QString webAddress)",
         i18n("Add an author. Only the author name is required."), 2, 5);
   KommanderPlugin::registerFunction(AddTranslator, "addTranslator(QString widget, QString author, QString email)",
         i18n("Add a translator. Only the name is required."), 2, 3);
   KommanderPlugin::registerFunction(SetDescription, "setDescription(QString widget, QString description)",
         i18n("Set a short description text."), 2);
   KommanderPlugin::registerFunction(SetHomepage, "setHomepage(QString widget, QString homepage)",
         i18n("Set a homepage address."), 2);
   KommanderPlugin::registerFunction(SetBugAddress, "setBugAddress(QString widget, QString address)",
         i18n("Set an email address, where bugs can be reported."), 2);
   KommanderPlugin::registerFunction(SetLicense, "setLicense(QString widget, QString license)",
         i18n("Sets license information of the application. The parameter can be one of the license keys - GPL_V2, LGPL_V2, BSD, ARTISTIC -, or a freely specified text."), 2);
   KommanderPlugin::registerFunction(Version, "version(QString widget)",
         i18n("Returns the set version string."), 1);
}


AboutDialog::~AboutDialog()
{
  delete m_aboutData;
  m_aboutData = 0L;
}

QString AboutDialog::currentState() const
{
  return QString("default");
}

bool AboutDialog::isKommanderWidget() const
{
  return true;
}
QStringList AboutDialog::associatedText() const
{
  return KommanderWidget::associatedText();
}

void AboutDialog::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}


bool AboutDialog::isFunctionSupported(int f)
{
  return (f > FirstFunction && f < LastFunction) || f == DCOP::execute;
}

void AboutDialog::initialize(const QString& appName, const QString &icon, const QString& version, const QString& copyright)
{
  delete m_aboutData;  
  m_authors.clear();
  m_emails.clear();
  m_tasks.clear();
  m_addresses.clear();
  m_description = "";
  m_homepage = "";
  m_bugaddress = "";
  m_version = version;
  m_appName = appName;
  m_icon = icon;
  m_copyright = copyright;
  m_aboutData = new KAboutData(m_appName, m_appName, m_version);
  m_aboutData->setCopyrightStatement(m_copyright);
  if (!m_icon.isEmpty())
    m_aboutData->setProgramLogo(KGlobal::iconLoader()->loadIcon(m_icon, KIcon::NoGroup, KIcon::SizeMedium).convertToImage());  
}

void AboutDialog::addAuthor(const QString& author, const QString &task, const QString& email, const QString &webAddress)
{
  if (!m_aboutData)
    return;
  m_authors.append(author);
  m_emails.append(email);
  m_tasks.append(task);
  m_addresses.append(webAddress);
  m_aboutData->addAuthor(author, task, email, webAddress);
}

void AboutDialog::addTranslator(const QString& author, const QString& email)
{
  if (!m_aboutData)
    return;
  m_authors.append(author);
  m_emails.append(email);
  m_aboutData->setTranslator(author, email);
}

void AboutDialog::setDescription(const QString& description)
{
  if (!m_aboutData)
    return;
  m_description = description;
  m_aboutData->setShortDescription(m_description);
}

void AboutDialog::setHomepage(const QString &homepage)
{
  if (!m_aboutData)
    return;
  m_homepage = homepage;
  m_aboutData->setHomepage(m_homepage);
}

void AboutDialog::setBugAddress(const QString &bugAddress)
{
  if (!m_aboutData)
    return;
  m_bugaddress = bugAddress;
  m_aboutData->setBugAddress(m_bugaddress);
}

void AboutDialog::setLicense(const QString &key)
{
  if (!m_aboutData)
    return;
  QString license = key.upper();
  QString file;
  if (key == "GPL_V2")
  {
    file = locate("data", "LICENSES/GPL_V2");
  } else
  if (key == "LGPL_V2")
  {
    file = locate("data", "LICENSES/LGPL_V2");    
  } else
  if (key == "BSD")
  {
    file = locate("data", "LICENSES/BSD");
  } else
  if (key == "ARTISTIC")
  {
    file = locate("data", "LICENSES/ARTISTIC");
  }
  if (file.isEmpty() && !QFile::exists( key ))
  {
    if (!key.isEmpty())
      m_license = key;
      m_aboutData->setLicenseText(m_license);
  }
  else
  {
    if (file.isEmpty())
    {
      if (!key.isEmpty())
        file = key;
    }
    m_aboutData->setLicenseTextFile(file);
  } 
  
}

void AboutDialog::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString AboutDialog::populationText() const
{
  return KommanderWidget::populationText();
}

void AboutDialog::populate()
{
  setAssociatedText(KommanderWidget::evalAssociatedText( populationText()));
}

void AboutDialog::execute()
{
  if (m_aboutData)
  {        
    KAboutApplication dialog(m_aboutData, this);
    dialog.exec();
  }
}

QString AboutDialog::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case Initialize:
    {
      initialize(args[0], args[1], args[2], args[3]);
      break;
    }
    case SetLicense:
    {
      setLicense(args[0]);
      break;
    }
    case AddAuthor:
    {
      addAuthor(args[0], args[1], args[2], args[3]);
      break;
    }
    case AddTranslator:
    {
      addTranslator(args[0], args[1]);
      break;
    }
    case SetDescription:
    {
      setDescription(args[0]);
      break;
    }
    case SetHomepage:
    {
      setHomepage(args[0]);
      break;
    }
    case SetBugAddress:
    {
      setBugAddress(args[0]);
      break;
    }
    case Version:
    {
      if (m_aboutData)
        return m_aboutData->version();
      break;
    }
    case DCOP::execute:
    {
      if (m_aboutData)
      {        
        KAboutApplication dialog(m_aboutData, this);
        dialog.exec();
      }
      break;
    }
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}

#include "aboutdialog.moc"
