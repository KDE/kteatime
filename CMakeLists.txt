project(kteatime)



########### next target ###############

set(kteatime_SRCS 
   main.cpp 
   toplevel.cpp 
   tealist.cpp 
   timeedit.cpp )

kde4_automoc(${kteatime_SRCS})

kde4_add_executable(kteatime ${kteatime_SRCS})

target_link_libraries(kteatime  ${KDE4_KIO_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} )

install(TARGETS kteatime  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES kteatime.desktop  DESTINATION ${XDG_APPS_DIR})
install( FILES kteatime.notifyrc  DESTINATION ${DATA_INSTALL_DIR}/kteatime)

kde4_install_icons( ${ICON_INSTALL_DIR}  )


