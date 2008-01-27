project(kteatime)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
macro_optional_find_package(Plasma)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${KDE4_INCLUDES})

ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(src)
if(PLASMA_FOUND)
    ADD_SUBDIRECTORY(plasma)
endif(PLASMA_FOUND)
