# CMakeList.txt: файл проекта CMake верхнего уровня; проведите глобальную конфигурацию
# и включите сюда подпроекты.
#
cmake_minimum_required (VERSION 3.8 )
project ("QtWSClient" VERSION 0.0.1 LANGUAGES CXX C)
list(APPEND CMAKE_PREFIX_PATH "C:/Qt/6.5.2/msvc2019_64")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(ORG_NAME "MyOrg")

find_package(Qt6 REQUIRED COMPONENTS Core  Qml Quick WebSockets Network Concurrent)


# Включите подпроекты.
add_subdirectory ("QtChatClient")
