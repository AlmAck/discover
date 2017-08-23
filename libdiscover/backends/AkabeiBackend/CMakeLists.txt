include_directories(
    ${AKABEICORE_INCLUDE_DIR}
    ${AKABEICLIENT_INCLUDE_DIR}
    #${QJSON-qt5_INCLUDE_DIR}
)


set(akabeibackend_SRCS
    AppstreamUtils.cpp
    AppAkabeiResource.cpp
    AkabeiUpdater.cpp 
    AkabeiQuestion.cpp
    AkabeiBackend.cpp
    AkabeiResource.cpp
    AkabeiTransaction.cpp
)

add_library(akabei-backend MODULE ${akabeibackend_SRCS})
target_link_libraries(akabei-backend Discover::Common
                                     Qt5::Core
                                     Qt5::Network
                                     KF5::KIOWidgets
                                     #${QJSON-qt5_LIBRARIES}
                                     ${AKABEICLIENT_LIBRARIES}
                                     ${AKABEICORE_LIBRARIES})

install(TARGETS akabei-backend DESTINATION ${PLUGIN_INSTALL_DIR}/discover)
install(FILES akabei-backend.desktop DESTINATION ${DATA_INSTALL_DIR}/libdiscover/backends)
install(FILES akabei-backend-categories.xml DESTINATION ${DATA_INSTALL_DIR}/libdiscover/categories)

#notifier
set(akabeinotifier_SRCS
    AkabeiNotifier.cpp
)

add_library(MuonPackageKitNotifier MODULE ${akabeinotifier_SRCS})
target_link_libraries(MuonPackageKitNotifier Discover::Notifiers
                                             Qt5::Network
                                             ${AKABEICLIENT_LIBRARIES}
                                             ${AKABEICORE_LIBRARIES})

install(TARGETS MuonPackageKitNotifier DESTINATION ${PLUGIN_INSTALL_DIR}/discover-notifier)
