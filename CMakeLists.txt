project(kteatime)



########### next target ###############

set(kteatime_SRCS 
   main.cpp 
   toplevel.cpp 
   tealist.cpp 
   timeedit.cpp )

kde4_automoc(kteatime ${kteatime_SRCS})

kde4_add_executable(kteatime ${kteatime_SRCS})

target_link_libraries(kteatime  ${KDE4_KIO_LIBS} knotifyconfig )

install(TARGETS kteatime  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES kteatime.desktop  DESTINATION ${XDG_APPS_DIR})
install( FILES kteatime.notifyrc  DESTINATION ${DATA_INSTALL_DIR}/kteatime)

kde4_install_icons( ${ICON_INSTALL_DIR}  )
kde4_install_icons( ${ICON_INSTALL_DIR}  )



#original Makefile.am contents follow:

#
#INCLUDES= $(all_includes)
#
#bin_PROGRAMS = kteatime
#
#kteatime_SOURCES = main.cpp toplevel.cpp tealist.cpp timeedit.cpp
#
#kteatime_METASOURCES = AUTO
#kteatime_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
## LIB_KIO needed for KNotifyDialog
#kteatime_LDADD = $(LIB_KDEUI) $(LIB_KIO)
#
#KDE_ICON = kteatime
#
#KDE_OPTIONS = nofinal
#
#appsdata_DATA = eventsrc
#appsdatadir = $(kde_datadir)/kteatime
#
#kteatimeicondir = $(kde_datadir)/kteatime/icons
#kteatimeicon_ICON = AUTO
#
#messages:
#	$(XGETTEXT) $(kteatime_SOURCES) -o $(podir)/kteatime.pot
#
#xdg_apps_DATA = kteatime.desktop
