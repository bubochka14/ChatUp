import QtQuick

Image {
    id: root
    signal clicked()
    visible: enabled
    sourceSize.width: width
    sourceSize.height: height
    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
        cursorShape: Qt.PointingHandCursor
    }
}
