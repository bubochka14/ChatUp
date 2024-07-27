QT_ADD_QML_MODULE(componentsUI STATIC
	URI components.ui
	NO_PLUGIN
	QML_FILES
		RoundedFrame.qml
		MessageCloud.qml
		TextLabel.qml
		ImageButton.qml
)
target_link_libraries(componentsUI PRIVATE Qt::QmlPrivate Qt::QuickPrivate)
