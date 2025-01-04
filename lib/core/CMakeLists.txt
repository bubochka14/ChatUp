find_package(Qt6 REQUIRED COMPONENTS Core Gui Multimedia)
set(LIB_NAME Core)
set(LIB_SOURCES
	"res.qrc"
	"startupwindow.h"
	"startupwindow.cpp"
	"applicationsettings.h" 
	"startupwindow.h" 
	"startupwindow.cpp" 
	"applicationsettings.cpp"
	"cache.h"
	"scopeguard.h"
	"data.h"
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
	NO_GENERATE_PLUGIN_SOURCE
    SOURCES ${LIB_SOURCES}
)
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
)
include(GNUInstallDirs)
install(TARGETS ${LIB_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)   


