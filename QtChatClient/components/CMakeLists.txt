QT_ADD_LIBRARY(components STATIC)
QT_ADD_QML_MODULE(components
	URI "components"
	VERSION 1.0
    NO_PLUGIN
	QML_FILES 
        MainWindow.qml
        AuthorizationDialog.qml
        RoomList.qml
        InputBox.qml
        ChatInput.qml
        ChatBox.qml
  

)
set_target_properties(components PROPERTIES AUTOMOC ON)
set_target_properties(components PROPERTIES AUTORCC ON)
