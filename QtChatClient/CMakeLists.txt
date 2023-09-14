find_package(Qt6 REQUIRED COMPONENTS Core  Qml Quick WebSockets Network Concurrent QuickControls2 LinguistTools) 
set(APP_NAME ${PROJECT_NAME})
add_executable (${APP_NAME} 
	"main.cpp" 
	"wsclient.cpp" 
	"wsclient.h" 
	"authenticationmaster.h" 
    "res.qrc"
	"messageconstructor.cpp"
	"messageconstructor.h"
	"authenticationmaster.cpp"
	"singletonholder.cpp" 
	"singletonholder.h"
	"chatroommodel.h"
	"chatroommodel.cpp"
	"chatroomstructs.h" 
	"chatroomstructs.cpp" 
	"chatclient.h" 
	"applicationsettings.h" 
	"applicationsettings.cpp"
	"chatclient.cpp"
	"userverifydialog.h"
	"userverifydialog.cpp"
 "servermethodcaller.h" "servermethodcaller.cpp" "chatwindow.h" "chatwindow.cpp" "quickwindow.h" "MessageHistoryModel.h" "MessageHistoryModel.cpp")
 file(REAL_PATH "../components" LIB_PATH BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND QML_IMPORT_PATHS "${LIB_PATH}")
set(QML_IMPORT_PATH "${QML_IMPORT_PATHS}" CACHE STRING "Qt Creator QML import paths")


set(APP_BINARY ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
set(APP_SOURCE ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
set(APP_QM_DIR "${CMAKE_BINARY_DIR}/translations" CACHE INTERNAL "")
add_custom_command(TARGET ${APP_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_QM_DIR})

target_compile_definitions(${APP_NAME} PRIVATE APP_NAME="${APP_NAME}" ORG_NAME="${ORG_NAME}" APP_VERSION="${CMAKE_PROJECT_VERSION}" QM_DIR="${APP_QM_DIR}") 
target_link_libraries(${APP_NAME}  PRIVATE 
	Qt6::Core 
	Qt6::Qml 
	Qt6::Quick 
	Qt6::WebSockets 
	Qt6::Network
	Qt6::Concurrent
	Qt6::QuickControls2
	components
	
)


target_include_directories(${APP_NAME} 
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
)
add_subdirectory(components)


#qt_add_lupdate(components TS_FILES "${APP_SOURCC_DIR}/translations/comp_ru.ts" "${APP_SOURCC_DIR}/translations/comp_en.ts")
#set_source_files_properties(comp_ru.ts comp_en.ts
#    PROPERTIES OUTPUT_LOCATION "${APP_BINARY_DIR}/translations")
#qt_add_lrelease(components TS_FILES "${APP_SOURCC_DIR}/translations/comp_ru.ts" "${APP_SOURCC_DIR}/translations/comp_en.ts")


#qt_add_lrelease(${APP_NAME} TS_FILES app_ru.ts arr_en.ts)
