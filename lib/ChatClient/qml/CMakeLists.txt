set(QML_FILES 	"AuthorizationDialog.qml"
	"ChatBox.qml"
	"ChatInput.qml"
	"MainWindow.qml"
	"RoomList.qml"
	"RoundedFrame.qml"
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

QT_ADD_LIBRARY(components STATIC)
QT_ADD_QML_MODULE(components
	URI components
	NO_PLUGIN
	QML_FILES
		${QML_FILES}
	RESOURCES
		${QML_RESOURCES}

)
file(MAKE_DIRECTORY ${tr_dir}) 

#qt_create_translation(QM_FILES ${QML_FILES} "${tr_dir}/comp_en.ts" "${tr_dir}/comp_ru.ts")
qt_add_lupdate(components TS_FILES "${tr_dir}/comp_en.ts" "${tr_dir}/comp_ru.ts")
set_source_files_properties("${tr_dir}/comp_en.ts" "${tr_dir}/comp_ru.ts"
    PROPERTIES OUTPUT_LOCATION ${APP_QM_DIR})
qt_add_lrelease(components 
    TS_FILES "${tr_dir}/comp_en.ts" "${tr_dir}/comp_ru.ts"
    QM_FILES_OUTPUT_VARIABLE qm_files)

##install(FILES ${qm_files} DESTINATION "${CMAKE_BINARY_DIR}/translations")
#add_custom_target(comp_translates  DEPENDS ${QM_FILES})
add_dependencies(components components_lupdate components_lrelease)


#install(FILES ${QM_FILES} DESTINATION ${APP_QM_DIR})
#add_custom_command(TARGET components POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy ${QM_FILES} ${APP_QM_DIR}
#)
