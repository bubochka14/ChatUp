import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property bool hovered: false
    property alias label: label
    property alias subLabel: subLabel
    property Item icon :Item{}
    signal clicked()
    height: 70
    radius: 10
    color: hovered ? "#303552" : "transparent"
    RowLayout {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
        // anchors.right: parent.right
        Control
        {
            contentItem: root.icon
            implicitHeight: root.icon.height
            implicitWidth: root.icon.width
            padding: 0
        }

        Column {
            Label {
                id: label
                font.pointSize: 14
            }
            Label {
                id:subLabel
                font.pointSize: 12
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
                       root.clicked()
                   }
    }
}
