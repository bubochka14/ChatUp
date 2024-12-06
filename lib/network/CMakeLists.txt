find_package(Qt6 REQUIRED COMPONENTS Core WebSockets Network Concurrent Multimedia)

set(LIB_NAME Network)
set(LIB_SOURCES

    "servermethodcaller.h"
    "servermethodcaller.cpp"
    "authenticationmaster.h"
    "callscontroller.h"
    "callscontroller.cpp"
    "authenticationmaster.cpp"
    "callerauthentificationmaster.cpp"
    "callerauthentificationmaster.h"
    "callerchatcontrollers.h"
    "callerchatcontrollers.cpp"
    "chatcontroller.cpp"
    "chatcontroller.h"
    "clientmethoddispatcher.h"
    "clientmethoddispatcher.cpp"
    "messageconstructor.cpp"
    "messageconstructor.h"
    "networkfactory.h"
    "networkfactory.cpp"
    "network_include.h"
    "serverhandler.h"
    "serverhandler.cpp"
    "wsclient.h"
    "wsclient.cpp"
    "wsservermethodcaller.h"
    "wsservermethodcaller.cpp"
	)
QT_ADD_LIBRARY(${LIB_NAME} SHARED)
qt_add_qml_module(${LIB_NAME} 
    URI ${LIB_PREFIX}.Network
    NO_GENERATE_PLUGIN_SOURCE
    SOURCES ${LIB_SOURCES}
)
add_library(${LIB_PREFIX}::${LIB_NAME} ALIAS ${LIB_NAME})

target_include_directories(${LIB_NAME} PUBLIC 
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_definitions(${LIB_NAME} PRIVATE 	
	CC_NETWORK_SHARED
)
include(GNUInstallDirs)
install(TARGETS ${LIB_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)   
target_link_libraries(${LIB_NAME}  PRIVATE 
	Core
	Qt6::Core 
	Qt6::WebSockets 
	Qt6::Network
	Qt6::Concurrent
	Qt6::Multimedia
)
