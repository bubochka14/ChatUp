import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChatClient

Rectangle {
    id: delegate
    property bool hovered: false
    property string lastMessage
    required property string name
    required property var id
    property Item icon: Rectangle {
        height: 50
        width: 50
        radius: 20
        color: "#19182a"
        Image {
            id: icon
            anchors.centerIn: parent
            source: "pics/users.svg"
            sourceSize.height: 30
            sourceSize.width: 30
        }
    }
    signal clicked
    height: 70
    radius: 10
    color: hovered ? "#3e3c62" : "transparent"
    RowLayout {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
        // anchors.right: parent.right
        Control
        {
            contentItem: delegate.icon
            height: delegate.icon.height
            width: delegate.icon.width
            padding: 0
        }

        Column {
            Label {
                text: delegate.name
                font.bold: true
                font.pixelSize: 16
            }
            Label {
                font.pixelSize: 12
                text: delegate.lastMessage
            }
        }
    }
    MouseArea {
        hoverEnabled: true
        anchors.fill: parent
        preventStealing: true
        cursorShape: Qt.PointingHandCursor
        onEntered: hovered = true
        onExited: hovered = false
        onClicked: mouse => {
                       delegate.clicked()
                   }
        // onExited: listView.currentIndex = -1
    }
}
