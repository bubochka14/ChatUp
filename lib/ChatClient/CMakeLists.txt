find_package(Qt6 REQUIRED COMPONENTS Core Widgets Qml Quick WebSockets Network Concurrent QuickControls2) 

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(LIB_SOURCES
	"res.qrc"
	"wsclient.cpp" 
	"wsclient.h" 
	"authenticationmaster.h" 
	"messageconstructor.cpp"
	"startupwindow.h"
	"startupwindow.cpp"
	"messageconstructor.h"
	"authenticationmaster.cpp"
	"roommodel.h"
	"roommodel.cpp"
	"usersmodel.cpp"
	"usersmodel.h"
	"userinfo.h" 
	"userinfo.cpp" 
	"networkfactory.h"
	"networkfactory.cpp"
	"applicationsettings.h" 
	"applicationsettings.cpp"
	"callerchatcontrollers.h"
	"callerchatcontrollers.cpp"
    "servermethodcaller.h" 
	"servermethodcaller.cpp" 
	"callerauthentificationmaster.h"
	"callerauthentificationmaster.cpp"
	"messagemodel.h" 
	"messagemodel.cpp"
	"offlineservermethodcaller.h"
	"offlineservermethodcaller.cpp"
	"chatcontroller.h"
	"chatcontroller.cpp"
	"clientmethoddispatcher.h"
	"clientmethoddispatcher.cpp"
	"wsservermethodcaller.h"
	"chatclient_include.h"
	"wsservermethodcaller.cpp"
	)
set(LIB_NAME ChatClient)
QT_ADD_LIBRARY(${LIB_NAME} SHARED)
qt_add_qml_module(${LIB_NAME} 
    URI ChatClient
	NO_GENERATE_PLUGIN_SOURCE
    SOURCES ${LIB_SOURCES}
)
target_compile_definitions(${LIB_NAME} PRIVATE 	
	CHAT_CLIENT_LIB
)
target_include_directories(${LIB_NAME} PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR} 
)
include(GNUInstallDirs)
install(TARGETS ${LIB_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)   
target_link_libraries(${LIB_NAME}  PRIVATE 
	Qt6::Core 
	Qt6::Qml 
	Qt6::Quick 
	Qt6::WebSockets 
	Qt6::Network
	Qt6::Concurrent
	Qt6::QuickControls2
)
