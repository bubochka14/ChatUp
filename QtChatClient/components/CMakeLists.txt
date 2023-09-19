set(QML_FILES 	
	"AuthorizationDialog.qml"
	"ChatBox.qml"
	"ChatInput.qml"
	"MainWindow.qml"
	"RoomList.qml"
    "UserList.qml"
    "EmojiList.qml"
    )
set(QML_RESOURCES 
	"pics/clear.png"
	"pics/fileicon.png"
	"pics/emoji.svg"
	"emojies/emojies.txt"
)
set(tr_dir "${CMAKE_CURRENT_SOURCE_DIR}/translations")
QT_ADD_QML_MODULE(components
	URI components
	QML_FILES
		${QML_FILES}
	RESOURCES
		${QML_RESOURCES}
	SOURCES
		chatwindow.cpp
	chatwindow.h
	userverifydialog.cpp
	userverifydialog.h
	components_global.h
	DEPENDENCIES 
		Quick
	IMPORT_PATH
		"${CMAKE_CURRENT_BINARY_DIR}/ui"

)
target_link_libraries(components PRIVATE Qt::QmlPrivate Qt::QuickPrivate componentsUI)
target_compile_definitions(components PRIVATE SHARED=true)

file(MAKE_DIRECTORY ${tr_dir}) 


#qt_create_translation(QM_FILES ${QML_FILES} "${tr_dir}/comp_en.ts" "${tr_dir}/comp_ru.ts")
qt_add_lupdate(components TS_FILES "${tr_dir}/comp_en.ts" "${tr_dir}/comp_ru.ts")
set_source_files_properties("${tr_dir}/comp_en.ts" "${tr_dir}/comp_ru.ts"
    PROPERTIES OUTPUT_LOCATION ${APP_QM_DIR})
qt_add_lrelease(components 
    TS_FILES "${tr_dir}/comp_en.ts" "${tr_dir}/comp_ru.ts"
    QM_FILES_OUTPUT_VARIABLE qm_files)
target_include_directories(components PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE  ${CMAKE_CURRENT_BINARY_DIR})
add_dependencies(components components_lupdate components_lrelease)
add_subdirectory(ui)