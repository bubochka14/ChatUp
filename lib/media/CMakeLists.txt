find_package(Qt6 REQUIRED COMPONENTS Core Multimedia MultimediaWidgets Concurrent)
find_package(FFMPEG REQUIRED)
set(LIB_NAME Media)
set(LIB_SOURCES
    "datapipe.h"
    "decoder.cpp"
    "decoder.h"
    "encoder.h"
    "filters.h"
    "filters.cpp"
    "encoder.cpp"
    "media.h"
    "media.cpp"
    "microphoneinput.h"
    "audiooutput.h"
    "audiooutput.cpp"
    "microphoneinput.cpp"
    "camerainput.h"
    "camerainput.cpp"
    "audiooutput.h"
    "pipelines.h"
    "pipelines.cpp"
    "media_include.h"
    "rtppacketizer.h"
    "rtppacketizer.cpp"
    "mediacontext.h"
    "mediacontext.cpp"
    "demuxer.h"
    "demuxer.cpp"
)
add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
add_library(${LIB_PREFIX}::${LIB_NAME} ALIAS ${LIB_NAME}) 
target_compile_definitions(${LIB_NAME} PRIVATE 	
    CC_MEDIA_SHARED
)
target_include_directories(${LIB_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FFMPEG_INCLUDE_DIRS}

)
target_link_directories(${LIB_NAME} PUBLIC ${FFMPEG_LIBRARY_DIRS})
set_target_properties(app PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)
include(GNUInstallDirs)
install(TARGETS ${LIB_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)   
list(APPEND FFMPEG_LIBRARIES 	
    avdevice
    swresample
	swscale
    avfilter
)

target_link_libraries(${LIB_NAME} PUBLIC 
    ChatClient::Core
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::Concurrent
    Qt6::Multimedia Qt6::MultimediaWidgets
    ${FFMPEG_LIBRARIES}
)

