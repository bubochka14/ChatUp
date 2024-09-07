find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick LinguistTools) 
set(APP_SOURCES
	main.cpp
	app.cpp
	app.h
        res.qrc
)
add_executable (app
	${APP_SOURCES}
)
#file(REAL_PATH "../components" LIB_PATH BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
#list(APPEND QML_IMPORT_PATHS "${LIB_PATH}")
#set(QML_IMPORT_PATH "${QML_IMPORT_PATHS}" CACHE STRING "Qt Creator QML import paths")

set(APP_BINARY ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
set(APP_SOURCE ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
set(APP_QM_DIR "${CMAKE_BINARY_DIR}/translations" CACHE INTERNAL "")

add_custom_command(TARGET app PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_QM_DIR})

target_compile_definitions(app
	PRIVATE APP_NAME="${CMAKE_PROJECT_NAME}" 
	ORG_NAME="${ORG_NAME}" 
	APP_VERSION="${CMAKE_PROJECT_VERSION}" 
	QM_DIR="${APP_QM_DIR}"
) 
target_link_libraries(app PRIVATE 
	Qt6::Core 
	Qt6::Qml 
	Qt6::Quick 
	ChatClient	

)

#qt_add_lupdate(components TS_FILES "${APP_SOURCC_DIR}/translations/comp_ru.ts" "${APP_SOURCC_DIR}/translations/comp_en.ts")
#set_source_files_properties(comp_ru.ts comp_en.ts
#    PROPERTIES OUTPUT_LOCATION "${APP_BINARY_DIR}/translations")
#qt_add_lrelease(components TS_FILES "${APP_SOURCC_DIR}/translations/comp_ru.ts" "${APP_SOURCC_DIR}/translations/comp_en.ts")


#qt_add_lrelease(${APP_NAME} TS_FILES app_ru.ts arr_en.ts)
