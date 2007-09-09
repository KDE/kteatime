project(kteatime)

set( kteatime_SRCS settings.cpp timeedit.cpp toplevel.cpp tealistmodel.cpp tea.cpp main.cpp )

kde4_add_ui_files( kteatime_SRCS settings.ui timeedit.ui )

kde4_add_executable( kteatime ${kteatime_SRCS} )

target_link_libraries( kteatime ${KDE4_KIO_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} )


########### install files ###############
install( TARGETS kteatime DESTINATION ${BIN_INSTALL_DIR} )
install( FILES kteatime.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES kteatime.notifyrc  DESTINATION ${DATA_INSTALL_DIR}/kteatime )

kde4_install_icons( ${DATA_INSTALL_DIR}/kteatime  )
