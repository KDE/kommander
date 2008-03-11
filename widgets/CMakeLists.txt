### Kommander Widgets ###
add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

INCLUDE_DIRECTORIES( 
 	${CMAKE_CURRENT_SOURCE_DIR}/../lib
	${CMAKE_CURRENT_SOURCE_DIR} 
	${CMAKE_CURRENT_BINARY_DIR} 
	${KDE4_INCLUDE_DIR} 
	${QT_INCLUDE_DIR}
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

TARGET_LINK_LIBRARIES(
 kommanderwidgets  
 kommandercore 
 ${KDE4_KDEUI_LIBS}
 ${KDE4_KDE3SUPPORT_LIBS}
 ${KDE4_KIO_LIBS} 
)

SET_TARGET_PROPERTIES(kommanderwidgets PROPERTIES VERSION 4.2.0 SOVERSION 4 )
INSTALL(TARGETS kommanderwidgets DESTINATION ${LIB_INSTALL_DIR} )


########### install files ###############

