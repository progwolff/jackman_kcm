set(JACK_CONFIG_FILE         "jackman.conf"             CACHE PATH      "Path of the jack devices config file")
    
configure_file(config.h.in config.h IMMEDIATE @ONLY)
# add_subdirectory(configwidgets)

#include_directories(configwidgets)

set(JACK_KCM_SRCS
    jackmankcm.cpp
    devicesconfig.cpp
    behaviorconfig.cpp
    devicesmodel.cpp
    devicesmetadata.cpp
    #devicesdelegate.cpp
    latencymeasurebox.cpp
)

set(JACK_KCM_UI
    ui/devicesconfig.ui
    qml/main.qml
    qml/TooltipArea.qml
    ui/behaviorconfig.ui
    qml/behavior.qml)

ki18n_wrap_ui(JACK_KCM_SRCS ${JACK_KCM_UI})
add_library(kcm-jackman MODULE ${JACK_KCM_SRCS})
target_compile_definitions(kcm-jackman PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(kcm-jackman
    Qt5::Widgets
    Qt5::X11Extras
    Qt5::Quick
    Qt5::QuickWidgets
    KF5::I18n
    KF5::ConfigWidgets
    KF5::Auth
    KF5::KIOWidgets
    KF5::Declarative
    KF5::I18n
)


if (ALSA_FOUND)
  target_link_libraries(kcm-jackman ${ALSA_LIBRARY})
endif (ALSA_FOUND)

install(TARGETS kcm-jackman DESTINATION ${CMAKE_INSTALL_PLUGINDIR})

install(FILES qml/main.qml DESTINATION ${CMAKE_INSTALL_DATADIR}/jackman_kcm)
install(FILES qml/TooltipArea.qml DESTINATION ${CMAKE_INSTALL_DATADIR}/jackman_kcm)
install(FILES qml/behavior.qml DESTINATION ${CMAKE_INSTALL_DATADIR}/jackman_kcm)

kcoreaddons_desktop_to_json(kcm-jackman ../kcm-jackman.desktop)


