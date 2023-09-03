find_package(Qt6 REQUIRED COMPONENTS Core  Qml Quick WebSockets Network Concurrent QuickControls2)
set(APP_NAME ${PROJECT_NAME})
add_executable (${APP_NAME} 
	"main.cpp" 
	"wsclient.cpp" 
	"wsclient.h" 
	"authenticationmaster.h" 
        "res.qrc"
	"messageconstructor.cpp"
	"messageconstructor.h"
	"authenticationmaster.cpp"
	"singletonholder.cpp" 
	"singletonholder.h"
	"chatroommodel.h"
	"chatroommodel.cpp"
	"chatroomstructs.h" 
	"chatroomstructs.cpp" 
	"chatclient.h" 
	"applicationsettings.h" 
	"applicationsettings.cpp"
	"chatclient.cpp"
	"userverifydialog.h"
	"userverifydialog.cpp"
 "servermethodcaller.h" "servermethodcaller.cpp" "chatwindow.h" "chatwindow.cpp" "quickwindow.h" "MessageHistoryModel.h" "MessageHistoryModel.cpp")
 file(REAL_PATH "../components" LIB_PATH BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND QML_IMPORT_PATHS "${LIB_PATH}")
set(QML_IMPORT_PATH "${QML_IMPORT_PATHS}" CACHE STRING "Qt Creator QML import paths")

target_link_libraries(${APP_NAME}  PRIVATE 
	Qt6::Core 
	Qt6::Qml 
	Qt6::Quick 
	Qt6::WebSockets 
	Qt6::Network
	Qt6::Concurrent
	Qt6::QuickControls2
	components
	
)
target_compile_definitions(${APP_NAME} PRIVATE APP_NAME="${APP_NAME}" ORG_NAME="${ORG_NAME}" CLIENT_VERSION="${CMAKE_PROJECT_VERSION}")

target_include_directories(${APP_NAME} 
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
)
add_subdirectory(components)
