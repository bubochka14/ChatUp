find_package(Qt6 REQUIRED COMPONENTS Core WebSockets Network Concurrent Multimedia)
find_package(LibDataChannel CONFIG REQUIRED)

set(LIB_NAME Network)
set(LIB_SOURCES

    "authenticationmaster.h"
    "authenticationmaster.cpp"
    "callerauthentificationmaster.cpp"
    "callerauthentificationmaster.h"
    "messageconstructor.cpp"
    "messageconstructor.h"
    "networkmanager.h"
    "networkmanager.cpp"
    "wsnetworkmanager.cpp"
    "wsnetworkmanager.cpp"
    "network_include.h"
    "serverhandler.h"
    "serverhandler.cpp"
    "transport.h"
    "wsclient.h"
    "wsclient.cpp"
    "api/authentication.h"
    "api/authentication.cpp"
    "api/group.h"
    "api/group.cpp"
    "api/message.h"
    "api/message.cpp"
    "api/user.h"
    "api/user.cpp"
    "api/call.cpp"
    "api/call.h"
    "controllers/callcontroller.h"
    "controllers/callcontroller.cpp"
    "controllers/controllermanager.h"
    "controllers/controllermanager.cpp"
    "controllers/groupcontroller.h"
    "controllers/groupcontroller.cpp"
    "controllers/messagecontroller.h"
    "controllers/messagecontroller.cpp"
    "controllers/usercontroller.h"
    "controllers/usercontroller.cpp"
    "controllers/abstractcontroller.h"
	)
QT_ADD_LIBRARY(${LIB_NAME} SHARED)
qt_add_qml_module(${LIB_NAME} 
    URI ${LIB_PREFIX}.Network
    NO_GENERATE_PLUGIN_SOURCE
    SOURCES ${LIB_SOURCES}
)
target_include_directories(${LIB_NAME} PRIVATE controllers)
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
    Media
	Qt6::Core 
	Qt6::WebSockets 
	Qt6::Network
	Qt6::Concurrent
	Qt6::Multimedia
    LibDataChannel::LibDataChannel

)
