
find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick Gui QuickControls2 Multimedia)
find_package(nlohmann_json CONFIG REQUIRED)
qt_standard_project_setup(REQUIRES 6.8)
set(QWINDOWKIT_BUILD_QUICK ON)
set(APP_BINARY ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
set(APP_SOURCE ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
set(APP_QM_DIR "${CMAKE_BINARY_DIR}/translations" CACHE INTERNAL "")
file(REAL_PATH "../qml" LIB_PATH BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB_RECURSE  app_resources resources/*)
foreach(res IN LISTS app_resources)
    file(RELATIVE_PATH res_name ${CMAKE_CURRENT_SOURCE_DIR}/resources ${res})
    set_source_files_properties(${res} PROPERTIES
        QT_RESOURCE_ALIAS ${res_name}
    )
endforeach()
file(GLOB_RECURSE  qml_files qml/*)
foreach(qml IN LISTS qml_files)
    get_filename_component(qml_name ${qml} NAME)
    set_source_files_properties(${qml} PROPERTIES
        QT_RESOURCE_ALIAS ${qml_name}
    )
endforeach()
list(APPEND QML_IMPORT_PATHS "${LIB_PATH}")
set(QML_IMPORT_PATH "${QML_IMPORT_PATHS}" CACHE STRING "Qt Creator QML import paths")
set(APP_SOURCES
    app.cpp
    qmlaudiooutput.h
    qmlaudiooutput.cpp
    abstractwindowfactory.h
    abstractwindowfactory.cpp
    qmlchatwindow.cpp
    qmlprofileviewer.cpp
    qmlprofileviewer.h
    qmlchatwindow.h
    qmlstartupwindow.h
    qmlstartupwindow.cpp
    qmlwindowfactory.cpp
    main.cpp
    qmlwindowfactory.h
    abstractchatwindow.cpp
    app.h
    qmlobjectconverter.h
    )
qt_add_executable (app  ${APP_SOURCES}
)
qt_add_qml_module(app 
    URI app 
    VERSION 1.0
    QML_FILES ${qml_files}
    RESOURCES ${app_resources}
)
set_target_properties(
    app
    PROPERTIES OUTPUT_NAME "ChatUp"
               CXX_STANDARD 20
               CXX_STANDARD_REQUIRED YES
)
if(MSVC)
    set_target_properties(
        app
        PROPERTIES   LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
        LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

set_target_properties(app PROPERTIES CXX_STANDARD 17)
if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
        target_compile_options(app PUBLIC "/Zc:__cplusplus")
endif()
set_target_properties(app PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ChatUp
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)
include(GNUInstallDirs)
install(TARGETS app
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_compile_definitions(app
    PRIVATE APP_NAME="${CMAKE_PROJECT_NAME}"
    ORG_NAME="${ORG_NAME}"
    APP_VERSION="${CMAKE_PROJECT_VERSION}"
    QM_DIR="${APP_QM_DIR}"
)
target_link_libraries(app PRIVATE
    ChatClient::Core
    ChatClient::Network
    ChatClient::Media
    nlohmann_json::nlohmann_json
    Qt6::Core
    Qt6::QuickControls2
    Qt6::Qml
    Qt6::Quick
    Qt6::Gui
    Qt6::Multimedia
    quickfuture
    QWindowKit::Quick
    QWindowKit::Core
)