cmake_minimum_required (VERSION 3.8 )
project ("ChatClient" VERSION 0.0.1 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ORG_NAME "MyOrg")

find_package(Qt6 REQUIRED COMPONENTS Core  Qml Quick WebSockets Network Concurrent QuickControls2 LinguistTools)
set(CMAKE_AUTORCC ON)


qt_standard_project_setup(REQUIRES 6.5)
add_subdirectory (src)
add_subdirectory (lib)
#qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} appp_ru.ts)configure_file(${QM_FILES} ${CMAKE_SOURCE_DIR} COPYONLY)
