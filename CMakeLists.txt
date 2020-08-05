cmake_minimum_required(VERSION 3.5)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "20")
set (RELEASE_SERVICE_VERSION_MINOR "08")
set (RELEASE_SERVICE_VERSION_MICRO "0")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kteatime VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "5.9.0")
set(KF5_MIN_VERSION "5.46.0")


find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
include(ECMInstallIcons)

# Search KDE installation
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED
  Core
  Widgets
)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED
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
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

if (KF5Config_VERSION VERSION_GREATER "5.56.0")
        add_definitions(-DQT_NO_FOREACH)
        MESSAGE(STATUS "compile without foreach")
endif()

ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(src)

install(FILES org.kde.kteatime.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
