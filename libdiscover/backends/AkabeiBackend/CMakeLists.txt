include_directories(
${AKABEICORE_INCLUDE_DIR}
${AKABEICLIENT_INCLUDE_DIR}
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

add_library(akabei-backend ${akabeibackend_SRCS})
target_link_libraries(akabei-backend ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${qjson_LIBRARIES} ${AKABEICLIENT_LIBRARIES} ${AKABEICORE_LIBRARIES} Discover::Common)
install(TARGETS akabei-backend DESTINATION ${PLUGIN_INSTALL_DIR}/discover)
install(FILES akabei-backend.desktop DESTINATION ${DATA_INSTALL_DIR}/libdiscover/backends)
install(FILES akabei-backend-categories.xml DESTINATION ${DATA_INSTALL_DIR}/libdiscover/categories)

set(akabeinotifier_SRCS
    AkabeiNotifier.cpp
)

add_library(kded_muonakabeinotifier MODULE ${akabeinotifier_SRCS})

target_link_libraries(kded_muonakabeinotifier ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${AKABEICLIENT_LIBRARIES} ${AKABEICORE_LIBRARIES} Discover::Common)

install(TARGETS kded_muonakabeinotifier DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES muonakabeinotifier.desktop  DESTINATION  ${SERVICES_INSTALL_DIR}/kded)