add_definitions(-DTRANSLATION_DOMAIN=\"kteatime\")
add_executable(kteatime)

target_sources(kteatime PRIVATE settings.cpp timeedit.cpp toplevel.cpp tealistmodel.cpp tea.cpp main.cpp )

set_property(SOURCE main.cpp APPEND PROPERTY COMPILE_DEFINITIONS "KTEATIME_VERSION=\"${kteatime_VERSION}\"")

ki18n_wrap_ui(kteatime settings.ui timeedit.ui)

target_link_libraries(kteatime
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::GuiAddons
    KF6::Crash
    KF6::I18n
    KF6::IconThemes
    KF6::Notifications
    KF6::NotifyConfig
    KF6::TextWidgets
    KF6::XmlGui
)

install( TARGETS kteatime ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )
install( PROGRAMS org.kde.kteatime.desktop  DESTINATION ${KDE_INSTALL_APPDIR} )
install( FILES kteatime.notifyrc  DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR} )
