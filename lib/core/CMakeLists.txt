find_package(Qt6 REQUIRED COMPONENTS core Widgets Qml Quick WebSockets Network Concurrent QuickControls2) 

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(LIB_NAME Core)
set(LIB_SOURCES
	"res.qrc"
	"startupwindow.h"
	"startupwindow.cpp"
	"roommodel.h"
	"roommodel.cpp"
	"usersmodel.cpp"
	"usersmodel.h"
	"userinfo.h" 
	"userinfo.cpp" 
	"applicationsettings.h" 
	"startupwindow.h" 
	"startupwindow.cpp" 
	"applicationsettings.cpp"
	"messagemodel.h" 
	"messagemodel.cpp"
	"taskqueue.h"
	"taskqueue.cpp"
	"core_include.h"
	)
QT_ADD_LIBRARY(${LIB_NAME} SHARED)
qt_add_qml_module(${LIB_NAME} 
    URI ${LIB_PREFIX}.Core
	NO_GENERATE_PLUGIN_SOURCE
    SOURCES ${LIB_SOURCES}
)
target_compile_definitions(${LIB_NAME} PRIVATE 	
	CC_CORE_SHARED
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

