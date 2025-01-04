find_package(FFMPEG REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Multimedia MultimediaWidgets Concurrent)

set(LIB_NAME Media)
set(LIB_SOURCES
	"datapipe.h"
	"decoder.cpp"
	"decoder.h"
	"encoder.h"
	"encoder.cpp"
	"media.h"
	"microphoneinput.h"
	"microphoneinput.cpp"
	"camerainput.h"
	"camerainput.cpp"
	"audiooutput.h"
	"media_include.h"
	)
add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
#add_library(${LIB_PREFIX}::${LIB_NAME} ALIAS ${LIB_NAME})
target_compile_definitions(${LIB_NAME} PRIVATE 	
	CC_MEDIA_SHARED
)
target_include_directories(${LIB_NAME} PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}
	PUBLIC ${FFMPEG_INCLUDE_DIRS}
)
target_link_directories(${LIB_NAME} PUBLIC ${FFMPEG_LIBRARY_DIRS})

include(GNUInstallDirs)
install(TARGETS ${LIB_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)   
target_link_libraries(${LIB_NAME}  PRIVATE 
	Core
	Qt6::Core 
	Qt6::Concurrent
	Qt6::Multimedia Qt6::MultimediaWidgets
	${FFMPEG_LIBRARIES}
	avdevice
)
