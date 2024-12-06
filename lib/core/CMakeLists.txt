find_package(Qt6 REQUIRED COMPONENTS Core Gui Multimedia)

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
	"cache.h"
	"participatemodel.h"
	"participatemodel.cpp"
	"messagemodel.h" 
	"messagemodel.cpp"
	"taskqueue.h"
	"taskqueue.cpp"
	"idmodel.h"
	"idmodel.cpp"
	"core_include.h"
	)
add_library(${LIB_NAME} SHARED)
add_library(${LIB_PREFIX}::${LIB_NAME} ALIAS ${LIB_NAME})
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
	Qt6::Gui 
	Qt6::Multimedia 
)

