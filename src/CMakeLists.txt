add_definitions(-DTRANSLATION_DOMAIN=\"kteatime\")

set(kteatime_SRCS settings.cpp timeedit.cpp toplevel.cpp tealistmodel.cpp tea.cpp main.cpp )

set_property(SOURCE main.cpp APPEND PROPERTY COMPILE_DEFINITIONS "KTEATIME_VERSION=\"${kteatime_VERSION}\"")

ki18n_wrap_ui(kteatime_SRCS settings.ui timeedit.ui)

add_executable(kteatime ${kteatime_SRCS})
target_link_libraries(kteatime
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::Crash
    KF5::I18n
    KF5::IconThemes
    KF5::Notifications
    KF5::NotifyConfig
    KF5::TextWidgets
    KF5::XmlGui
)

install( TARGETS kteatime ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )
install( PROGRAMS org.kde.kteatime.desktop  DESTINATION ${KDE_INSTALL_APPDIR} )
install( FILES kteatime.notifyrc  DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR} )
