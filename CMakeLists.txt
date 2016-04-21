cmake_minimum_required(VERSION 2.8.12)
project(kteatime)

find_package(ECM 1.3.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
include(ECMInstallIcons)

# Search KDE installation
find_package(Qt5 5.2.0 CONFIG REQUIRED
  Core
  Widgets
)

find_package(KF5 5.15.0 REQUIRED
  Config
  Crash
  DocTools
  I18n
  IconThemes
  NotifyConfig
  Notifications
  TextWidgets
  XmlGui
)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(src)

install(FILES org.kde.kteatime.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
