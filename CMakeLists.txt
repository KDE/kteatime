kde4_header()




########### next target ###############

set(kteatime_SRCS 
   main.cpp 
   toplevel.cpp 
   tealist.cpp 
   timeedit.cpp )

kde4_automoc(${kteatime_SRCS})

kde4_add_executable(kteatime ${kteatime_SRCS})

target_link_libraries(kteatime  ${KDE4_KIO_LIBS} )

install_targets(/bin kteatime )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES kteatime.desktop )
install_files( ${DATA_INSTALL_DIR}/kteatime/icons FILES  )
install_files( ${DATA_INSTALL_DIR}/kteatime FILES eventsrc )

kde4_install_icons( ${ICON_INSTALL_DIR} crystalsvg )

kde4_footer()



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
