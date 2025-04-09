find_package(Qt6 REQUIRED COMPONENTS Core Gui Multimedia)
find_package(nlohmann_json CONFIG REQUIRED)

set(LIB_NAME Core)
set(LIB_SOURCES
	"res.qrc"
	"applicationsettings.h"
	"applicationsettings.cpp"
	"startupwindow.h"
	"startupwindow.cpp"
	"startupwindow.h" 
	"startupwindow.cpp" 
	"cache.h"
	"scopeguard.h"
	"data.h"
	"userhandle.h"
	"userhandle.cpp"
	"core.h"
	"data.cpp"
	"taskqueue.h"
	"taskqueue.cpp"
	"core_include.h"
	"models/groupmodel.h"
	"models/groupmodel.cpp"
	"models/messagemodel.h"
	"models/messagemodel.cpp"
	"models/participatemodel.h"
	"models/participatemodel.cpp"
	"models/usermodel.h"
	"models/usermodel.cpp"
	"models/identifyingmodel.cpp"
	"models/identifyingmodel.h"
	)
add_library(${LIB_NAME} SHARED)
add_library(${LIB_PREFIX}::${LIB_NAME} ALIAS ${LIB_NAME})
qt_add_qml_module(${LIB_NAME} 
    URI ${LIB_PREFIX}.Core
    VERSION 1.0
    NO_GENERATE_PLUGIN_SOURCE
    SOURCES ${LIB_SOURCES}
)
set_property(TARGET ${LIB_NAME} PROPERTY CXX_STANDARD 20)

target_compile_definitions(${LIB_NAME} PRIVATE 	
    CC_CORE_SHARED
)
target_include_directories(${LIB_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/models

)
target_link_libraries(${LIB_NAME}  PRIVATE 
    Qt6::Core
    Qt6::Gui
    Qt6::Multimedia
    nlohmann_json::nlohmann_json
)
include(GNUInstallDirs)
install(TARGETS ${LIB_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)   


