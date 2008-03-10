### Kommander Executor ###

########### next target ###############

include_directories (
  ${QDBUS_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${KDE4_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib
  ${CMAKE_CURRENT_SOURCE_DIR}/../widgets
)
SET(kommander_SRCS
    instance.cpp 
    main.cpp
)
QT4_ADD_DBUS_ADAPTOR(kommander_SRCS org.kdewebdev.kommander.xml instance.h Instance )

KDE4_ADD_EXECUTABLE(kommander ${kommander_SRCS})
link_directories (
  ${CMAKE_CURRENT_BINARY_DIR}/../lib/
  ${CMAKE_CURRENT_BINARY_DIR}/../widgets/
)
TARGET_LINK_LIBRARIES(
  kommander
  kommandercore
  kommanderwidgets
  ${QT_QTCORE_LIBS} 
  ${QT_QTUI_LIBS} 
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
)

INSTALL( TARGETS kommander DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

INSTALL_FILES( /share/applnk/.hidden FILES  kommander.desktop )

