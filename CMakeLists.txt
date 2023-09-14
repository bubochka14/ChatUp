# CMakeList.txt: файл проекта CMake верхнего уровня; проведите глобальную конфигурацию
# и включите сюда подпроекты.
#
cmake_minimum_required (VERSION 3.8 )
project ("QtWSClient" VERSION 0.0.1 LANGUAGES CXX C)
list(APPEND CMAKE_PREFIX_PATH "C:/Qt/6.5.2/msvc2019_64")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ORG_NAME "MyOrg")

find_package(Qt6 REQUIRED COMPONENTS Core  Qml Quick WebSockets Network Concurrent QuickControls2 LinguistTools)
set(CMAKE_AUTORCC ON)


qt_standard_project_setup(REQUIRES 6.5)
# Включите подпроекты.
add_subdirectory ("QtChatClient")
#qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} appp_ru.ts)configure_file(${QM_FILES} ${CMAKE_SOURCE_DIR} COPYONLY)
