find_package(Qt6 REQUIRED COMPONENTS Core  Qml Quick WebSockets Network Concurrent QuickControls2) 

set(REL_QML_SOURCES
    "qml/AuthorizationPage.qml"
    "qml/ChatBox.qml"
    "qml/ChatInput.qml"
    "qml/ChatWindow.qml"
    "qml/EmojiList.qml"	
    "qml/RoomList.qml"
    "qml/RoundedFrame.qml"
    "qml/StartupWindow.qml"
    "qml/UserList.qml"
    "qml/LoadingPage.qml"
    "qml/MessageDelegate.qml"
)
foreach(file IN LISTS REL_QML_SOURCES)
    get_filename_component(QML_FILENAME ${file}	NAME)
    set_source_files_properties(${file} PROPERTIES
            QT_RESOURCE_ALIAS ${QML_FILENAME}
    )

    list(APPEND QML_SOURCES ${file})
endforeach()
set(LIB_NAME "ChatClient")

set(LIB_RESOURCES 
	"resources/pics/clear.png"
	"resources/pics/emoji.svg"
	"resources/pics/file.png"
    "resources/gif/duck.gif"
    "resources/pics/read.svg"
    "resources/pics/error.svg"
    "resources/pics/sent.svg"
    "qtquickcontrols2.conf"

    )
set(LIB_SOURCES
	"../quickfuture/src/qffuture.h"
	"../quickfuture/src/qffuture.cpp"
	"../quickfuture/src/quickfuture.h"
	"../quickfuture/src/qfvariantwrapper.h"
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
	"applicationfactory.h"
	"applicationfactory.cpp"
	"qmlchatwindow.h"
	"qmlchatwindow.cpp"
	"applicationsettings.h" 
	"applicationsettings.cpp"
    "servermethodcaller.h" 
	"servermethodcaller.cpp" 
	"abstractchatwindow.h" 
	"abstractchatwindow.cpp" 
	"callerauthentificationmaster.h"
	"callerauthentificationmaster.cpp"
	"messagemodel.h" 
	"messagemodel.cpp"
	"offlineservermethodcaller.h"
	"offlineservermethodcaller.cpp"
	"qmlstartupwindow.cpp"
	"abstractwindowfactory.cpp"
	"qmlstartupwindow.h"
	"abstractwindowfactory.h"
	"qmlwindowfactory.cpp"
	"qmlwindowfactory.h"
	"chatcontroller.h"
	"chatcontroller.cpp"
	"clientmethoddispatcher.h"
	"clientmethoddispatcher.cpp"
	"wsservermethodcaller.h"
	"chatclient_include.h"
	"wsservermethodcaller.cpp"
	)

qt_add_qml_module(${LIB_NAME} STATIC
    URI ChatClient
	PLUGIN_TARGET ${LIB_NAME}
    SOURCES ${LIB_SOURCES}
    RESOURCES ${LIB_RESOURCES}
	QML_FILES ${QML_SOURCES}
	RESOURCES ${LIB_RESOURCES}


)
target_compile_definitions(${LIB_NAME} PRIVATE 	
	CHAT_CLIENT_LIB
)
if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
        target_compile_options(${LIB_NAME} PUBLIC "/Zc:__cplusplus")
endif()
target_include_directories(${LIB_NAME} PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR} 
	../quickfuture/src
)
include(GNUInstallDirs)
install(TARGETS ${LIB_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
set(LIB_SOURCE ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
target_link_libraries(${LIB_NAME}  PRIVATE 
	Qt6::Core 
	Qt6::Qml 
	Qt6::Quick 
	Qt6::WebSockets 
	Qt6::Network
	Qt6::Concurrent
	Qt6::QuickControls2
)
