%{CPP_TEMPLATE}
#include "%{APPNAMELC}.h"

#include <kommanderplugin.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include "%{APPNAMELC}.h"

class %{APPNAME}Plugin : public KommanderPlugin
{
public:
    %{APPNAME}Plugin();
    virtual QWidget *create( const QString &className, QWidget *parent = 0, const char *name = 0);
};

%{APPNAME}Plugin::%{APPNAME}Plugin()
{
    addWidget( "%{APPNAME}", "Custom", i18n("Kommander %{APPNAME} plugin."), new QIconSet(KGlobal::iconLoader()->loadIcon("%{APPNAMELC}", KIcon::NoGroup, KIcon::SizeMedium)) );
}

QWidget *%{APPNAME}Plugin::create( const QString &className, QWidget *parent, const char *name)
{
  if (className == "%{APPNAME}")
    return new %{APPNAME}(parent, name);
  return 0;
}

KOMMANDER_EXPORT_PLUGIN(%{APPNAME}Plugin)
