find_package(Qt6 REQUIRED COMPONENTS Core Widgets Qml Quick WebSockets Network Concurrent QuickControls2) 

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(LIB_NAME Calls)

set(LIB_SOURCES
	"callcontroller.cpp"
	"callcontroller.h"
)
QT_ADD_LIBRARY(${LIB_NAME} SHARED)
qt_add_qml_module(${LIB_NAME} 
    URI ${LIB_PREFIX}.Calls
	NO_GENERATE_PLUGIN_SOURCE
    SOURCES ${LIB_SOURCES}
)
target_compile_definitions(${LIB_NAME} PRIVATE 	
	CC_CALLS_SHARED
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