//
// C++ Implementation: designerwidgets
//
// Description: 
//
//
// Author:  <eric@localhost>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "kmdrdesignerwidgets.h"

KmdrDesignerWidgets::KmdrDesignerWidgets(QObject *parent)
  : QObject(parent)
{
  widgets.append(new AboutDialog(this));
  widgets.append(new ButtonGroup(this));
  widgets.append(new CheckBox(this));
  widgets.append(new CloseButton(this));
  widgets.append(new ComboBox(this));
  widgets.append(new DatePicker(this));
  widgets.append(new Dialog(this));
  widgets.append(new ExecButton(this));
  widgets.append(new FileSelector(this));
  widgets.append(new FontDialog(this));
  widgets.append(new GroupBox(this));
  widgets.append(new Konsole(this));
  widgets.append(new Label(this));
  widgets.append(new LineEdit(this));
  widgets.append(new ListBox(this));
  widgets.append(new PixmapLabel(this));
  widgets.append(new PopupMenu(this));
  widgets.append(new ProgressBar(this));
  widgets.append(new Slider(this));
  widgets.append(new SpinBoxInt(this));
  widgets.append(new StatusBar(this));
  widgets.append(new Table(this));
  widgets.append(new TabWidget(this));
  widgets.append(new TextBrowser(this));
  widgets.append(new TextEdit(this));
  widgets.append(new Timer(this));
  widgets.append(new ToolBox(this));
  widgets.append(new TreeWidget(this));
  widgets.append(new Wizard(this));  
}

  QList<QDesignerCustomWidgetInterface*> KmdrDesignerWidgets::customWidgets() const
{
  return widgets;
}

  Q_EXPORT_PLUGIN2(customwidgetsplugin, KmdrDesignerWidgets)



