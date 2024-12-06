import QtQuick
import QtQuick.Controls
Item {
    id:root
    signal clicked
    property string name
    property var iconSource
    height: 50
    width: 70
    Column {
        anchors.fill: parent
        spacing: 3
        Image {
            id: icon
            anchors.horizontalCenter: parent.horizontalCenter
            mipmap: true
            sourceSize.width: 25
            sourceSize.height: 25
            source: iconSource
        }
        Label {
            id: text
            anchors.horizontalCenter: parent.horizontalCenter
            text: name
            font.family: "Helvetica"
        }
    }
    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            root.clicked()
        }
    }
}
