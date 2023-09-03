import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    property var currentRoomID: -1
    Rectangle {
        id: rect
        anchors.fill: parent
        ListView {
            id: listView
            spacing: 5
            currentIndex: -1
            model : roomModel
            height: parent.height
            width: parent.width
            delegate: roomListDelegate
            highlight: Rectangle {
                color: "lightgrey"
                radius: 5
            }
        }
        border.width: 1
    }
    Component {
        id: roomListDelegate
        Item {
            width: listView.width
            height: 50
            Text {
                text: roomName + " " + roomID
            }
            MouseArea {
                hoverEnabled: true
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onEntered: listView.currentIndex = index
                onClicked: selectRoomID(roomID)
                onExited: listView.currentIndex = -1
            }
        }
    }
    function selectRoomID(roomID: int) {
        currentRoomID = roomID;
    }
}
