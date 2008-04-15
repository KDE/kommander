### Kommander Widgets ###
add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

INCLUDE_DIRECTORIES( 
 	${CMAKE_CURRENT_SOURCE_DIR}/../lib
	${CMAKE_CURRENT_SOURCE_DIR} 
	${CMAKE_CURRENT_BINARY_DIR} 
	${KDE4_INCLUDE_DIR} 
	${QT_INCLUDE_DIR}
        ${QT_DESIGNER_LIBRARY}
)


########### next target ###############

SET(kommanderwidgets_LIB_SRCS
aboutdialog.cpp
fontdialog.cpp
buttongroup.cpp
checkbox.cpp
closebutton.cpp
combobox.cpp
datepicker.cpp
dialog.cpp
execbutton.cpp
fileselector.cpp
groupbox.cpp
konsole.cpp
label.cpp
lineedit.cpp
listbox.cpp
pixmaplabel.cpp
plugin.cpp
popupmenu.cpp
progressbar.cpp
radiobutton.cpp
richtexteditor.cpp
scriptobject.cpp
slider.cpp
spinboxint.cpp
statusbar.cpp
subdialog.cpp
table.cpp
tabwidget.cpp
textbrowser.cpp
textedit.cpp
timer.cpp
toolbox.cpp
treewidget.cpp
wizard.cpp
)

KDE4_ADD_LIBRARY(kommanderwidgets SHARED ${kommanderwidgets_LIB_SRCS})

########### next target ###############

SET(kmdrdesignerwidgets_LIB_SRCS
aboutdialogkmdrdesignerplugin.cpp
buttongroupkmdrdesignerplugin.cpp
checkboxkmdrdesignerplugin.cpp
closebuttonkmdrdesignerplugin.cpp
comboboxkmdrdesignerplugin.cpp
datepickerkmdrdesignerplugin.cpp
execbuttonkmdrdesignerplugin.cpp
fileselectorkmdrdesignerplugin.cpp
fontdialogkmdrdesignerplugin.cpp
groupboxkmdrdesignerplugin.cpp
kmdrdesignerwidgets.cpp
konsolekmdrdesignerplugin.cpp
labelkmdrdesignerplugin.cpp
lineeditkmdrdesignerplugin.cpp
listboxkmdrdesignerplugin.cpp
pixmaplabelkmdrdesignerplugin.cpp
popupmenukmdrdesignerplugin.cpp
progressbarkmdrdesignerplugin.cpp
radiobuttonkmdrdesignerplugin.cpp
scriptobjectkmdrdesignerplugin.cpp
sliderkmdrdesignerplugin.cpp
spinboxintkmdrdesignerplugin.cpp
statusbarkmdrdesignerplugin.cpp
tablekmdrdesignerplugin.cpp
tabwidgetkmdrdesignerplugin.cpp
textbrowserkmdrdesignerplugin.cpp
texteditkmdrdesignerplugin.cpp
timerkmdrdesignerplugin.cpp
toolboxkmdrdesignerplugin.cpp
treewidgetkmdrdesignerplugin.cpp
wizardkmdrdesignerplugin.cpp
)

#KDE4_ADD_LIBRARY(kmdrdesignerwidgets SHARED ${kmdrdesignerwidgets_LIB_SRCS})

TARGET_LINK_LIBRARIES(
 kommanderwidgets  
 kommandercore 
 ${KDE4_KDEUI_LIBS}
 ${KDE4_KDE3SUPPORT_LIBS}
 ${KDE4_KIO_LIBS} 
)

SET_TARGET_PROPERTIES(kommanderwidgets PROPERTIES VERSION 4.2.0 SOVERSION 4 )
INSTALL(TARGETS kommanderwidgets ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

