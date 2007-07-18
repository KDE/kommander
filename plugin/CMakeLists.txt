### Kommander Plugin ###

INCLUDE_DIRECTORIES( 
${CMAKE_SOURCE_DIR}/kommander/widget 
${CMAKE_CURRENT_SOURCE_DIR} 
${CMAKE_CURRENT_BINARY_DIR} 
${KDE4_INCLUDE_DIR} 
${QT_INCLUDE_DIR}  )

########### next target ###############

SET(kommanderplugin_LIB_SRCS
   kommanderplugin.cpp
   )

# kde4_automoc(${kommanderplugin_LIB_SRCS})

ADD_LIBRARY(kommanderplugin SHARED ${kommanderplugin_LIB_SRCS})

TARGET_LINK_LIBRARIES(kommanderplugin  ${QT_AND_KDECORE_LIBS} )

SET_TARGET_PROPERTIES(kommanderplugin PROPERTIES VERSION 4.2.0 SOVERSION 4 )
INSTALL(TARGETS kommanderplugin DESTINATION lib} )


########### install files ###############

INSTALL_FILES( /include FILES  kommanderplugin.h )



