### Kommander Executor ###

########### next target ###############

include_directories (
  ${QDBUS_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${KDE4_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../widget
  ${CMAKE_CURRENT_SOURCE_DIR}/../factory
  ${CMAKE_CURRENT_SOURCE_DIR}/../widgets
)
SET(kmdr-executor_SRCS
    instance.cpp 
    main.cpp
)

ADD_EXECUTABLE(kmdr-executor ${kmdr-executor_SRCS})

TARGET_LINK_LIBRARIES(
  kmdr-executor  
  ${QT_QTCORE_LIBS} 
  ${QT_QTUI_LIBS} 
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
)

INSTALL( TARGETS kmdr-executor DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

INSTALL_FILES( /share/applnk/.hidden FILES  kmdr-executor.desktop )



#original Makefile.am contents follow:

#bin_PROGRAMS = kmdr-executor
#
#kmdr_executor_SOURCES = instance.cpp main.cpp dcopkommanderif.skel
#kmdr_executor_LDADD   = $(top_builddir)/kommander/widget/libkommanderwidget.la $(top_builddir)/kommander/widgets/libkommanderwidgets.la $(top_builddir)/kommander/factory/libkommanderfactory.la  $(LIB_KIO)
#
##KDE_ICON = kmdr_executor
#
#appsdir = $(kde_appsdir)/.hidden
#apps_DATA = kmdr-executor.desktop
#
#AM_CPPFLAGS = -I$(top_srcdir)/kommander/widgets -I$(top_srcdir)/kommander/widget -I$(top_srcdir)/kommander/factory $(all_includes)
#
#
#METASOURCES = AUTO
#
## the library search path.
#kmdr_executor_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
#noinst_HEADERS =  dcopkommanderif.h
