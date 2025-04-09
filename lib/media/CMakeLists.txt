#find_package(FFMPEG REQUIRED)
include(FindPkgConfig)
find_package(PkgConfig)
set(ENV{PKG_CONFIG_PATH} /home/istomin/ffmpeg_build/lib/pkgconfig)
pkg_check_modules(FFmpeg REQUIRED
    libavcodec
    libavdevice
    libswscale
    libavfilter
    libpostproc
    libavutil
    libswresample
)
find_package(Qt6 REQUIRED COMPONENTS Core Multimedia MultimediaWidgets Concurrent)
find_package(nlohmann_json CONFIG REQUIRED)
set(LIB_NAME Media)
set(LIB_SOURCES
    "datapipe.h"
    "decoder.cpp"
    "decoder.h"
    "encoder.h"
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
)
add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
add_library(${LIB_PREFIX}::${LIB_NAME} ALIAS ${LIB_NAME}) 
target_compile_definitions(${LIB_NAME} PRIVATE 	
    CC_MEDIA_SHARED
)

target_compile_options(${LIB_NAME} PRIVATE -Wall -O3 -fPIC -Bsymbolic)
target_include_directories(${LIB_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${FFmpeg_INCLUDE_DIRS}

)
target_link_directories(${LIB_NAME} PRIVATE ${FFmpeg_LIBRARY_DIRS})
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
message("AAAAAAAAAA${FFmpeg_INCLUDE_DIRS}")
target_compile_options(${LIB_NAME} PUBLIC -Wall -O3 -fPIC -Bsymbolic)
target_link_libraries(${LIB_NAME}  PRIVATE 
    Core
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::Concurrent
    Qt6::Multimedia Qt6::MultimediaWidgets
    nlohmann_json::nlohmann_json

    ${FFmpeg_LIBRARIES}
)

