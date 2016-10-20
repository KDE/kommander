#### Kommander Project ###

project(kommander)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)
include (MacroOptionalAddSubdirectory)

add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)
add_definitions( ${QT_QTDBUS_DEFINITIONS} ${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

add_subdirectory(lib)
add_subdirectory(widgets)
add_subdirectory(executor)
macro_optional_add_subdirectory(doc)
