import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChatClient

Rectangle {
    id: delegate
    property bool hovered: false
    property alias label: label
    property alias subLabel: subLabel
    property Item icon
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
            implicitHeight: delegate.icon.height
            implicitWidth: delegate.icon.width
            padding: 0
        }

        Column {
            Label {
                id: label
                font.pixelSize: 16
            }
            Label {
                id:subLabel
                font.pixelSize: 12
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
    }
}
