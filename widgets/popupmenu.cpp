//
// C++ Implementation: popupmenu
//
// Description: 
//
//
// Author: Andras Mantia <amantia@kdewebdev.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "popupmenu.h"
#include "specials.h"

#include <qcursor.h>

#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kommanderplugin.h>

#define INSERTMENUITEM 100
#define INSERTSEPARATOR 101
#define CHANGEMENUITEM 102
#define SETITEMENABLED 103
#define ITEMENABLED 104
#define SETITEMVISIBLE 105
#define ITEMVISIBLE 106
#define INSERTSUBMENU 107
#define LAST_FUNCTION INSERTSUBMENU 

PopupMenu::PopupMenu(QWidget *parent, const char *name)
 : QLabel(parent, name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  if (KommanderWidget::inEditor)
  {
    setPixmap(KGlobal::iconLoader()->loadIcon("contents", KIcon::NoGroup, KIcon::SizeMedium));
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(1);
    setFixedSize(pixmap()->size());
  }
  else
    setHidden(true);

  m_menu = new KPopupMenu(this);
  connect(m_menu, SIGNAL(activated(int)), this, SLOT(slotMenuItemActivated(int)));

  KommanderPlugin::setDefaultGroup(Group::DCOP);
  KommanderPlugin::registerFunction(INSERTMENUITEM, "insertMenuItem(QString widget, QString text, QString executeWidget, int index)",  i18n("Insert an item into the popup menu. The executeWidget's execute method will be run when this item is selected. Returns the id of the inserted item."), 3);
  KommanderPlugin::registerFunction(INSERTSEPARATOR, "insertSeparator(QString widget, int index)",  i18n("Insert a separator item into the popup menu."), 1);
  KommanderPlugin::registerFunction(CHANGEMENUITEM, "changeMenuItem(QString widget, int id, QString text, QString executeWidget)",  i18n("Change an item specified by id in the popup menu. The executeWidget's execute method will be run when this item is selected."), 3);
  KommanderPlugin::registerFunction(SETITEMENABLED, "setItemEnabled(QString widget, int id, bool enable)",  i18n("Enable the item specified by id in the popup menu."), 2);
  KommanderPlugin::registerFunction(ITEMENABLED, "itemEnabled(QString widget, int id)",  i18n("Check if the item specified by id is enabled."), 1);
  KommanderPlugin::registerFunction(SETITEMVISIBLE, "setItemVisible(QString widget, int id, bool enable)",  i18n("Make the item specified by id visible."), 2);
  KommanderPlugin::registerFunction(ITEMVISIBLE, "itemVisible(QString widget, int id)",  i18n("Check if the item specified by id is visible."), 1);
  KommanderPlugin::registerFunction(INSERTSUBMENU, "insertSubmenu(QString widget, QString text, QString menuWidget, int index)",  i18n("Insert submenu widget into the popup menu."), 3);
}


PopupMenu::~PopupMenu()
{
  delete m_menu;
}

QString PopupMenu::currentState() const
{
  return QString("default");
}

bool PopupMenu::isKommanderWidget() const
{
  return true;
}

QStringList PopupMenu::associatedText() const
{
  return KommanderWidget::associatedText();
}

void PopupMenu::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void PopupMenu::setWidgetText(const QString& a_text)
{
  KommanderWidget::setAssociatedText(a_text);
}

void PopupMenu::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString PopupMenu::populationText() const
{
  return KommanderWidget::populationText();
}

void PopupMenu::popup(int x, int y)
{
  m_menu->exec(QPoint(x, y));
}

void PopupMenu::slotMenuItemActivated(int id)
{
  QString widget = m_associations[id];
  KommanderWidget::evalAssociatedText(QString("#!kommander\n%1.execute(%2)").arg(widget).arg(id)); 
}

void PopupMenu::populate()
{
  setAssociatedText(KommanderWidget::evalAssociatedText( populationText()));
}

QString PopupMenu::insertSubmenu(const QString& title, const QString &menuWidget, int index)
{
  KommanderWidget *w = widgetByName(menuWidget);
  if (dynamic_cast<PopupMenu*>(w))
  {
    return QString::number(m_menu->insertItem(title, dynamic_cast<PopupMenu*>(w)->menu(), index));
  }
  return QString();
}


bool PopupMenu::isFunctionSupported(int f)
{
  return f == DCOP::clear || f == DCOP::execute || f == DCOP::item || (f >= INSERTMENUITEM && f <= LAST_FUNCTION) || f == DCOP::count;
}

QString PopupMenu::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::clear:
      m_menu->clear();
      m_associations.clear();
      break;
    case DCOP::execute:
      m_params = args;
      evalAssociatedText();
      break;
    case INSERTMENUITEM:
    {
      uint index = args[2].toInt();
      int id = m_menu->insertItem(args[0], index);
      m_associations[id] = args[1];
      return QString::number(id);
      break;
    }
    case INSERTSUBMENU:
    {
      return insertSubmenu(args[0], args[1], args[2].toInt());
      break;
    }
    case CHANGEMENUITEM:
    {
      uint id = args[0].toInt();
      m_menu->changeItem(id, args[1]);
      m_associations[id] = args[2];
      break;
    }
    case INSERTSEPARATOR:
    {
      uint index = args[0].toInt();
      m_menu->insertSeparator(index);     
    }
    case SETITEMENABLED:
    {
      uint id = args[0].toInt();
      m_menu->setItemEnabled(id, args[1] == "true" || args[1] == "1" ? true : false);
      break;
    }
    case ITEMENABLED:
    {
      uint id = args[0].toInt();
      return m_menu->isItemEnabled(id) ? "1" : "0";
      break;
    }
    case SETITEMVISIBLE:
    {
      uint id = args[0].toInt();
      m_menu->setItemVisible(id, args[1] == "true" || args[1] == "1" ? true : false);
      break;
    }
    case ITEMVISIBLE:
    {
      uint id = args[0].toInt();
      return m_menu->isItemVisible(id) ? "1" : "0";
      break;
    }
    case DCOP::item:
    {
      uint index = args[0].toInt();
      return index < m_params.count() ? m_params[index] : QString::null;
    }
    case DCOP::count:
      return QString::number(m_menu->count());
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString();
}



#include "popupmenu.moc"
