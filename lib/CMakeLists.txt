find_package(Qt6 REQUIRED COMPONENTS Core  Qml Quick WebSockets Network Concurrent) 
set(LIB_RESOURCES 
)
set(LIB_SOURCES
	"wsclient.cpp" 
	"wsclient.h" 
	"authenticationmaster.h" 
	"messageconstructor.cpp"
	"messageconstructor.h"
	"authenticationmaster.cpp"
	"chatroommodel.h"
	"chatroommodel.cpp"
	"chatroomstructs.h" 
	"chatroomstructs.cpp" 
	"applicationsettings.h" 
	"applicationsettings.cpp"
	"userverifydialog.h"
	"userverifydialog.cpp"
    "servermethodcaller.h" 
	"servermethodcaller.cpp" 
	"chatwindow.h" 
	"chatwindow.cpp" 
	"MessageHistoryModel.h" 
	"MessageHistoryModel.cpp"
)

qt_add_qml_module(ChatClient STATIC
    URI ChatClient
    VERSION 1.0
    SOURCES ${LIB_SOURCES}
    RESOURCES ${LIB_RESOURCES}

)
if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
        target_compile_options(ChatClient PUBLIC "/Zc:__cplusplus")
endif()
target_include_directories(ChatClient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(ChatClient PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.lightManager
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ChatClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
set(LIB_SOURCE ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
target_link_libraries(ChatClient  PRIVATE 
	Qt6::Core 
	Qt6::Qml 
	Qt6::Quick 
	Qt6::WebSockets 
	Qt6::Network
	Qt6::Concurrent
)
