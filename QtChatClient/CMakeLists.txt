find_package(Qt6 REQUIRED COMPONENTS Core  Qml Quick WebSockets Network Concurrent)
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
target_link_libraries(${APP_NAME}  PRIVATE 
	Qt6::Core 
	Qt6::Qml 
	Qt6::Quick 
	Qt6::WebSockets 
	Qt6::Network
	Qt6::Concurrent
	
)
target_compile_definitions(${APP_NAME} PRIVATE APP_NAME="${APP_NAME}" ORG_NAME="${ORG_NAME}" CLIENT_VERSION="${CMAKE_PROJECT_VERSION}")

target_include_directories(${APP_NAME} 
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
)
add_subdirectory(qml)
