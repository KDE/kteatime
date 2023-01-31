add_definitions(-DTRANSLATION_DOMAIN=\"kteatime\")
add_executable(kteatime)

target_sources(kteatime PRIVATE settings.cpp timeedit.cpp toplevel.cpp tealistmodel.cpp tea.cpp main.cpp )

set_property(SOURCE main.cpp APPEND PROPERTY COMPILE_DEFINITIONS "KTEATIME_VERSION=\"${kteatime_VERSION}\"")

ki18n_wrap_ui(kteatime settings.ui timeedit.ui)

target_link_libraries(kteatime
    KF${KF_MAJOR_VERSION}::ConfigCore
    KF${KF_MAJOR_VERSION}::ConfigGui
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::Crash
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::IconThemes
    KF${KF_MAJOR_VERSION}::Notifications
    KF${KF_MAJOR_VERSION}::NotifyConfig
    KF${KF_MAJOR_VERSION}::TextWidgets
    KF${KF_MAJOR_VERSION}::XmlGui
)

install( TARGETS kteatime ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )
install( PROGRAMS org.kde.kteatime.desktop  DESTINATION ${KDE_INSTALL_APPDIR} )
install( FILES kteatime.notifyrc  DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR} )
