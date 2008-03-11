### Kommander Pluginmanager ### 
add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/kommander/factory 
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_BINARY_DIR}
  ${KDE4_INCLUDE_DIR} 
  ${QT_INCLUDE_DIR})


########### next target ###############

SET(kmdr-plugins_SRCS
      main.cpp 
      mainwindow.cpp 
      pluginmanager.cpp
   )

# KDE4_AUTOMOC(${kmdr-plugins_SRCS})


ADD_EXECUTABLE(kmdr-plugins ${kmdr-plugins_SRCS})

TARGET_LINK_LIBRARIES(
  kmdr-plugins  
  ${KDE4_KDEUI_LIBS}
)

INSTALL( TARGETS kmdr-plugins DESTINATION bin)


########### install files ###############

INSTALL_FILES( /share/applnk/Applications FILES  )



#original Makefile.am contents follow:

#bin_PROGRAMS = kmdr-plugins
#
#kmdr_plugins_SOURCES = main.cpp mainwindow.cpp pluginmanager.cpp
#kmdr_plugins_LDADD   = $(top_builddir)/kommander/factory/libkommanderfactory.la  $(LIB_KIO)
#
##KDE_ICON = kmdr-plugins
#
#appsdir = $(kde_appsdir)/Applications
##apps_DATA = kmdr-plugins.desktop
#
#INCLUDES = -I$(top_srcdir)/kommander/factory $(all_includes)
#
#METASOURCES = AUTO
#
## the library search path.
#kmdr_plugins_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
#noinst_HEADERS = mainwindow.h pluginmanager.h
