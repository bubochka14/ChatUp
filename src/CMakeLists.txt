find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick LinguistTools) 
set(APP_BINARY ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
set(APP_SOURCE ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
set(APP_QM_DIR "${CMAKE_BINARY_DIR}/translations" CACHE INTERNAL "")
set(APP_SOURCES
	${APP_SOURCE}/main.cpp
	${APP_SOURCE}/app.cpp
	${APP_SOURCE}/abstractwindowfactory.h
	${APP_SOURCE}/abstractwindowfactory.cpp
	${APP_SOURCE}/qmlchatwindow.cpp
	${APP_SOURCE}/qmlchatwindow.h
	${APP_SOURCE}/qmlstartupwindow.h
	${APP_SOURCE}/qmlstartupwindow.cpp
	${APP_SOURCE}/qmlwindowfactory.cpp
	${APP_SOURCE}/qmlwindowfactory.h
	${APP_SOURCE}/abstractwindowfactory.h
	${APP_SOURCE}/abstractwindowfactory.cpp
	${APP_SOURCE}/abstractchatwindow.cpp
	${APP_SOURCE}/abstractchatwindow.cpp
	${APP_SOURCE}/app.h
	${APP_SOURCE}/qmlobjectconverter.h
	${APP_SOURCE}/qml/qml.qrc
	${APP_SOURCE}/resources/resources.qrc
	CACHE INTERNAL "")
add_executable (app 
	${APP_SOURCES})
file(REAL_PATH "../qml" LIB_PATH BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND QML_IMPORT_PATHS "${LIB_PATH}")
set(QML_IMPORT_PATH "${QML_IMPORT_PATHS}" CACHE STRING "Qt Creator QML import paths")
#file(REAL_PATH "../components" LIB_PATH BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
#list(APPEND QML_IMPORT_PATHS "${LIB_PATH}")
#set(QML_IMPORT_PATH "${QML_IMPORT_PATHS}" CACHE STRING "Qt Creator QML import paths")


add_custom_command(TARGET app PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_QM_DIR})  
set_target_properties(app PROPERTIES CXX_STANDARD 17)
if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
	target_compile_options(app PUBLIC "/Zc:__cplusplus")
endif()
target_include_directories(app PRIVATE C:/Users/Denis/source/repos/QtChatClient/lib/network
    C:/Users/Denis/source/repos/QtChatClient/lib/core)
target_compile_definitions(app
	PRIVATE APP_NAME="${CMAKE_PROJECT_NAME}" 
	ORG_NAME="${ORG_NAME}" 
	APP_VERSION="${CMAKE_PROJECT_VERSION}" 
	QM_DIR="${APP_QM_DIR}"
) 
target_link_libraries(app PRIVATE 
	Core
	network
#	Media
	Calls
	Qt6::Core 
	Qt6::Qml 
	Qt6::Quick 	
#	quickfuture
	QWindowKit::Quick
	QWindowKit::Core
)

#qt_add_lupdate(components TS_FILES "${APP_SOURCC_DIR}/translations/comp_ru.ts" "${APP_SOURCC_DIR}/translations/comp_en.ts")
#set_source_files_properties(comp_ru.ts comp_en.ts
#    PROPERTIES OUTPUT_LOCATION "${APP_BINARY_DIR}/translations")
#qt_add_lrelease(components TS_FILES "${APP_SOURCC_DIR}/translations/comp_ru.ts" "${APP_SOURCC_DIR}/translations/comp_en.ts")


#qt_add_lrelease(${APP_NAME} TS_FILES app_ru.ts arr_en.ts)
