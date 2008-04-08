### Kommander Widget ###

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} 
${CMAKE_CURRENT_BINARY_DIR} 
${KDE4_INCLUDE_DIR} 
${QT_INCLUDE_DIR}  )


########### next target ###############

SET(kommandercore_LIB_SRCS
domtool.cpp
expression.cpp
function.cpp
functionlib.cpp
invokeclass.cpp
kmdrmainwindow.cpp
kommanderfactory.cpp
kommanderfunctions.cpp
kommanderplugin.cpp
kommanderwidget.cpp
kommanderwindow.cpp
myprocess.cpp
parsenode.cpp
parser.cpp
parserdata.cpp
specialinformation.cpp
)

KDE4_ADD_LIBRARY(kommandercore SHARED ${kommandercore_LIB_SRCS}) 
TARGET_LINK_LIBRARIES(kommandercore  ${KDE4_KIO_LIBS} ${KDE4_KDE3SUPPORT_LIBS} )

SET_TARGET_PROPERTIES(kommandercore PROPERTIES VERSION 4.2.0 SOVERSION 4 )
INSTALL(TARGETS kommandercore DESTINATION ${LIB_INSTALL_DIR} )


########### install files ###############

INSTALL(FILES kommanderwidget.h kommandercore_export.h kommanderplugin.h specials.h DESTINATION ${INCLUDE_INSTALL_DIR})

